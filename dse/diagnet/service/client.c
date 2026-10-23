// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/codec.h>
#include <dse/diagnet/service/service.h>


static NCodecDiagSrpcMessageContext* _ctx_copy(
    const NCodecDiagSrpcMessageContext* src)
{
    /* Deep copy a context into a single allocation (struct followed by the
     * request and response bytes), so that service_ctx_delete() is a single
     * object.
     */
    size_t request_len = src->request ? src->request_len : 0;
    size_t response_len = src->response ? src->response_len : 0;

    NCodecDiagSrpcMessageContext* ctx =
        calloc(1, sizeof(*ctx) + request_len + response_len);
    if (ctx == NULL) return NULL;

    *ctx = *src;
    uint8_t* data = (uint8_t*)(ctx + 1);
    ctx->request = NULL;
    ctx->request_len = 0;
    ctx->response = NULL;
    ctx->response_len = 0;
    if (request_len) {
        memcpy(data, src->request, request_len);
        ctx->request = data;
        ctx->request_len = request_len;
        data += request_len;
    }
    if (response_len) {
        memcpy(data, src->response, response_len);
        ctx->response = data;
        ctx->response_len = response_len;
    }
    return ctx;
}

static NCodecDiagSrpcMessageContext** _request_find(
    ServiceManager* sm, uint32_t context_id, size_t* index)
{
    for (size_t i = 0; i < vector_len(&sm->requests); i++) {
        NCodecDiagSrpcMessageContext** item = vector_at(&sm->requests, i, NULL);
        if (item && *item && (*item)->context_id == context_id) {
            if (index) *index = i;
            return item;
        }
    }
    return NULL;
}

static ServiceGroup* _group_find(ServiceManager* sm, const char* name)
{
    ServiceGroup key = { .name = name };
    return vector_find(&sm->groups, &key, 0, NULL);
}


/**
service_request
===============

Write and flush a service request through a registered service group's Network
Codec. Requests requiring a response are tracked by context identifier. The
request context and its buffers remain caller owned; a deep copy is retained
for response tracking.

Parameters
----------
sm (ServiceManager*)
: Service Manager object.

ecu_name (const char*)
: Target ECU name. Must not be NULL.

ecu_id (uint32_t)
: Target ECU identifier.

swc_id (uint32_t)
: Target software component identifier.

service (const char*)
: Name of the registered service group and remote service.

function (const char*)
: Name of the remote function to invoke.

ctx (NCodecDiagSrpcMessageContext*)
: Request context containing encoded arguments, the context identifier, and
    response requirements.

Returns
-------
0
: The request was written and flushed, and response tracking was established
    when required.

-EINVAL
: A required pointer argument is NULL.

-ENOENT
: The service group is not registered or has no Network Codec.

-EEXIST
: A request with the same context identifier is already awaiting a response.

-ENOMEM
: The tracking context could not be copied or added to the request registry.
    Failure to add the context can occur after the request has been sent.

<int>
: A negative error returned by `ncodec_write` or `ncodec_flush`.
*/
int service_request(ServiceManager* sm, const char* ecu_name, uint32_t ecu_id,
    uint32_t swc_id, const char* service, const char* function,
    NCodecDiagSrpcMessageContext* ctx)
{
    if (sm == NULL || service == NULL || function == NULL || ctx == NULL) {
        return -EINVAL;
    }
    ServiceGroup* group = _group_find(sm, service);
    if (group == NULL || group->nc == NULL) return -ENOENT;
    if (ctx->response_required && _request_find(sm, ctx->context_id, NULL)) {
        return -EEXIST;
    }

    NCodecDiagSrpcMessageContext* tracked = NULL;
    if (ctx->response_required) {
        tracked = _ctx_copy(ctx);
        if (tracked == NULL) return -ENOMEM;
    }

    NCodecDiagSrpcMessage msg = {
        .target = {
            .ecu_name = ecu_name,
            .ecu_id = ecu_id,
            .swc_id = swc_id,
        },
        .origin = {
            .ecu_id = sm->ecu_id,
            .swc_id = sm->swc_id,
        },
        .payload_type = NCodecDiagSrpcPayloadTypeInvokeCall,
        .payload.invoke_call = {
            .service = service,
            .function = function,
            .ctx = *ctx,
        },
    };
    int32_t rc = ncodec_write(group->nc, (NCodecMessage*)&msg);
    if (rc >= 0) rc = ncodec_flush(group->nc);
    if (rc < 0) {
        service_ctx_delete(tracked);
        return rc;
    }

    if (tracked && vector_push(&sm->requests, &tracked) != 0) {
        service_ctx_delete(tracked);
        return -ENOMEM;
    }
    return 0;
}


/**
service_response
================

Read a response matching a pending request and the Service Manager's ECU and
software component identifiers. The matching request is removed from response
tracking. Messages that are not matching responses are consumed and ignored.

Parameters
----------
sm (ServiceManager*)
: Service Manager object.

ctx (NCodecDiagSrpcMessageContext**)
: (out) Heap allocated copy of the response context and its request/response
    buffers. The caller owns this object and must release it with
    `service_ctx_delete`. Set to NULL when no response is returned, provided
    both arguments are valid.

return_value (uint32_t*)
: Simple cast of the return value, if any.

error_code (int32_t*)
: Error code related to the request.

Returns
-------
0
: A matching response context was returned.

-EINVAL
: The Service Manager or output pointer is NULL.

-ENOMEM
: The response context could not be copied. The pending request remains
    registered, but the response message has been consumed.

-ENOMSG
: No matching response was available from the registered Network Codecs.
*/
int service_response(ServiceManager* sm, NCodecDiagSrpcMessageContext** ctx,
    uint32_t* return_value, int32_t* error_code)
{
    if (sm == NULL || ctx == NULL) return -EINVAL;

    *ctx = NULL;

    // TODO: will this be re-entrant, or do we need to drain the nc objects
    // to a vector?

    for (size_t i = 0; i < vector_len(&sm->groups); i++) {
        ServiceGroup* group = vector_at(&sm->groups, i, NULL);
        if (group == NULL || group->nc == NULL) continue;

        while (1) {
            NCodecDiagSrpcMessage msg = { 0 };
            if (ncodec_read(group->nc, (NCodecMessage*)&msg) < 0) break;
            if (msg.payload_type != NCodecDiagSrpcPayloadTypeInvokeReturn) {
                continue;
            }

            /* Filter the response. */
            if (msg.origin.ecu_id != sm->ecu_id) {
                continue;
            }
            if (msg.origin.swc_id != sm->swc_id) {
                continue;
            }

            /* Process the response. */
            NCodecDiagSrpcInvokeReturn*    ret = &msg.payload.invoke_return;
            size_t                         index = 0;
            NCodecDiagSrpcMessageContext** request =
                _request_find(sm, ret->ctx.context_id, &index);
            if (request == NULL) {
                if (ret->error_code == 0) {
                    continue;
                } else {
                    /* Error, return response. */
                }
            } else {
                /* Response requested. */
                service_ctx_delete(*request);
                vector_delete_at(&sm->requests, index);
            }
            NCodecDiagSrpcMessageContext* response = _ctx_copy(&ret->ctx);
            if (response == NULL) return -ENOMEM;
            *ctx = response;
            if (return_value) *return_value = ret->return_value;
            if (error_code) *error_code = ret->error_code;
            return 0;
        }
    }
    return -ENOMSG;
}


/**
service_reset
=============

Truncate the Network Codec stream for every registered service group. Pending
client request contexts remain registered. Codec truncation errors are ignored.

Parameters
----------
sm (ServiceManager*)
: Service Manager object. If NULL, no streams are truncated.
*/
void service_reset(ServiceManager* sm)
{
    if (sm == NULL) return;

    for (size_t i = 0; i < vector_len(&sm->groups); i++) {
        ServiceGroup* group = vector_at(&sm->groups, i, NULL);
        if (group && group->nc) ncodec_truncate(group->nc);
    }
}


/**
service_ctx_cancel
==================

Remove a pending client request from response tracking and release its copied
context and buffers. No cancellation message is sent to the remote service.

Parameters
----------
sm (ServiceManager*)
: Service Manager object. If NULL, no request is cancelled.

context_id (uint32_t)
: Identifier of the pending request. If no request matches, no action is taken.
*/
void service_ctx_cancel(ServiceManager* sm, uint32_t context_id)
{
    if (sm == NULL) return;

    size_t                         index = 0;
    NCodecDiagSrpcMessageContext** request =
        _request_find(sm, context_id, &index);
    if (request == NULL) return;
    service_ctx_delete(*request);
    vector_delete_at(&sm->requests, index);
}


/**
service_ctx_delete
==================

Release a client response context returned by `service_response`. The context
and its copied buffers occupy a single allocation and are released together.
Do not pass a caller-owned request context or free its copied buffers
separately.

Parameters
----------
ctx (NCodecDiagSrpcMessageContext*)
: Client response context to release. If NULL, no memory is released.
*/
void service_ctx_delete(NCodecDiagSrpcMessageContext* ctx)
{
    free(ctx);
}
