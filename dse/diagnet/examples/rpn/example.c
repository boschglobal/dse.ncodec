// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <ffi.h>
#include <dse/ncodec/codec.h>
#include <dse/diagnet/service/service.h>
#include <dse/diagnet/examples/rpn/rpn.h>


#define SERVICE_NAME "RPN"

static ffi_type* _int_arg[] = { &ffi_type_sint };

static ServiceFunction _registry[] = {
    {
        .name = "push",
        .fn_ptr = (void (*)(void))rpn_push,
        .return_type = &ffi_type_void,
        .arg_count = 1,
        .arg_types = _int_arg,
    },
    {
        .name = "pop",
        .fn_ptr = (void (*)(void))rpn_pop,
        .return_type = &ffi_type_sint,
    },
    {
        .name = "add",
        .fn_ptr = (void (*)(void))rpn_add,
        .return_type = &ffi_type_void,
    },
    {
        .name = "minus",
        .fn_ptr = (void (*)(void))rpn_minus,
        .return_type = &ffi_type_void,
    },
    {
        .name = "multiply",
        .fn_ptr = (void (*)(void))rpn_multiply,
        .return_type = &ffi_type_void,
    },
};


static int client_request(ServiceManager* sm, const char* func, int val)
{
    static uint8_t context_id = 0;
    int            rc = 0;

    /* Find the NCodec of the "RPN" service group. */
    NCODEC* nc = NULL;
    for (size_t i = 0; i < vector_len(&sm->funcs); i++) {
        ServiceGroup* group = vector_at(&sm->funcs, i, NULL);
        if (group && strcmp(group->name, SERVICE_NAME) == 0) nc = group->nc;
    }
    if (nc == NULL) return -ENOENT;

    /* Build the request. */
    NCodecDiagSrpcMessageContext request = {
        .context_id = ++context_id,
        .response_required = (strcmp(func, "pop") == 0),
    };
    CallArgumentsBuilder builder;
    rc = call_arguments_builder_init(&builder);
    if (rc != 0) return rc;
    if (strcmp(func, "push") == 0) {
        rc = call_arguments_add_i32(&builder, val);
        if (rc != 0) goto reset;
    }
    rc = call_arguments_finalize(&builder, &request);
    if (rc != 0) goto reset;

    /* Push the request. */
    rc = service_request(sm, SERVICE_NAME, func, &request);
    if (rc != 0) goto reset;

    /* Cycle the transport/server (single stream, rewind to read). */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    service_process_requests(sm);

    /* Get any responses. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    if (request.response_required) {
        NCodecDiagSrpcMessageContext* response = NULL;
        while (service_response(sm, &response) >= 0) {
            if (response->context_id == request.context_id) {
                ffi_sarg value = 0;
                if (response->response_len == sizeof(value)) {
                    memcpy(&value, response->response, sizeof(value));
                }
                printf("%s: returned %d (context_id %u)\n", func, (int)value,
                    response->context_id);
            }
            service_ctx_delete(response);
        }
    }

reset:
    call_arguments_builder_destroy(&builder);
    service_reset(sm); /* Truncate NC objects. */
    return rc;
}


int run_example(NCODEC* nc)
{
    /* Create a service manager (shared between client and server). */
    ServiceManager sm = service_manager_create();

    /* Setup the server part of the service manager. */
    service_register(&sm, nc, SERVICE_NAME, _registry,
        sizeof(_registry) / sizeof(_registry[0]));

    /* Send some client requests (via/over the NCodec). */
    client_request(&sm, "push", 7);
    client_request(&sm, "push", 8);
    client_request(&sm, "multiply", 0);
    client_request(&sm, "push", 15);
    client_request(&sm, "minus", 0);
    client_request(&sm, "push", 1);
    client_request(&sm, "add", 0);
    client_request(&sm, "pop", 0);

    /* Destroy the service manager. */
    service_destroy(&sm);
    return 0;
}
