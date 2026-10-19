// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/diag/srpc.h>
#include <tests/cmocka/codec/diag/test_diag.h>


#define MIMETYPE_NODE(id)                                                      \
    MIMETYPE("srpc") ";node_id=" id ";swc_id=2;ecu_id=3;name=ecu_" id


static int test_setup(void** state)
{
    return diag_test_setup(state, MIMETYPE_NODE("1"));
}

static int test_teardown(void** state)
{
    return diag_test_teardown(state);
}


static void assert_context_equal(const NCodecDiagSrpcMessageContext* a,
    const NCodecDiagSrpcMessageContext*                              b)
{
    assert_vector_or_null(
        a->request, a->request_len, b->request, b->request_len);
    assert_vector_or_null(
        a->response, a->response_len, b->response, b->response_len);
    assert_int_equal(a->request_type, b->request_type);
    assert_int_equal(a->request_source, b->request_source);
    assert_int_equal(a->response_include_request, b->response_include_request);
    assert_int_equal(a->response_required, b->response_required);
    assert_int_equal(a->context_id, b->context_id);
    assert_int_equal(a->rx_pdu_id, b->rx_pdu_id);
}


void test_srpc_payload(void** state)
{
    Mock* mock = *state;

    static const uint8_t  request[] = { 0x22, 0xf1, 0x90 };
    static const uint8_t  response[] = { 0x62, 0xf1, 0x90, 0x41 };
    static const uint8_t  empty[1];
    NCodecDiagSrpcMessage msg[] = {
        { .call_id = 1,
            .payload_type = NCodecDiagSrpcPayloadTypeInvokeCall,
            .payload.invoke_call = { .service = "Dcm",
                .function = "Dispatch",
                .op_status = 3,
                .ctx = { .request = request,
                    .request_len = ARRAY_SIZE(request),
                    .request_type = 1,
                    .request_source = 2,
                    .response_include_request = true,
                    .response_required = false,
                    .context_id = 4,
                    .rx_pdu_id = 0x1234 } },
            .frame_id = 0x7e0,
            .simulation_time = 1.5,
            .utc_time = 1700000000 },
        { .call_id = 1,
            .payload_type = NCodecDiagSrpcPayloadTypeInvokeReturn,
            .payload.invoke_return = { .return_value = 5,
                .error_code = 6,
                .ctx = { .request = empty,
                    .response = response,
                    .response_len = ARRAY_SIZE(response),
                    .response_required = true } },
            .simulation_time = 2.5 },
        { .call_id = 2 },
    };
    for (size_t i = 0; i < ARRAY_SIZE(msg); i++) {
        assert_true(ncodec_write(mock->nc, &msg[i]) > 0);
    }

    /* Read with a different node. */
    NCODEC* nc = ncodec_open(
        MIMETYPE_NODE("9"), ncodec_buffer_stream_create(BUFFER_LEN));
    stream_append(nc, mock->nc);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);

    NCodecDiagSrpcMessage out = {};
    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.call_id, 1);
    assert_int_equal(out.payload_type, NCodecDiagSrpcPayloadTypeInvokeCall);
    assert_string_equal(out.payload.invoke_call.service, "Dcm");
    assert_string_equal(out.payload.invoke_call.function, "Dispatch");
    assert_int_equal(out.payload.invoke_call.op_status, 3);
    assert_context_equal(
        &msg[0].payload.invoke_call.ctx, &out.payload.invoke_call.ctx);
    assert_int_equal(out.frame_id, 0x7e0);
    assert_true(out.simulation_time == 1.5);
    assert_int_equal(out.utc_time, 1700000000);

    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.payload_type, NCodecDiagSrpcPayloadTypeInvokeReturn);
    assert_int_equal(out.payload.invoke_return.return_value, 5);
    assert_int_equal(out.payload.invoke_return.error_code, 6);
    assert_context_equal(
        &msg[1].payload.invoke_return.ctx, &out.payload.invoke_return.ctx);
    assert_true(out.simulation_time == 2.5);

    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.call_id, 2);
    assert_int_equal(out.payload_type, NCodecDiagSrpcPayloadTypeNone);
    assert_int_equal(ncodec_read(nc, &out), -ENOMSG);
    ncodec_close(nc);
}


void test_srpc_metadata(void** state)
{
    Mock* mock = *state;

    /* Codec parameters are used when message fields are not set. */
    NCodecDiagSrpcMessage msg = { .call_id = 1 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);
    /* Message fields take precedence. */
    msg = (NCodecDiagSrpcMessage){ .call_id = 2,
        .ecu_name = "target",
        .ecu_id = 300,
        .swc_id = 301,
        .node_uid = 302 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);

    NCODEC* nc = ncodec_open(
        MIMETYPE_NODE("9"), ncodec_buffer_stream_create(BUFFER_LEN));
    stream_append(nc, mock->nc);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);

    NCodecDiagSrpcMessage out = {};
    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.call_id, 1);
    assert_string_equal(out.ecu_name, "ecu_1");
    assert_int_equal(out.ecu_id, 3);
    assert_int_equal(out.swc_id, 2);
    assert_int_equal(out.node_uid, 1);

    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.call_id, 2);
    assert_string_equal(out.ecu_name, "target");
    assert_int_equal(out.ecu_id, 300);
    assert_int_equal(out.swc_id, 301);
    assert_int_equal(out.node_uid, 302);
    ncodec_close(nc);

    /* Without codec parameters, fields remain unset. */
    nc = ncodec_open(MIMETYPE("srpc"), ncodec_buffer_stream_create(BUFFER_LEN));
    msg = (NCodecDiagSrpcMessage){ .call_id = 3 };
    assert_true(ncodec_write(nc, &msg) > 0);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.call_id, 3);
    assert_null(out.ecu_name);
    assert_int_equal(out.ecu_id, 0);
    assert_int_equal(out.swc_id, 0);
    assert_int_equal(out.node_uid, 0);
    ncodec_close(nc);
}


void test_srpc_filter(void** state)
{
    Mock* mock = *state;

    NCodecDiagSrpcMessage msg = { .call_id = 1 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);
    msg = (NCodecDiagSrpcMessage){ .call_id = 2, .node_uid = 7 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);

    /* Filter: sender==receiver. */
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    NCodecDiagSrpcMessage out = {};
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(out.call_id, 2);
    assert_int_equal(ncodec_read(mock->nc, &out), -ENOMSG);

    /* Loopback disables the filter. */
    ncodec_config(mock->nc,
        (struct NCodecConfigItem){ .name = "loopback", .value = "1" });
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(out.call_id, 1);
    assert_int_equal(out.node_uid, 1);
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(out.call_id, 2);
    assert_int_equal(ncodec_read(mock->nc, &out), -ENOMSG);
}


void test_srpc_required(void** state)
{
    Mock* mock = *state;

    NCodecDiagSrpcMessage msg[] = {
        { .payload_type = NCodecDiagSrpcPayloadTypeInvokeCall,
            .payload.invoke_call = { .function = "f" } },
        { .payload_type = NCodecDiagSrpcPayloadTypeInvokeCall,
            .payload.invoke_call = { .service = "s" } },
    };
    for (size_t i = 0; i < ARRAY_SIZE(msg); i++) {
        assert_int_equal(ncodec_write(mock->nc, &msg[i]), -EINVAL);
        assert_int_equal(ncodec_tell(mock->nc), 0);
    }

    /* Codec remains usable. */
    NCodecDiagSrpcMessage ok = { .call_id = 1, .node_uid = 5 };
    assert_true(ncodec_write(mock->nc, &ok) > 0);
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    NCodecDiagSrpcMessage out = {};
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(out.call_id, 1);
}


int run_srpc_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_srpc_payload, s, t),
        cmocka_unit_test_setup_teardown(test_srpc_metadata, s, t),
        cmocka_unit_test_setup_teardown(test_srpc_filter, s, t),
        cmocka_unit_test_setup_teardown(test_srpc_required, s, t),
    };

    return cmocka_run_group_tests_name("SRPC", tests, NULL, NULL);
}
