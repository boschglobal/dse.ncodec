// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdint.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/diag/codec.h>
#include <dse/ncodec/interface/diag/srpc.h>
#include <dse/ncodec/schema/abs/diagnostic/srpc_builder.h>


#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Diagnostic_ServiceRpc, x)


extern uint8_t* diag_stream_read(
    DiagCodecInstance* nc, const char* identifier, size_t* length);
extern int32_t diag_stream_write(DiagCodecInstance* nc);


static uint32_t _emit_message_context(
    flatcc_builder_t* B, const NCodecDiagSrpcMessageContext* ctx)
{
    ns(MessageContext_start(B));
    if (ctx->request) {
        ns(MessageContext_request_add(B,
            flatbuffers_uint8_vec_create(B, ctx->request, ctx->request_len)));
    }
    if (ctx->response) {
        ns(MessageContext_response_add(B,
            flatbuffers_uint8_vec_create(B, ctx->response, ctx->response_len)));
    }
    ns(MessageContext_request_type_add(B, ctx->request_type));
    ns(MessageContext_request_source_add(B, ctx->request_source));
    ns(MessageContext_response_include_request_add(
        B, ctx->response_include_request));
    ns(MessageContext_response_required_add(B, ctx->response_required));
    ns(MessageContext_context_id_add(B, ctx->context_id));
    ns(MessageContext_rx_pdu_id_add(B, ctx->rx_pdu_id));
    return ns(MessageContext_end(B));
}

static uint32_t _emit_invoke_call(
    flatcc_builder_t* B, const NCodecDiagSrpcInvokeCall* call)
{
    if (call->service == NULL || call->function == NULL) return 0;

    uint32_t ctx = _emit_message_context(B, &call->ctx);
    ns(InvokeCall_start(B));
    ns(InvokeCall_service_add(
        B, flatbuffers_string_create_str(B, call->service)));
    ns(InvokeCall_function_add(
        B, flatbuffers_string_create_str(B, call->function)));
    ns(InvokeCall_op_status_add(B, call->op_status));
    ns(InvokeCall_ctx_add(B, ctx));
    return ns(InvokeCall_end(B));
}

static uint32_t _emit_invoke_return(
    flatcc_builder_t* B, const NCodecDiagSrpcInvokeReturn* ret)
{
    uint32_t ctx = _emit_message_context(B, &ret->ctx);
    ns(InvokeReturn_start(B));
    ns(InvokeReturn_return_value_add(B, ret->return_value));
    ns(InvokeReturn_error_code_add(B, ret->error_code));
    ns(InvokeReturn_ctx_add(B, ctx));
    return ns(InvokeReturn_end(B));
}

static uint32_t _emit_message(flatcc_builder_t* B,
    const NCodecDiagSrpcMessage* m, const char* target_ecu_name,
    uint32_t target_ecu_id, uint32_t target_swc_id, uint32_t origin_ecu_id,
    uint32_t origin_swc_id, uint32_t node_uid)
{
    ns(ServiceRpcMessage_start_as_root_with_size(B));
    ns(ServiceRpcMessage_call_id_add(B, m->call_id));
    switch (m->payload_type) {
    case NCodecDiagSrpcPayloadTypeInvokeCall: {
        uint32_t ref = _emit_invoke_call(B, &m->payload.invoke_call);
        if (ref == 0) return 0;
        ns(ServiceRpcMessage_payload_InvokeCall_add(B, ref));
    } break;
    case NCodecDiagSrpcPayloadTypeInvokeReturn: {
        uint32_t ref = _emit_invoke_return(B, &m->payload.invoke_return);
        ns(ServiceRpcMessage_payload_InvokeReturn_add(B, ref));
    } break;
    default:
        break;
    }
    if (target_ecu_name) {
        ns(ServiceRpcMessage_target_ecu_name_add(
            B, flatbuffers_string_create_str(B, target_ecu_name)));
    }
    ns(ServiceRpcMessage_target_ecu_id_add(B, target_ecu_id));
    ns(ServiceRpcMessage_target_swc_id_add(B, target_swc_id));
    ns(ServiceRpcMessage_origin_ecu_id_add(B, origin_ecu_id));
    ns(ServiceRpcMessage_origin_swc_id_add(B, origin_swc_id));
    ns(ServiceRpcMessage_frame_id_add(B, m->frame_id));
    ns(ServiceRpcMessage_node_uid_add(B, node_uid));
    ns(ServiceRpcMessage_simulation_time_add(B, m->simulation_time));
    ns(ServiceRpcMessage_utc_time_add(B, m->utc_time));
    return ns(ServiceRpcMessage_end_as_root(B));
}


static void _decode_message_context(
    ns(MessageContext_table_t) t, NCodecDiagSrpcMessageContext* ctx)
{
    ctx->request = ns(MessageContext_request(t));
    ctx->request_len = flatbuffers_uint8_vec_len(ctx->request);
    ctx->response = ns(MessageContext_response(t));
    ctx->response_len = flatbuffers_uint8_vec_len(ctx->response);
    ctx->request_type = ns(MessageContext_request_type(t));
    ctx->request_source = ns(MessageContext_request_source(t));
    ctx->response_include_request =
        ns(MessageContext_response_include_request(t));
    ctx->response_required = ns(MessageContext_response_required(t));
    ctx->context_id = ns(MessageContext_context_id(t));
    ctx->rx_pdu_id = ns(MessageContext_rx_pdu_id(t));
}

static void _decode_invoke_call(
    ns(InvokeCall_table_t) t, NCodecDiagSrpcInvokeCall* call)
{
    call->service = ns(InvokeCall_service(t));
    call->function = ns(InvokeCall_function(t));
    call->op_status = ns(InvokeCall_op_status(t));
    _decode_message_context(ns(InvokeCall_ctx(t)), &call->ctx);
}

static void _decode_invoke_return(
    ns(InvokeReturn_table_t) t, NCodecDiagSrpcInvokeReturn* ret)
{
    ret->return_value = ns(InvokeReturn_return_value(t));
    ret->error_code = ns(InvokeReturn_error_code(t));
    _decode_message_context(ns(InvokeReturn_ctx(t)), &ret->ctx);
}

static void _decode_message(
    ns(ServiceRpcMessage_table_t) t, NCodecDiagSrpcMessage* m)
{
    m->call_id = ns(ServiceRpcMessage_call_id(t));
    switch (ns(ServiceRpcMessage_payload_type(t))) {
    case ns(Payload_InvokeCall):
        m->payload_type = NCodecDiagSrpcPayloadTypeInvokeCall;
        _decode_invoke_call(
            (ns(InvokeCall_table_t))ns(ServiceRpcMessage_payload(t)),
            &m->payload.invoke_call);
        break;
    case ns(Payload_InvokeReturn):
        m->payload_type = NCodecDiagSrpcPayloadTypeInvokeReturn;
        _decode_invoke_return(
            (ns(InvokeReturn_table_t))ns(ServiceRpcMessage_payload(t)),
            &m->payload.invoke_return);
        break;
    default:
        break;
    }
    m->target.ecu_name = ns(ServiceRpcMessage_target_ecu_name(t));
    m->target.ecu_id = ns(ServiceRpcMessage_target_ecu_id(t));
    m->target.swc_id = ns(ServiceRpcMessage_target_swc_id(t));
    m->origin.ecu_id = ns(ServiceRpcMessage_origin_ecu_id(t));
    m->origin.swc_id = ns(ServiceRpcMessage_origin_swc_id(t));
    m->frame_id = ns(ServiceRpcMessage_frame_id(t));
    m->node_uid = ns(ServiceRpcMessage_node_uid(t));
    m->simulation_time = ns(ServiceRpcMessage_simulation_time(t));
    m->utc_time = ns(ServiceRpcMessage_utc_time(t));
}


int32_t srpc_write(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*     _nc = (DiagCodecInstance*)nc;
    NCodecDiagSrpcMessage* _msg = (NCodecDiagSrpcMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    /* Set the Target fields for this message. */
    const char* target_ecu_name = _msg->target.ecu_name;
    uint32_t    target_ecu_id = _msg->target.ecu_id;
    uint32_t    target_swc_id = _msg->target.swc_id;

    /* Set the Origin fields for this message. */
    uint32_t origin_ecu_id = _msg->origin.ecu_id;
    uint32_t origin_swc_id = _msg->origin.swc_id;
    if (_msg->payload_type != NCodecDiagSrpcPayloadTypeInvokeReturn) {
        if (origin_ecu_id == 0) origin_ecu_id = _nc->ecu_id;
        if (origin_swc_id == 0) origin_swc_id = _nc->swc_id;
    }

    /* Set the node_uid for the sender of _this_ message. */
    uint32_t node_uid = _msg->node_uid ? _msg->node_uid : _nc->node_id;

    flatcc_builder_t* B = &_nc->fbs_builder;
    flatcc_builder_reset(B);
    if (_emit_message(B, _msg, target_ecu_name, target_ecu_id, target_swc_id,
            origin_ecu_id, origin_swc_id, node_uid) == 0) {
        flatcc_builder_reset(B);
        log_error(_nc, "Service RPC message is missing a required field");
        return -EINVAL;
    }
    return diag_stream_write(_nc);
}


int32_t srpc_read(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*     _nc = (DiagCodecInstance*)nc;
    NCodecDiagSrpcMessage* _msg = (NCodecDiagSrpcMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    /* Reset the message, in case caller ignores the return value. */
    *_msg = (NCodecDiagSrpcMessage){};

    size_t   length = 0;
    uint8_t* buffer = NULL;
    while ((buffer = diag_stream_read(_nc, flatbuffers_identifier, &length))) {
        ns(ServiceRpcMessage_table_t) t = ns(ServiceRpcMessage_as_root(buffer));

        /* Filter: sender==receiver. */
        if ((_nc->node_id) &&
            (_nc->node_id == ns(ServiceRpcMessage_node_uid(t)))) {
            if (_nc->loopback == false) continue;
        }

        _decode_message(t, _msg);
        return length;
    }
    return -ENOMSG;
}
