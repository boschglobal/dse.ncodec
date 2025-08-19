// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/logger.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>

#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define BUFFER_LEN    1024


typedef struct Mock {
    NCODEC* nc;
} Mock;


static int test_setup(void** state)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);
    *state = mock;
    return 0;
}


static int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock && mock->nc) ncodec_close((void*)mock->nc);
    if (mock) free(mock);
    return 0;
}


NCodecPduFlexrayConfig cc_config = {
    .bit_rate = NCodecPduFlexrayBitrate10,
    .channel_enable = NCodecPduFlexrayChannelA,
    .macrotick_per_cycle = 3361u,
    .microtick_per_cycle = 200000u,
    .network_idle_start = (3361u - 5u - 1u),
    .static_slot_length = 55u,
    .static_slot_count = 38u,
    .minislot_length = 6u,
    .minislot_count = 211u,
    .static_slot_payload_length = (32u * 2), /* Word to Byte */

    .coldstart_node = false,
    .sync_node = false,
    .coldstart_attempts = 8u,
    .wakeup_channel_select = 0, /* Channel A */
    .single_slot_enabled = false,
    .key_slot_id = 0u,
};


void test_pdu_flexray__2vcn_normalactive(void** state)
{
    Mock* mock = *state;
    int   rc;

#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=1;vcn=2;model=flexray"

    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(MIMETYPE, stream);
    assert_non_null(mock->nc);
    NCODEC* nc = mock->nc;

    NCodecPduFlexrayConfig config = cc_config;

    ncodec_truncate(nc);
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeConfig,
                    .metadata.config = config,
                } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandConfig,
                    } } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandReady,
                    } } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandRun,
                    } } });
    assert_int_equal(rc, 0);
    ncodec_flush(nc);

    /* Reset the stream pointer for reading. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);

    /* Read a PDU (triggers Bus Model). */
    NCodecPdu pdu = {};
    rc = ncodec_read(nc, &pdu);
    assert_int_equal(rc, 0);
    assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
    assert_int_equal(NCodecPduFlexrayMetadataTypeStatus,
        pdu.transport.flexray.metadata_type);
    assert_int_equal(5, pdu.transport.flexray.metadata.status.cycle);
    assert_int_equal(0xbde, pdu.transport.flexray.metadata.status.macrotick);
    assert_int_equal(NCodecPduFlexrayPocStateNormalActive,
        pdu.transport.flexray.metadata.status.channel[0].poc_state);
    assert_int_equal(NCodecPduFlexrayTransceiverStateFrameSync,
        pdu.transport.flexray.metadata.status.channel[0].tcvr_state);
}


void test_pdu_flexray__static_frame_txrx(void** state)
{
    Mock* mock = *state;
    int   rc;

#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=1;vcn=2;model=flexray"

    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(MIMETYPE, stream);
    assert_non_null(mock->nc);
    NCODEC* nc = mock->nc;

    NCodecPduFlexrayConfig     config = cc_config;
    NCodecPduFlexrayLpduConfig frame_table[] = {
        {
            .slot_id = 7,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionTx,
        },
        {
            .slot_id = 7,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionRx,
        },
    };
    config.frame_config.table = frame_table;
    config.frame_config.count = ARRAY_SIZE(frame_table);

    ncodec_truncate(nc);
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeConfig,
                    .metadata.config = config,
                } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandConfig,
                    } } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandReady,
                    } } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandRun,
                    } } });
    assert_int_equal(rc, 0);
    ncodec_flush(nc);

    /* Reset the stream pointer for reading. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);

    /* Read a PDU (triggers Bus Model). */
    NCodecPdu pdu = {};
    rc = ncodec_read(nc, &pdu);
    assert_int_equal(rc, 0);
    assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
    assert_int_equal(NCodecPduFlexrayMetadataTypeStatus,
        pdu.transport.flexray.metadata_type);
    assert_int_equal(5, pdu.transport.flexray.metadata.status.cycle);
    assert_int_equal(0xbde, pdu.transport.flexray.metadata.status.macrotick);
    assert_int_equal(NCodecPduFlexrayPocStateNormalActive,
        pdu.transport.flexray.metadata.status.channel[0].poc_state);
    assert_int_equal(NCodecPduFlexrayTransceiverStateFrameSync,
        pdu.transport.flexray.metadata.status.channel[0].tcvr_state);

/* Configure LPDU's for Tx->Rx. */
#define PAYLOAD "hello world"
    ncodec_truncate(nc);
    rc = ncodec_write(
        nc, &(NCodecPdu){ .id = 2, /* Slot ID */
                .payload = PAYLOAD,
                .payload_len = strlen(PAYLOAD),
                .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                    .metadata.lpdu = {
                        .frame_config_index = 0, /* Index into Config Table. */
                        .status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    } } });
    assert_int_equal(rc, strlen(PAYLOAD));
    rc = ncodec_write(
        nc, &(NCodecPdu){ .id = 2, /* Slot ID */
                .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                    .metadata.lpdu = {
                        .frame_config_index = 1, /* Index into Config Table. */
                        .status = NCodecPduFlexrayLpduStatusNotReceived,
                    } } });
    assert_int_equal(rc, 0);
    ncodec_flush(nc);

    /* Reset the stream pointer for reading. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);

    /* Read a PDU - status. */
    pdu = (NCodecPdu){ 0 };
    rc = ncodec_read(nc, &pdu);
    assert_int_equal(rc, 0);
    assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
    assert_int_equal(NCodecPduFlexrayMetadataTypeStatus,
        pdu.transport.flexray.metadata_type);

    /* Read a PDU - config index 0, Tx status. */
    pdu = (NCodecPdu){ 0 };
    rc = ncodec_read(nc, &pdu);
    assert_int_equal(rc, 0);
    assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
    assert_int_equal(
        NCodecPduFlexrayMetadataTypeLpdu, pdu.transport.flexray.metadata_type);
    assert_int_equal(NCodecPduFlexrayLpduStatusTransmitted,
        pdu.transport.flexray.metadata.lpdu.status);

    /* Read a PDU - config index 1, Rx status and payload. */
    pdu = (NCodecPdu){ 0 };
    rc = ncodec_read(nc, &pdu);
    assert_int_equal(rc, 11);
    assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
    assert_int_equal(
        NCodecPduFlexrayMetadataTypeLpdu, pdu.transport.flexray.metadata_type);
    assert_int_equal(NCodecPduFlexrayLpduStatusReceived,
        pdu.transport.flexray.metadata.lpdu.status);
    assert_memory_equal(PAYLOAD, pdu.payload, strlen(PAYLOAD));
    assert_memory_equal(PAYLOAD, pdu.payload_len, strlen(PAYLOAD));
}


int run_pdu_flexray_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;
#define T cmocka_unit_test_setup_teardown

    const struct CMUnitTest tests[] = {
        T(test_pdu_flexray__2vcn_normalactive, s, t),
        T(test_pdu_flexray__static_frame_txrx, s, t),
    };

    return cmocka_run_group_tests_name("PDU  FLEXRAY", tests, NULL, NULL);
}
