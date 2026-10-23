// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <errno.h>
#include <ffi.h>
#include <string.h>
#include <dse/ncodec/codec.h>
#include <dse/diagnet/service/service.h>
#include <dse/diagnet/examples/plant/plant.h>


enum {
    PLANT_SERVICE_SUCCESS = 0,
    PLANT_SERVICE_FAILURE = 1,
};

#define ECU_NAME     "vECU"
#define ECU_ID       1
#define SWC_ID       0
#define SERVICE_NAME "Plant"


static ffi_type* _plant_args[] = {
    &ffi_type_uint8,
    &ffi_type_pointer,
    &ffi_type_pointer,
};

static ServiceFunction _registry[] = {
    { .name = "clear",
        .fn_ptr = (void (*)(void))plant_clear,
        .return_type = &ffi_type_uint8,
        .arg_count = 3,
        .arg_types = _plant_args },
    { .name = "append",
        .fn_ptr = (void (*)(void))plant_append,
        .return_type = &ffi_type_uint8,
        .arg_count = 3,
        .arg_types = _plant_args },
    { .name = "read",
        .fn_ptr = (void (*)(void))plant_read,
        .return_type = &ffi_type_uint8,
        .arg_count = 3,
        .arg_types = _plant_args },
};


static int client_request(
    ServiceManager* sm, const char* func, const char* text)
{
    static uint8_t context_id;
    NCODEC*        nc = NULL;
    int            rc = 0;

    for (size_t i = 0; i < vector_len(&sm->groups); i++) {
        ServiceGroup* group = vector_at(&sm->groups, i, NULL);
        if (group && strcmp(group->name, SERVICE_NAME) == 0) nc = group->nc;
    }
    if (nc == NULL) return -ENOENT;

    NCodecDiagSrpcMessageContext ctx = {
        .context_id = ++context_id,
        .response_required = (strcmp(func, "read") == 0),
    };
    if (text != NULL) {
        ctx.request = (uint8_t*)text;
        ctx.request_len = strlen(text);
    }

    rc =
        service_request(sm, ECU_NAME, ECU_ID, SWC_ID, SERVICE_NAME, func, &ctx);
    if (rc != 0) goto reset;

    /* Cycle the transport/server (single stream, rewind to read). */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    service_process_requests(sm);

    /* Get any responses. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    if (ctx.response_required) {
        NCodecDiagSrpcMessageContext* response = NULL;
        int32_t                       error_code = 0;
        while (service_response(sm, &response, NULL, &error_code) >= 0) {
            if (response->context_id == ctx.context_id) {
                if (response->response) {
                    printf("%s: returned %.*s (context_id %u)\n", func,
                        (int)response->response_len, response->response,
                        response->context_id);
                }
            }
            service_ctx_delete(response);
        }
    }

reset:
    service_reset(sm); /* Truncate NC objects. */
    return rc;
}


int service_plant_dispatch(ServiceManager* sm,
    const NCodecDiagSrpcInvokeCall* call, NCodecDiagSrpcInvokeReturn** ret)
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
        return -ENOENT;
    }

    /* Build the call arguments func(uint8, void*, uint8*). */
    uint8_t      status = (uint8_t)call->op_status;
    PlantMessage msg = {
        .req_data = call->ctx.request,
        .req_data_len = call->ctx.request_len,
        .address_info.suppressPosResponse = false,
    };
    if (strcmp(call->function, "read") == 0) {
        msg.res_data = malloc(PLANT_MAX_DATA_SIZE);
        if (msg.res_data == NULL) return -ENOMEM;
        msg.res_max_data_len = PLANT_MAX_DATA_SIZE;
    }
    PlantMessage* msg_ptr = &msg;
    uint8_t       error_code = 0;
    uint8_t*      error_code_ptr = &error_code;

    /* Call the function. */
    void*    args[] = { &status, &msg_ptr, &error_code_ptr };
    ffi_sarg _result = 0;
    ffi_call(&func->cif, func->fn_ptr, &_result, args);

    /* Generate the response. */
    if ((!call->ctx.response_required && error_code == 0) || ret == NULL) {
        free(msg.res_data);
        return 0;
    }

    NCodecDiagSrpcInvokeReturn* _ret = calloc(1, sizeof(*_ret));
    if (_ret == NULL) {
        free(msg.res_data);
        return -ENOMEM;
    }
    _ret->return_value = (uint32_t)_result;
    _ret->error_code = error_code;
    _ret->ctx = call->ctx;
    _ret->ctx.request = NULL;
    _ret->ctx.request_len = 0;
    _ret->ctx.response = msg.res_data;
    _ret->ctx.response_len = msg.res_data_len;

    /* Return the response. */
    *ret = _ret;
    return 0;
}


int run_example(NCODEC* nc)
{
    /* Create a service manager (shared between client and server). */
    ServiceManager sm = service_manager_create(ECU_NAME, ECU_ID, SWC_ID);

    /* Setup the server part of the service manager. */
    int rc = service_register(&sm, nc, SERVICE_NAME, _registry,
        sizeof(_registry) / sizeof(_registry[0]));
    service_set_dispatch(&sm, SERVICE_NAME, service_plant_dispatch);

    /* Send some client requests (via/over the NCodec). */
    client_request(&sm, "clear", NULL);
    client_request(&sm, "append", "Hello");
    client_request(&sm, "append", " ");
    client_request(&sm, "append", "World!");
    client_request(&sm, "read", NULL);

    /* Destroy the service manager. */
    service_destroy(&sm);
    return rc;
}
