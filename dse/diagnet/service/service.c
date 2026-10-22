// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ffi.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/diag/codec.h>
#include <dse/diagnet/service/service.h>


/* Both comparators work on structs whose first member is `const char* name`.
 * Lookups pass a stack key with only `name` set. */
static int _name_compar(const void* left, const void* right)
{
    const char* l = *(const char* const*)left;
    const char* r = *(const char* const*)right;
    if (l && r) return strcmp(l, r);
    return 0;
}

static void _func_destroy(void* item, void* data)
{
    (void)data;
    ServiceFunction* func = item;
    free(func->arg_types);
    func->arg_types = NULL;
}

static void _group_destroy(void* item, void* data)
{
    (void)data;
    ServiceGroup* group = item;
    vector_clear(&group->funcs, _func_destroy, NULL);
    vector_reset(&group->funcs);
}

static void _request_destroy(void* item, void* data)
{
    (void)data;
    service_ctx_delete(*(NCodecDiagSrpcMessageContext**)item);
}


ServiceManager service_manager_create(
    const char* ecu_name, uint32_t ecu_id, uint32_t swc_id)
{
    return (ServiceManager){
        .ecu_name = ecu_name,
        .ecu_id = ecu_id,
        .swc_id = swc_id,
        .groups = vector_make(sizeof(ServiceGroup), 0, _name_compar),
        .requests = vector_make(sizeof(NCodecDiagSrpcMessageContext*), 0, NULL),
    };
}


int service_register(ServiceManager* sm, void* nc, const char* name,
    ServiceFunction registry[], size_t count)
{
    if (sm == NULL || name == NULL || registry == NULL) return -EINVAL;

    // The group must not already exist.
    ServiceGroup key = { .name = name };
    if (vector_find(&sm->groups, &key, 0, NULL) != NULL) return -EEXIST;
    ServiceGroup new_group = {
        .name = name,
        .nc = nc,
        .funcs = vector_make(sizeof(ServiceFunction), 0, _name_compar),

        /* VTable interface - defaults. */
        .vtable = {
            .dispatch = service_dispatch,
        },
    };
    vector_push(&sm->groups, &new_group);
    vector_sort(&sm->groups);
    ServiceGroup* group = vector_find(&sm->groups, &key, 0, NULL);
    if (group == NULL) return -ENOMEM;

    int rc = 0;

    for (size_t i = 0; i < count; i++) {
        // Deep copy of func.
        ServiceFunction func = registry[i];
        memset(&func.cif, 0, sizeof(func.cif));
        func.arg_types =
            calloc(func.arg_count > 0 ? func.arg_count : 1, sizeof(ffi_type*));
        if (func.arg_types == NULL) {
            log_error(group->nc, "calloc failed (function=%s)", func.name);
            rc = -ENOMEM;
            continue;
        }
        if (func.arg_count > 0) {
            memcpy(func.arg_types, registry[i].arg_types,
                sizeof(ffi_type*) * func.arg_count);
        }

        // Check the CIF is valid.
        ffi_status status = ffi_prep_cif(&func.cif, FFI_DEFAULT_ABI,
            func.arg_count, func.return_type, func.arg_types);
        if (status == FFI_OK) {
            vector_push(&group->funcs, &func);  // shallow copy -> vector.
        } else {
            free(func.arg_types);
            log_error(group->nc, "ffi_prep_cif failed (function=%s, status=%d)",
                func.name, status);
            rc = -EINVAL;
        }
    }
    vector_sort(&group->funcs);
    return rc;
}


int service_set_dispatch(
    ServiceManager* sm, const char* name, ServiceDispatch func)
{
    if (sm == NULL || name == NULL) return -EINVAL;

    ServiceGroup  key = { .name = name };
    ServiceGroup* group = vector_find(&sm->groups, &key, 0, NULL);
    if (group == NULL) return -ENOENT;
    group->vtable.dispatch = func;

    return 0;
}


void service_process_requests(ServiceManager* sm)
{
    if (sm == NULL) return;

    for (size_t i = 0; i < vector_len(&sm->groups); i++) {
        ServiceGroup* group = vector_at(&sm->groups, i, NULL);
        if (group == NULL || group->nc == NULL) continue;
        if (group->vtable.dispatch == NULL) continue;

        Vector response = vector_make(sizeof(NCodecDiagSrpcMessage), 0, NULL);

        // Drain the stream; ncodec_read returns a negative value when empty.
        while (1) {
            NCodecDiagSrpcMessage msg = { 0 };
            if (ncodec_read(group->nc, (NCodecMessage*)&msg) < 0) break;
            if (msg.payload_type != NCodecDiagSrpcPayloadTypeInvokeCall) {
                continue;
            }
            NCodecDiagSrpcInvokeCall* call = &msg.payload.invoke_call;

            /* Filter the request. */
            if (msg.target.ecu_name) {
                if (strcmp(msg.target.ecu_name, sm->ecu_name) != 0) {
                    continue;
                }
            } else {
                if (msg.target.ecu_id != sm->ecu_id) {
                    continue;
                }
            }
            if (msg.target.swc_id != sm->swc_id) {
                continue;
            }

            /* Force the call response to NULL, even if in flatbuffer. */
            call->ctx.response = NULL;
            call->ctx.response_len = 0;

            /* Dispatch the request. */
            NCodecDiagSrpcInvokeReturn* ret = NULL;
            int rc = group->vtable.dispatch(sm, call, &ret);
            if (rc != 0) {
                log_error(group->nc,
                    "Service:Process: dispatch failed (call_id=%u, rc=%d)",
                    msg.call_id, rc);
            }
            if (rc == 0 && ret == NULL) continue; /* OK and no response. */

            /* Build the response (shallow copy of InvokeReturn). */
            NCodecDiagSrpcMessage response_msg = {
                /* Message metadata. */
                .call_id = msg.call_id,
                .target = msg.target,
                .origin = msg.origin,
                .payload_type = NCodecDiagSrpcPayloadTypeInvokeReturn,
                /* Context metadata from request. */
                .payload.invoke_return.ctx = call->ctx,
                .payload.invoke_return.ctx.request = NULL,
                .payload.invoke_return.ctx.request_len = 0,
                .payload.invoke_return.ctx.response = NULL,
                .payload.invoke_return.ctx.response_len = 0,
            };
            if (ret != NULL) {
                if (ret->ctx.response) {
                    if (ret->ctx.response == call->ctx.request) {
                        /* Response is reserved memory from flatbuffer,
                         * deepcopy. */
                        uint8_t* buf = malloc(ret->ctx.response_len);
                        if (buf) {
                            memcpy(
                                buf, ret->ctx.response, ret->ctx.response_len);
                        } else {
                            ret->ctx.response_len = 0;
                        }
                        ret->ctx.response = buf;
                    }
                }
                if (ret->ctx.request) {
                    if (ret->ctx.request != call->ctx.request) {
                        free((void*)ret->ctx.request);
                    }
                    /* Force the request to NULL in the return. */
                    ret->ctx.request = NULL;
                    ret->ctx.request_len = 0;
                }

                /* Set the response. */
                response_msg.payload.invoke_return = *ret;
                free(ret);
            }
            response_msg.payload.invoke_return.error_code = rc < 0 ? -rc : rc;
            vector_push(&response, &response_msg);
        }

        ncodec_truncate(group->nc);
        for (size_t j = 0; j < vector_len(&response); j++) {
            NCodecDiagSrpcMessage* msg = vector_at(&response, j, NULL);

            if (ncodec_write(group->nc, (NCodecMessage*)msg) < 0) {
                log_error(group->nc,
                    "Service:Process: response write failed (call_id=%u)",
                    msg->call_id);
            }

            /* Note: ctx.response is const because if can be from a flatbuffer.
             */
            free((void*)msg->payload.invoke_return.ctx.request);
            free((void*)msg->payload.invoke_return.ctx.response);
        }
        ncodec_flush(group->nc);
        vector_reset(&response);
    }
}


int service_dispatch(ServiceManager* sm, const NCodecDiagSrpcInvokeCall* call,
    NCodecDiagSrpcInvokeReturn** ret)
{
    if (sm == NULL || call == NULL || call->service == NULL ||
        call->function == NULL) {
        return -EINVAL;
    }
    if (ret) *ret = NULL; /* Set no response as default. */

    /* Locate the service group. */
    ServiceGroup  group_key = { .name = call->service };
    ServiceGroup* group = vector_find(&sm->groups, &group_key, 0, NULL);
    if (group == NULL) {
        return -ENOENT;
    }

    /* Locate the function. */
    ServiceFunction  func_key = { .name = call->function };
    ServiceFunction* func = vector_find(&group->funcs, &func_key, 0, NULL);
    if (func == NULL) {
        log_error(group->nc,
            "Service:Dispatch: function not registered (service=%s, "
            "function=%s)",
            call->service, call->function);
        return -ENOENT;
    }

    /* Generate the arguments. */
    CallArguments arguments = { 0 };
    int           rc = call_arguments_decode(call, func, &arguments);
    if (rc != 0) {
        /* Log/report the decoding error through the service error path. */
        call_arguments_destroy(&arguments);
        return rc;
    }

    /* Call the function. */
    ffi_sarg _result = 0;
    ffi_call(&func->cif, func->fn_ptr, &_result, arguments.args.items);
    call_arguments_destroy(&arguments);

    /* Generate the response. */
    if (!call->ctx.response_required || ret == NULL) return 0;

    uint8_t* buf = malloc(sizeof(ffi_sarg));
    if (buf == NULL) return -ENOMEM;
    memcpy(buf, &_result, sizeof(ffi_sarg));

    NCodecDiagSrpcInvokeReturn* _ret = calloc(1, sizeof(*_ret));
    if (_ret == NULL) {
        free(buf);
        return -ENOMEM;
    }
    _ret->return_value = (uint32_t)_result;
    _ret->ctx = call->ctx;
    _ret->ctx.request = NULL;
    _ret->ctx.request_len = 0;
    _ret->ctx.response = buf;
    _ret->ctx.response_len = sizeof(ffi_sarg);

    /* Return the response. */
    *ret = _ret;
    return 0;
}


void service_destroy(ServiceManager* sm)
{
    if (sm == NULL) return;
    vector_clear(&sm->groups, _group_destroy, NULL);
    vector_reset(&sm->groups);
    vector_clear(&sm->requests, _request_destroy, NULL);
    vector_reset(&sm->requests);
}
