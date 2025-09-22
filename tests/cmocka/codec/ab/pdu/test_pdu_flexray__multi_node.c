// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/logger.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec/ab/vector.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>
#include <flexray_harness.h>


/* Enable Debug: Add this to the start of a test function.
Values include: LOG_QUIET LOG_INFO LOG_DEBUG LOG_TRACE

```
void some_test(void** state)
{
    __log_level__ = LOG_DEBUG;
    ...
}
```
*/
static NCodecPduFlexrayConfig config = {
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

static TestNode testnode_A_2vcn = (TestNode){
    .mimetype = "application/x-automotive-bus; "
                "interface=stream;type=pdu;schema=fbs;"
                "ecu_id=1;vcn=2;model=flexray",
};
static TestNode testnode_B = (TestNode){
    .mimetype = "application/x-automotive-bus; "
                "interface=stream;type=pdu;schema=fbs;"
                "ecu_id=2;model=flexray",
};
static TestNode testnode_C = (TestNode){
    .mimetype = "application/x-automotive-bus; "
                "interface=stream;type=pdu;schema=fbs;"
                "ecu_id=3;model=flexray",
};

static TestFrameMap frame_table = { .map ={
    /* Node A */
    {
        {
            .slot_id = 5,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionTx,
            .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
            .index = {
                .frame_table = 0,
            },
        },
        {
            .slot_id = 10,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionTx,
            .index = {
                .frame_table = 1,
            },
        },
        {
            .slot_id = 15,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionTx,
            .index = {
                .frame_table = 2,
            },
        },
        {
            .slot_id = 60,
            .payload_length = 64,
            .direction = NCodecPduFlexrayDirectionTx,
            .index = {
                .frame_table = 3,
            },
        },
    },
    /* Node B */
    {
        {
            .slot_id = 5,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionRx,
            .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
            .index = {
                .frame_table = 0,
            },
        },
        {
            .slot_id = 10,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionRx,
            .index = {
                .frame_table = 1,
            },
        },
        {
            .slot_id = 15,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionRx,
            .index = {
                .frame_table = 2,
            },
        },
        {
            .slot_id = 60,
            .payload_length = 64,
            .direction = NCodecPduFlexrayDirectionRx,
            .index = {
                .frame_table = 3,
            },
        },
    },
    /* Node C */
    {
        {
            .slot_id = 5,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionRx,
            .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
            .index = {
                .frame_table = 0,
            },
        },
        {
            .slot_id = 10,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionRx,
            .index = {
                .frame_table = 1,
            },
        },
        {
            .slot_id = 15,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1,
            .direction = NCodecPduFlexrayDirectionRx,
            .index = {
                .frame_table = 2,
            },
        },
        {
            .slot_id = 60,
            .payload_length = 64,
            .direction = NCodecPduFlexrayDirectionRx,
            .index = {
                .frame_table = 3,
            },
        },
    },
    },
};

#define PAYLOAD_5  "hello 5 world"
#define PAYLOAD_10 "hello 10 world"
#define PAYLOAD_15 "hello 15 world"
#define PAYLOAD_60 "hello 60 world"
static TestPduMap pdu_map = { .map = {
    /* Node A */
    {
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
            .payload = PAYLOAD_5,
            .payload_len = strlen(PAYLOAD_5),
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
            .payload = PAYLOAD_10,
            .payload_len = strlen(PAYLOAD_10),
        },
        {
            .slot_id = 15,
            .frame_config_index = 2,
            .lpdu_status = NCodecPduFlexrayLpduStatusNone,
            .payload = PAYLOAD_15,
            .payload_len = strlen(PAYLOAD_15),
        },
        {
            .slot_id = 60,
            .frame_config_index = 3,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
            .payload = PAYLOAD_60,
            .payload_len = strlen(PAYLOAD_60),
        },
    },
    /* Node B */
    {
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
        {
            .slot_id = 15,
            .frame_config_index = 2,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
        {
            .slot_id = 60,
            .frame_config_index = 3,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
    },
    /* Node C */
    {
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
        {
            .slot_id = 15,
            .frame_config_index = 2,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
        {
            .slot_id = 60,
            .frame_config_index = 3,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        },
    },
},
};


static TestPduList pdu_multi_check = {
    .count = 18,
    .list = {
        /* Cycle 0 */
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
            .cycle = 0,
            .macrotick = 220,
            .node_ident = { .node.ecu_id = 1 },
        },
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_5,
            .payload_len = strlen(PAYLOAD_5),
            .cycle = 0,
            .macrotick = 220,
            .node_ident = { .node.ecu_id = 2 },
        },
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_5,
            .payload_len = strlen(PAYLOAD_5),
            .cycle = 0,
            .macrotick = 220,
            .node_ident = { .node.ecu_id = 3 },
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
            .cycle = 0,
            .macrotick = 495,
            .node_ident = { .node.ecu_id = 1 },
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_10,
            .payload_len = strlen(PAYLOAD_10),
            .cycle = 0,
            .macrotick = 495,
            .node_ident = { .node.ecu_id = 2 },
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_10,
            .payload_len = strlen(PAYLOAD_10),
            .cycle = 0,
            .macrotick = 495,
            .node_ident = { .node.ecu_id = 3 },
        },
        {
            .slot_id = 15,
            .frame_config_index = 2,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
            .cycle = 0,
            .macrotick = 770,
            .null_frame = true,
            .node_ident = { .node.ecu_id = 2 },
        },
        {
            .slot_id = 15,
            .frame_config_index = 2,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
            .cycle = 0,
            .macrotick = 770,
            .null_frame = true,
            .node_ident = { .node.ecu_id = 3 },
        },
        {
            .slot_id = 60,
            .frame_config_index = 3,
            .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
            .cycle = 0,
            .macrotick = 2216,
            .node_ident = { .node.ecu_id = 1 },
        },
        {
            .slot_id = 60,
            .frame_config_index = 3,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_60,
            .payload_len = strlen(PAYLOAD_60),
            .cycle = 0,
            .macrotick = 2216,
            .node_ident = { .node.ecu_id = 2 },
        },
        {
            .slot_id = 60,
            .frame_config_index = 3,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_60,
            .payload_len = strlen(PAYLOAD_60),
            .cycle = 0,
            .macrotick = 2216,
            .node_ident = { .node.ecu_id = 3 },
        },
        /* Cycle 1 */
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
            .cycle = 1,
            .macrotick = 220,
            .node_ident = { .node.ecu_id = 1 },
        },
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_5,
            .payload_len = strlen(PAYLOAD_5),
            .cycle = 1,
            .macrotick = 220,
            .node_ident = { .node.ecu_id = 2 },
        },
        {
            .slot_id = 5,
            .frame_config_index = 0,
            .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
            .payload = PAYLOAD_5,
            .payload_len = strlen(PAYLOAD_5),
            .cycle = 1,
            .macrotick = 220,
            .node_ident = { .node.ecu_id = 3 },
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
            .cycle = 1,
            .macrotick = 495,
            .null_frame = true,
            .node_ident = { .node.ecu_id = 2 },
        },
        {
            .slot_id = 10,
            .frame_config_index = 1,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
            .cycle = 1,
            .macrotick = 495,
            .null_frame = true,
            .node_ident = { .node.ecu_id = 3 },
        },
        {
            .slot_id = 15,
            .frame_config_index = 2,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
            .cycle = 1,
            .macrotick = 770,
            .null_frame = true,
            .node_ident = { .node.ecu_id = 2 },
        },
        {
            .slot_id = 15,
            .frame_config_index = 2,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
            .cycle = 1,
            .macrotick = 770,
            .null_frame = true,
            .node_ident = { .node.ecu_id = 3 },
        },
    },
};


void multi_node__mixed__2vcn(void** state)
{
    testnode_A_2vcn.config = config;
    testnode_B.config = config;
    testnode_C.config = config;

    testnode_A_2vcn.config.node_ident.node_id = 1;
    testnode_B.config.node_ident.node_id = 2;
    testnode_C.config.node_ident.node_id = 3;

    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                testnode_A_2vcn,
                testnode_B,
                testnode_C,
            },
            .frame_table = frame_table,
        },
        .run = {
            .push_active = true,
            .pdu_map = pdu_map,
            .cycles = 1,
        },
        .expect = {
            .cycle = 1+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = pdu_multi_check,
        },
    };
    flexray_harness_run_test(&mock->test);
}


/*
Node A B C

A sends
    static null always
    static once
    static repeat
    dynamic (once)
B receives
C receives


B ignores one frames
C ignores all frames
*/


int run_pdu_flexray_multi_node_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;
#define T cmocka_unit_test_setup_teardown

    const struct CMUnitTest tests[] = {
        T(multi_node__mixed__2vcn, s, t),
        // T(multi_node__2vcn, s, t),
        // T(multi_node__active, s, t),
        // T(multi_node__bridge, s, t),
        // T(multi_node__WUP, s, t),
    };

    return cmocka_run_group_tests_name(
        "PDU  FLEXRAY::MULTI_NODE", tests, NULL, NULL);
}
