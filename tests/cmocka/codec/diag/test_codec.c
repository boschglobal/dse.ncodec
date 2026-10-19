// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/diag/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/diag/uds.h>
#include <dse/ncodec/interface/diag/tp.h>
#include <dse/ncodec/interface/diag/srpc.h>
#include <dse/ncodec/interface/pdu.h>
#include <tests/cmocka/codec/diag/test_diag.h>


extern int32_t uds_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t tp_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t srpc_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t xil_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t pdu_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t can_write(NCODEC* nc, NCodecMessage* msg);


NCODEC* ncodec_open(const char* mime_type, NSTREAM* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc) {
        NCodecInstance* _nc = (NCodecInstance*)nc;
        _nc->stream = stream;
    }
    return nc;
}

int diag_test_setup(void** state, const char* mime_type)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);

    NSTREAM* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(mime_type, stream);
    assert_non_null(mock->nc);

    *state = mock;
    return 0;
}

int diag_test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock && mock->nc) ncodec_close((void*)mock->nc);
    if (mock) free(mock);

    return 0;
}

const uint8_t* stream_buffer(NCODEC* nc, size_t* length)
{
    NCodecStreamVTable* stream =
        (NCodecStreamVTable*)((NCodecInstance*)nc)->stream;
    uint8_t* buffer = NULL;
    *length = 0;
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    stream->read(nc, &buffer, length, NCODEC_POS_NC);
    return buffer;
}

void stream_append(NCODEC* dst, NCODEC* src)
{
    size_t              length = 0;
    const uint8_t*      buffer = stream_buffer(src, &length);
    NCodecStreamVTable* stream =
        (NCodecStreamVTable*)((NCodecInstance*)dst)->stream;
    ncodec_seek(dst, 0, NCODEC_SEEK_END);
    stream->write(dst, (uint8_t*)buffer, length);
}


static int test_setup(void** state)
{
    return diag_test_setup(state, MIMETYPE("srpc"));
}

static int test_teardown(void** state)
{
    return diag_test_teardown(state);
}


void test_codec_create(void** state)
{
    UNUSED(state);

    struct {
        const char* mime_type;
        void*       write;
    } valid[] = {
        { MIMETYPE("uds"), uds_write },
        { MIMETYPE("tp"), tp_write },
        { MIMETYPE("srpc"), srpc_write },
        { MIMETYPE("xil"), xil_write },
        { "application/x-automotive-bus; interface=diagnostic; type=srpc; "
          "schema=fbs; node_id=1; swc_id=2; ecu_id=3; name=ecu; loopback=1",
            srpc_write },
    };
    for (size_t i = 0; i < ARRAY_SIZE(valid); i++) {
        NCodecInstance* nc = ncodec_create(valid[i].mime_type);
        assert_non_null(nc);
        assert_ptr_equal(nc->mime_type, valid[i].mime_type);
        assert_ptr_equal(nc->codec.write, valid[i].write);
        /* No simulation time interface. */
        assert_int_equal(
            ncodec_utime(nc, (NCodecUtimeOperation){ 0 }), -ENOSYS);
        ncodec_close(nc);
    }

    const char* invalid[] = {
        NULL,
        "",
        "application/x-automotive-bus",
        "application/x-other; interface=diagnostic; type=uds; schema=fbs",
        "application/x-automotive-bus; interface=diagnostic; schema=fbs",
        "application/x-automotive-bus; interface=diagnostic; type=uds",
        "application/x-automotive-bus; type=uds; schema=fbs",
        MIMETYPE("pdu"),
        "application/x-automotive-bus; interface=diagnostic; type=uds; "
        "schema=json",
    };
    for (size_t i = 0; i < ARRAY_SIZE(invalid); i++) {
        assert_null(ncodec_create(invalid[i]));
    }

    /* AB Codec selection is unchanged. */
    NCodecInstance* nc = ncodec_create(
        "application/x-automotive-bus; interface=stream; type=pdu; schema=fbs");
    assert_non_null(nc);
    assert_ptr_not_equal(nc->codec.write, uds_write);
    assert_ptr_not_equal(nc->codec.write, srpc_write);
    ncodec_close(nc);
}


void test_codec_create_route(void** state)
{
    UNUSED(state);

    /* ncodec_create() routes on the interface parameter (last value). */
    struct {
        const char* mime_type;
        void*       write;
    } tc[] = {
        { "application/x-automotive-bus; interface=stream; type=pdu; "
          "schema=fbs",
            pdu_write },
        { "application/x-automotive-bus; interface=stream; type=frame; "
          "bus=can; schema=fbs",
            can_write },
        { "application/x-automotive-bus; interface=diagnostic; type=uds; "
          "schema=fbs",
            uds_write },
        { "application/x-automotive-bus;type=xil;schema=fbs;"
          "interface=diagnostic",
            xil_write },
        { "application/x-automotive-bus; interface=stream; type=uds; "
          "schema=fbs; interface=diagnostic",
            uds_write },
        { "application/x-automotive-bus; interface=diagnostic; type=pdu; "
          "schema=fbs; interface=stream",
            pdu_write },
        { "application/x-automotive-bus; flag; interface=diagnostic; "
          "type=tp; schema=fbs",
            tp_write },
        /* Not routed. */
        { "application/x-automotive-bus; type=uds; schema=fbs", NULL },
        { "application/x-automotive-bus; interface; type=uds; schema=fbs",
            NULL },
        { "application/x-automotive-bus; interface=; type=uds; schema=fbs",
            NULL },
        { "application/x-automotive-bus; interface=other; type=uds; "
          "schema=fbs",
            NULL },
        { "application/x-automotive-bus; interface=Stream; type=pdu; "
          "schema=fbs",
            NULL },
        /* Routed, rejected by the codec. */
        { "application/x-automotive-bus; interface=diagnostic; type=pdu; "
          "schema=fbs",
            NULL },
        { "application/x-automotive-bus; interface=stream; type=uds; "
          "schema=fbs",
            NULL },
        { "application/x-other; interface=stream; type=pdu; schema=fbs", NULL },
        { "interface=stream; type=pdu; schema=fbs", NULL },
        { " ; ", NULL },
    };
    for (size_t i = 0; i < ARRAY_SIZE(tc); i++) {
        NCodecInstance* nc = ncodec_create(tc[i].mime_type);
        if (tc[i].write == NULL) {
            assert_null(nc);
            continue;
        }
        assert_non_null(nc);
        assert_ptr_equal(nc->codec.write, tc[i].write);
        ncodec_close(nc);
    }
}


typedef struct config_tc {
    const char* name;
    const char* value;
    size_t      offset_value;
    size_t      offset_int_value;
    uint8_t     int_value;
} config_tc;

void test_codec_config(void** state)
{
    Mock*              mock = *state;
    DiagCodecInstance* nc = mock->nc;

    config_tc tc[] = {
        { .name = "interface",
            .value = "diagnostic",
            .offset_value = offsetof(DiagCodecInstance, interface) },
        { .name = "type",
            .value = "uds",
            .offset_value = offsetof(DiagCodecInstance, type) },
        { .name = "schema",
            .value = "fbs",
            .offset_value = offsetof(DiagCodecInstance, schema) },
        { .name = "node_id",
            .value = "2",
            .int_value = 2,
            .offset_value = offsetof(DiagCodecInstance, node_id_str),
            .offset_int_value = offsetof(DiagCodecInstance, node_id) },
        { .name = "swc_id",
            .value = "4",
            .int_value = 4,
            .offset_value = offsetof(DiagCodecInstance, swc_id_str),
            .offset_int_value = offsetof(DiagCodecInstance, swc_id) },
        { .name = "ecu_id",
            .value = "5",
            .int_value = 5,
            .offset_value = offsetof(DiagCodecInstance, ecu_id_str),
            .offset_int_value = offsetof(DiagCodecInstance, ecu_id) },
        { .name = "name",
            .value = "ecu",
            .offset_value = offsetof(DiagCodecInstance, name) },
        { .name = "loopback",
            .value = "1",
            .offset_value = offsetof(DiagCodecInstance, loopback_str) },
    };
    for (size_t i = 0; i < ARRAY_SIZE(tc); i++) {
        ncodec_config(nc, (struct NCodecConfigItem){
                              .name = tc[i].name, .value = tc[i].value });
        char* value = *(char**)((char*)nc + tc[i].offset_value);
        assert_string_equal(value, tc[i].value);
        if (tc[i].offset_int_value) {
            uint8_t int_value = *((uint8_t*)nc + tc[i].offset_int_value);
            assert_int_equal(int_value, tc[i].int_value);
        }
    }
    assert_true(nc->loopback);

    /* Selector changes are reported, the codec implementation is not. */
    assert_ptr_equal(nc->c.codec.write, srpc_write);

    /* Unknown parameters, including node_uid, are rejected. */
    assert_int_equal(
        nc->c.codec.config(mock->nc,
            (struct NCodecConfigItem){ .name = "node_uid", .value = "42" }),
        -EINVAL);
    assert_int_equal(
        nc->c.codec.config(mock->nc,
            (struct NCodecConfigItem){ .name = "bus_id", .value = "1" }),
        -EINVAL);
}


void test_codec_stat(void** state)
{
    UNUSED(state);

    const char* mime_type =
        "application/x-automotive-bus; interface=diagnostic; type=srpc; "
        "schema=fbs; node_id=1; swc_id=2; ecu_id=3; name=ecu; loopback=1; "
        "node_uid=42";
    NCODEC* nc = ncodec_create(mime_type);
    assert_non_null(nc);

    const char* names[] = { "interface", "type", "schema", "node_id", "swc_id",
        "ecu_id", "name", "loopback" };
    const char* values[] = { "diagnostic", "srpc", "fbs", "1", "2", "3", "ecu",
        "1" };
    int32_t     index = 0;
    for (; index >= 0; index++) {
        NCodecConfigItem item = ncodec_stat(nc, &index);
        if (index < 0) {
            assert_null(item.name);
            break;
        }
        assert_true((size_t)index < ARRAY_SIZE(names));
        assert_string_equal(item.name, names[index]);
        assert_string_equal(item.value, values[index]);
    }
    assert_int_equal(index, -1);
    assert_null(ncodec_stat(nc, NULL).name);
    ncodec_close(nc);
}


void test_codec_no_stream(void** state)
{
    UNUSED(state);

    const char* types[] = { MIMETYPE("uds"), MIMETYPE("tp"), MIMETYPE("srpc"),
        MIMETYPE("xil") };
    for (size_t i = 0; i < ARRAY_SIZE(types); i++) {
        NCodecInstance* nc = ncodec_create(types[i]);
        assert_non_null(nc);
        uint8_t msg[512] = {};
        assert_int_equal(ncodec_write(nc, msg), -ENOSR);
        assert_int_equal(ncodec_read(nc, msg), -ENOSR);
        assert_int_equal(ncodec_flush(nc), -ENOSR);
        assert_int_equal(ncodec_truncate(nc), -ENOSR);
        assert_int_equal(nc->codec.write(NULL, msg), -ENOSTR);
        assert_int_equal(nc->codec.read(NULL, msg), -ENOSTR);
        assert_int_equal(nc->codec.flush(NULL), -ENOSTR);
        assert_int_equal(nc->codec.truncate(NULL), -ENOSTR);
        ncodec_close(nc);
    }
}


void test_codec_no_message(void** state)
{
    Mock* mock = *state;

    assert_int_equal(ncodec_write(mock->nc, NULL), -EINVAL);
    assert_int_equal(ncodec_read(mock->nc, NULL), -EINVAL);
}


void test_codec_flush(void** state)
{
    Mock* mock = *state;

    /* Nothing buffered by the codec. */
    assert_int_equal(ncodec_flush(mock->nc), 0);
    assert_int_equal(ncodec_tell(mock->nc), 0);

    NCodecDiagSrpcMessage msg = { .call_id = 1 };
    int32_t               len = ncodec_write(mock->nc, &msg);
    assert_true(len > 0);
    /* Written immediately, flush does not modify the stream. */
    assert_int_equal(ncodec_tell(mock->nc), len);
    assert_int_equal(ncodec_flush(mock->nc), 0);
    assert_int_equal(ncodec_tell(mock->nc), len);
}


void test_codec_write_records(void** state)
{
    Mock* mock = *state;

    /* Each write appends one size-prefixed record at the stream position. */
    int32_t total = 0;
    for (uint32_t i = 1; i <= 3; i++) {
        NCodecDiagSrpcMessage msg = { .call_id = i };
        int32_t               len = ncodec_write(mock->nc, &msg);
        assert_true(len > 8);
        total += len;
        assert_int_equal(ncodec_tell(mock->nc), total);
    }

    size_t         length = 0;
    const uint8_t* buffer = stream_buffer(mock->nc, &length);
    assert_int_equal(length, total);
    size_t pos = 0;
    for (int i = 0; i < 3; i++) {
        uint32_t size = buffer[pos] | buffer[pos + 1] << 8 |
                        buffer[pos + 2] << 16 | (uint32_t)buffer[pos + 3] << 24;
        assert_memory_equal(&buffer[pos + 8], "DSRP", 4);
        pos += size + 4;
    }
    assert_int_equal(pos, length);

    /* Read in order, each read returns the record length. */
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    int32_t consumed = 0;
    for (uint32_t i = 1; i <= 3; i++) {
        NCodecDiagSrpcMessage msg = {};
        int32_t               len = ncodec_read(mock->nc, &msg);
        assert_true(len > 8);
        consumed += len;
        assert_int_equal(msg.call_id, i);
        assert_int_equal(ncodec_tell(mock->nc), consumed);
    }
    NCodecDiagSrpcMessage msg = { .call_id = 42 };
    assert_int_equal(ncodec_read(mock->nc, &msg), -ENOMSG);
    assert_int_equal(msg.call_id, 0);
    assert_int_equal(ncodec_tell(mock->nc), total);
    assert_int_equal(ncodec_read(mock->nc, &msg), -ENOMSG);
}


void test_codec_truncate(void** state)
{
    Mock* mock = *state;

    NCodecDiagSrpcMessage msg = { .call_id = 1 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    assert_true(ncodec_read(mock->nc, &msg) > 0);

    assert_int_equal(ncodec_truncate(mock->nc), 0);
    assert_int_equal(ncodec_tell(mock->nc), 0);
    size_t length = 0;
    stream_buffer(mock->nc, &length);
    assert_int_equal(length, 0);
    assert_int_equal(ncodec_read(mock->nc, &msg), -ENOMSG);

    /* Stream is reusable after truncate. */
    msg = (NCodecDiagSrpcMessage){ .call_id = 2 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    msg = (NCodecDiagSrpcMessage){};
    assert_true(ncodec_read(mock->nc, &msg) > 0);
    assert_int_equal(msg.call_id, 2);
}


void test_codec_mixed_stream(void** state)
{
    UNUSED(state);

    /* Source stream: UDS, AB PDU, TP, SRPC, UDS. */
    NCODEC* src_uds =
        ncodec_open(MIMETYPE("uds"), ncodec_buffer_stream_create(BUFFER_LEN));
    NCODEC* src_tp =
        ncodec_open(MIMETYPE("tp"), ncodec_buffer_stream_create(BUFFER_LEN));
    NCODEC* src_srpc =
        ncodec_open(MIMETYPE("srpc"), ncodec_buffer_stream_create(BUFFER_LEN));
    NCODEC* src_pdu = ncodec_open(
        "application/x-automotive-bus; interface=stream; type=pdu; schema=fbs",
        ncodec_buffer_stream_create(BUFFER_LEN));
    NCODEC* mixed =
        ncodec_open(MIMETYPE("uds"), ncodec_buffer_stream_create(BUFFER_LEN));

    NCodecDiagUdsMessage uds = { .service_id = 1 };
    ncodec_write(src_uds, &uds);
    stream_append(mixed, src_uds);
    ncodec_truncate(src_uds);

    uint8_t   payload[] = { 1, 2, 3 };
    NCodecPdu pdu = { .id = 7, .payload = payload, .payload_len = 3 };
    ncodec_write(src_pdu, &pdu);
    ncodec_flush(src_pdu);
    stream_append(mixed, src_pdu);

    NCodecDiagTpMessage tp = { .start_timestamp_us = 99 };
    ncodec_write(src_tp, &tp);
    stream_append(mixed, src_tp);

    NCodecDiagSrpcMessage srpc = { .call_id = 5 };
    ncodec_write(src_srpc, &srpc);
    stream_append(mixed, src_srpc);

    uds = (NCodecDiagUdsMessage){ .service_id = 2 };
    ncodec_write(src_uds, &uds);
    stream_append(mixed, src_uds);

    /* Each codec reads only its own records, in order. */
    size_t         length = 0;
    const uint8_t* buffer = stream_buffer(mixed, &length);
    NCODEC*        readers[] = { src_uds, src_tp, src_srpc };
    for (size_t i = 0; i < ARRAY_SIZE(readers); i++) {
        ncodec_truncate(readers[i]);
        NCodecStreamVTable* s =
            (NCodecStreamVTable*)((NCodecInstance*)readers[i])->stream;
        s->write(readers[i], (uint8_t*)buffer, length);
        ncodec_seek(readers[i], 0, NCODEC_SEEK_SET);
    }

    uds = (NCodecDiagUdsMessage){};
    assert_true(ncodec_read(src_uds, &uds) > 0);
    assert_int_equal(uds.service_id, 1);
    assert_true(ncodec_read(src_uds, &uds) > 0);
    assert_int_equal(uds.service_id, 2);
    assert_int_equal(ncodec_read(src_uds, &uds), -ENOMSG);

    tp = (NCodecDiagTpMessage){};
    assert_true(ncodec_read(src_tp, &tp) > 0);
    assert_int_equal(tp.start_timestamp_us, 99);
    assert_int_equal(ncodec_read(src_tp, &tp), -ENOMSG);

    srpc = (NCodecDiagSrpcMessage){};
    assert_true(ncodec_read(src_srpc, &srpc) > 0);
    assert_int_equal(srpc.call_id, 5);
    assert_int_equal(ncodec_read(src_srpc, &srpc), -ENOMSG);

    /* The AB codec skips the diagnostic records. */
    ncodec_truncate(src_pdu);
    NCodecStreamVTable* s =
        (NCodecStreamVTable*)((NCodecInstance*)src_pdu)->stream;
    s->write(src_pdu, (uint8_t*)buffer, length);
    ncodec_seek(src_pdu, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){};
    assert_int_equal(ncodec_read(src_pdu, &pdu), 3);
    assert_int_equal(pdu.id, 7);
    assert_int_equal(ncodec_read(src_pdu, &pdu), -ENOMSG);

    ncodec_close(src_uds);
    ncodec_close(src_tp);
    ncodec_close(src_srpc);
    ncodec_close(src_pdu);
    ncodec_close(mixed);
}


void test_codec_incomplete_record(void** state)
{
    Mock* mock = *state;

    NCodecDiagSrpcMessage msg = { .call_id = 1 };
    int32_t               len = ncodec_write(mock->nc, &msg);
    assert_true(len > 0);
    msg.call_id = 2;
    assert_int_equal(ncodec_write(mock->nc, &msg), len);

    /* Truncate the final record, it is not returned. */
    size_t         length = 0;
    const uint8_t* buffer = stream_buffer(mock->nc, &length);
    uint8_t*       copy = malloc(length);
    memcpy(copy, buffer, length);
    ncodec_truncate(mock->nc);
    NCodecStreamVTable* s =
        (NCodecStreamVTable*)((NCodecInstance*)mock->nc)->stream;
    s->write(mock->nc, copy, length - 1);
    free(copy);

    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    msg = (NCodecDiagSrpcMessage){};
    assert_int_equal(ncodec_read(mock->nc, &msg), len);
    assert_int_equal(msg.call_id, 1);
    assert_int_equal(ncodec_read(mock->nc, &msg), -ENOMSG);
    assert_int_equal(ncodec_tell(mock->nc), length - 1);
}


static unsigned log_count;

static void trace_log(NCODEC* nc, NCodecTraceLogLevel level, const char* msg)
{
    assert_non_null(nc);
    assert_int_equal(level, NCODEC_LOG_ERROR);
    assert_non_null(strstr(msg, "missing a required field (srpc_write:"));
    /* Formatting the message does not modify errno. */
    assert_int_equal(errno, EDOM);
    log_count++;
}

void test_codec_trace_log(void** state)
{
    Mock*              mock = *state;
    DiagCodecInstance* nc = mock->nc;
    nc->c.trace.log = trace_log;

    /* InvokeCall: service and function are required. */
    NCodecDiagSrpcMessage msg = {
        .payload_type = NCodecDiagSrpcPayloadTypeInvokeCall,
        .payload.invoke_call = { .function = "f" },
    };
    log_count = 0;
    errno = EDOM;
    assert_int_equal(ncodec_write(mock->nc, &msg), -EINVAL);
    assert_int_equal(log_count, 1);
    assert_int_equal(errno, EDOM);
    assert_int_equal(ncodec_tell(mock->nc), 0);

    nc->log_level = NCODEC_LOG_FATAL;
    assert_int_equal(ncodec_write(mock->nc, &msg), -EINVAL);
    assert_int_equal(log_count, 1);
}


int run_codec_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_codec_create),
        cmocka_unit_test(test_codec_create_route),
        cmocka_unit_test_setup_teardown(test_codec_config, s, t),
        cmocka_unit_test(test_codec_stat),
        cmocka_unit_test(test_codec_no_stream),
        cmocka_unit_test_setup_teardown(test_codec_no_message, s, t),
        cmocka_unit_test_setup_teardown(test_codec_flush, s, t),
        cmocka_unit_test_setup_teardown(test_codec_write_records, s, t),
        cmocka_unit_test_setup_teardown(test_codec_truncate, s, t),
        cmocka_unit_test(test_codec_mixed_stream),
        cmocka_unit_test_setup_teardown(test_codec_incomplete_record, s, t),
        cmocka_unit_test_setup_teardown(test_codec_trace_log, s, t),
    };

    return cmocka_run_group_tests_name("CODEC", tests, NULL, NULL);
}
