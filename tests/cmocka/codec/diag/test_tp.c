// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/diag/tp.h>
#include <tests/cmocka/codec/diag/test_diag.h>


static int test_setup(void** state)
{
    return diag_test_setup(state, MIMETYPE("tp"));
}

static int test_teardown(void** state)
{
    return diag_test_teardown(state);
}


static void assert_frame_equal(
    const NCodecDiagIsoTpFrame* a, const NCodecDiagIsoTpFrame* b)
{
    assert_int_equal(a->frame_type, b->frame_type);
    assert_int_equal(a->timestamp_us, b->timestamp_us);
    switch (a->frame_type) {
    case NCodecDiagIsoTpFramePayloadTypeSingleFrame:
        assert_int_equal(a->frame.single_frame.data_length,
            b->frame.single_frame.data_length);
        assert_vector_or_null(a->frame.single_frame.payload,
            a->frame.single_frame.payload_len, b->frame.single_frame.payload,
            b->frame.single_frame.payload_len);
        break;
    case NCodecDiagIsoTpFramePayloadTypeFirstFrame:
        assert_int_equal(a->frame.first_frame.total_length,
            b->frame.first_frame.total_length);
        assert_vector_or_null(a->frame.first_frame.payload,
            a->frame.first_frame.payload_len, b->frame.first_frame.payload,
            b->frame.first_frame.payload_len);
        break;
    case NCodecDiagIsoTpFramePayloadTypeConsecutiveFrame:
        assert_int_equal(a->frame.consecutive_frame.sequence_number,
            b->frame.consecutive_frame.sequence_number);
        assert_vector_or_null(a->frame.consecutive_frame.payload,
            a->frame.consecutive_frame.payload_len,
            b->frame.consecutive_frame.payload,
            b->frame.consecutive_frame.payload_len);
        break;
    case NCodecDiagIsoTpFramePayloadTypeFlowControl:
        assert_int_equal(
            a->frame.flow_control.status, b->frame.flow_control.status);
        assert_int_equal(
            a->frame.flow_control.block_size, b->frame.flow_control.block_size);
        assert_int_equal(
            a->frame.flow_control.st_min_ms, b->frame.flow_control.st_min_ms);
        break;
    default:
        break;
    }
}


void test_tp_iso_tp(void** state)
{
    Mock* mock = *state;

    static const uint8_t payload[] = { 0x22, 0xf1, 0x90, 1, 2, 3, 4, 5 };
    static const uint8_t empty[1];
    static const NCodecDiagIsoTpFrame frames[] = {
        { .frame_type = NCodecDiagIsoTpFramePayloadTypeSingleFrame,
            .frame.single_frame = { .data_length = 3,
                .payload = payload,
                .payload_len = 3 },
            .timestamp_us = 10 },
        { .frame_type = NCodecDiagIsoTpFramePayloadTypeFirstFrame,
            .frame.first_frame = { .total_length = 8,
                .payload = payload,
                .payload_len = 6 },
            .timestamp_us = 20 },
        { .frame_type = NCodecDiagIsoTpFramePayloadTypeFlowControl,
            .frame.flow_control = { .status = NCodecDiagIsoTpFlowStatusWait,
                .block_size = 4,
                .st_min_ms = 0xf5 },
            .timestamp_us = 30 },
        { .frame_type = NCodecDiagIsoTpFramePayloadTypeConsecutiveFrame,
            .frame.consecutive_frame = { .sequence_number = 1,
                .payload = &payload[6],
                .payload_len = 2 },
            .timestamp_us = 40 },
        { .frame_type = NCodecDiagIsoTpFramePayloadTypeSingleFrame,
            .frame.single_frame = { .payload = empty },
            .timestamp_us = 50 },
        { .frame_type = NCodecDiagIsoTpFramePayloadTypeNone,
            .timestamp_us = 60 },
    };
    NCodecDiagTpMessage msg = {
        .transport_meta_type = NCodecDiagTpTransportMetadataTypeIsoTp,
        .transport_meta.iso_tp = { .can_id = 0x7e0, .total_length = 8 },
        .payload = payload,
        .payload_len = ARRAY_SIZE(payload),
        .frames = frames,
        .frames_len = ARRAY_SIZE(frames),
        .start_timestamp_us = 10,
        .end_timestamp_us = 40,
    };
    int32_t len = ncodec_write(mock->nc, &msg);
    assert_true(len > 0);

    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    NCodecDiagTpMessage out = {};
    assert_int_equal(ncodec_read(mock->nc, &out), len);
    assert_int_equal(
        out.transport_meta_type, NCodecDiagTpTransportMetadataTypeIsoTp);
    assert_int_equal(out.transport_meta.iso_tp.can_id, 0x7e0);
    assert_int_equal(out.transport_meta.iso_tp.total_length, 8);
    assert_vector_or_null(
        payload, ARRAY_SIZE(payload), out.payload, out.payload_len);
    assert_int_equal(out.frames_len, ARRAY_SIZE(frames));
    for (size_t i = 0; i < ARRAY_SIZE(frames); i++) {
        assert_frame_equal(&frames[i], &out.frames[i]);
    }
    assert_int_equal(out.start_timestamp_us, 10);
    assert_int_equal(out.end_timestamp_us, 40);
    assert_int_equal(ncodec_read(mock->nc, &out), -ENOMSG);
}


void test_tp_do_ip(void** state)
{
    Mock* mock = *state;

    static const uint8_t              payload[] = { 0x3e, 0x00 };
    static const NCodecDiagIsoTpFrame frames[1];
    NCodecDiagTpMessage               msg[] = {
        { .transport_meta_type = NCodecDiagTpTransportMetadataTypeDoIp,
                          .transport_meta.do_ip = { .protocol_version = 2,
                              .payload_type = NCodecDiagDoIpPayloadTypeDiagnosticMessage,
                              .source_address = 0x0e00,
                              .target_address = 0x1001 },
                          .payload = payload,
                          .payload_len = ARRAY_SIZE(payload) },
        /* Present, empty frames vector. */
        { .frames = frames },
        /* No metadata, payload or frames. */
        { .start_timestamp_us = 1 },
    };
    for (size_t i = 0; i < ARRAY_SIZE(msg); i++) {
        assert_true(ncodec_write(mock->nc, &msg[i]) > 0);
    }

    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    NCodecDiagTpMessage out = {};
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(
        out.transport_meta_type, NCodecDiagTpTransportMetadataTypeDoIp);
    assert_int_equal(out.transport_meta.do_ip.protocol_version, 2);
    assert_int_equal(out.transport_meta.do_ip.payload_type,
        NCodecDiagDoIpPayloadTypeDiagnosticMessage);
    assert_int_equal(out.transport_meta.do_ip.source_address, 0x0e00);
    assert_int_equal(out.transport_meta.do_ip.target_address, 0x1001);
    assert_vector_or_null(
        payload, ARRAY_SIZE(payload), out.payload, out.payload_len);
    assert_null(out.frames);

    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(
        out.transport_meta_type, NCodecDiagTpTransportMetadataTypeNone);
    assert_null(out.payload);
    assert_non_null(out.frames);
    assert_int_equal(out.frames_len, 0);

    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_null(out.payload);
    assert_null(out.frames);
    assert_int_equal(out.start_timestamp_us, 1);
    assert_int_equal(ncodec_read(mock->nc, &out), -ENOMSG);
}


int run_tp_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_tp_iso_tp, s, t),
        cmocka_unit_test_setup_teardown(test_tp_do_ip, s, t),
    };

    return cmocka_run_group_tests_name("TP", tests, NULL, NULL);
}
