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


typedef struct ResponseItem {
    NCodecDiagSrpcMessage msg;
    ffi_sarg              result;
} ResponseItem;

void service_process_requests(ServiceManager* sm)
{
    if (sm == NULL) return;

    for (size_t i = 0; i < vector_len(&sm->groups); i++) {
        ServiceGroup* group = vector_at(&sm->groups, i, NULL);
        if (group == NULL || group->nc == NULL) continue;

        Vector response = vector_make(sizeof(ResponseItem), 0, NULL);

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

            /* Dispatch the request. */
            ffi_sarg result = 0;
            int      rc = service_dispatch(sm, call, &result);
            if (rc != 0) {
                log_error(group->nc,
                    "Service:Process: dispatch failed (call_id=%u, rc=%d)",
                    msg.call_id, rc);
            }
            if (rc == 0 && !call->ctx.response_required) continue;

            /* Build the response (shallow copy of the request context). */
            NCodecDiagSrpcMessage ret = msg;
            ret.payload_type = NCodecDiagSrpcPayloadTypeInvokeReturn;
            memset(&ret.payload, 0, sizeof(ret.payload));
            ret.payload.invoke_return.return_value = (uint8_t)result;
            ret.payload.invoke_return.error_code = (uint8_t)(rc < 0 ? -rc : rc);
            ret.payload.invoke_return.ctx = call->ctx;
            ret.payload.invoke_return.ctx.request = NULL;
            ret.payload.invoke_return.ctx.request_len = 0;
            ret.payload.invoke_return.ctx.response = NULL;
            ret.payload.invoke_return.ctx.response_len = 0;
            ResponseItem item = { .msg = ret, .result = result };
            vector_push(&response, &item);
        }

        ncodec_truncate(group->nc);
        for (size_t j = 0; j < vector_len(&response); j++) {
            ResponseItem*          item = vector_at(&response, j, NULL);
            NCodecDiagSrpcMessage* ret = &item->msg;
            // The result is returned (native ffi_sarg) in ctx.response.
            ret->payload.invoke_return.ctx.response =
                (const uint8_t*)&item->result;
            ret->payload.invoke_return.ctx.response_len = sizeof(ffi_sarg);
            if (ncodec_write(group->nc, (NCodecMessage*)ret) < 0) {
                log_error(group->nc,
                    "Service:Process: response write failed (call_id=%u)",
                    ret->call_id);
            }
        }
        ncodec_flush(group->nc);
        vector_reset(&response);
    }
}


int service_dispatch(
    ServiceManager* sm, NCodecDiagSrpcInvokeCall* call, ffi_sarg* result)
{
    if (sm == NULL || call == NULL || call->service == NULL ||
        call->function == NULL) {
        return -EINVAL;
    }

    // Locate the service group.
    ServiceGroup  group_key = { .name = call->service };
    ServiceGroup* group = vector_find(&sm->groups, &group_key, 0, NULL);
    if (group == NULL) {
        return -ENOENT;
    }

    // Locate the function.
    ServiceFunction  func_key = { .name = call->function };
    ServiceFunction* func = vector_find(&group->funcs, &func_key, 0, NULL);
    if (func == NULL) {
        log_error(group->nc,
            "Service:Dispatch: function not registered (service=%s, "
            "function=%s)",
            call->service, call->function);
        return -ENOENT;
    }

    // Generate the arguments.
    CallArguments arguments = { 0 };
    int           rc = call_arguments_decode(call, func, &arguments);
    if (rc != 0) {
        /* Log/report the decoding error through the service error path. */
        call_arguments_destroy(&arguments);
        return rc;
    }

    // Call the function.
    ffi_sarg _result = 0;
    ffi_call(&func->cif, func->fn_ptr, &_result, arguments.args.items);
    call_arguments_destroy(&arguments);
    if (result) *result = _result;

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
