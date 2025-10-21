// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec/ab/vector.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>
#include <flexray_harness.h>

extern uint8_t __log_level__;

/* Enable Debug: Add this to the start of a test function.
Values include: NCODEC_LOG_QUIET NCODEC_LOG_INFO NCODEC_LOG_DEBUG
NCODEC_LOG_TRACE

```
void some_test(void** state)
{
    __log_level__ = NCODEC_LOG_DEBUG;
    ...
}
```
*/

static TestNode testnode_A = (TestNode){
    .mimetype = "application/x-automotive-bus; "                               \
        "interface=stream;type=pdu;schema=fbs;"                                \
        "ecu_id=1;vcn=2;model=flexray",
    .config = {
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
        .inhibit_null_frames = false,
    },
};

static TestNode testnode_inhibit = (TestNode){
    .mimetype = "application/x-automotive-bus; "                               \
        "interface=stream;type=pdu;schema=fbs;"                                \
        "ecu_id=1;vcn=2;model=flexray",
    .config = {
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
        .inhibit_null_frames = true,
    },
};

#define PAYLOAD_1 "hello world"

void null_frames(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                    /* Sent on Cycle 0, then Tx/Rx NULL on Cycle 1,2,3 ... */
                    {
                        .slot_id = 7,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                    {
                        .slot_id = 7,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                    /* Sent on Cycle 0,1,2,3 .... */
                    {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
                        .index = {
                            .frame_table = 2,  /* Self index. */
                        },
                    },
                    {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 3,  /* Self index. */
                        },
                    },
                    /* Sent on Cycle 0. */
                    {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 4,  /* Self index. */
                        },
                    },
                    {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 5,  /* Self index. */
                        },
                    },
                    /* This dynamic frame will never have TX status set, no
                    expected RX will occur. */
                    {
                        .slot_id = 42,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 6,  /* Self index. */
                        },
                    },
                    {
                        .slot_id = 42,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 7,  /* Self index. */
                        },
                    },
                    /* This static frame will never have TX status set, only
                    NULL frame will be received. */
                    {
                        .slot_id = 18,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 8,  /* Self index. */
                        },
                    },
                    {
                        .slot_id = 18,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 9,  /* Self index. */
                        },
                    },
                }, },
            },
            },
        },
        .run = {
            .push_active = true,
            .pdu_map = { .map = {
                { .list = {
                {
                    .slot_id = 7,
                    .frame_config_index = 0,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 7,
                    .frame_config_index = 1,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 11,
                    .frame_config_index = 2,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 11,
                    .frame_config_index = 3,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 39,
                    .frame_config_index = 4,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 39,
                    .frame_config_index = 5,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 42,
                    .frame_config_index = 6,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNone,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 42,
                    .frame_config_index = 7,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 18,
                    .frame_config_index = 8,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNone,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 18,
                    .frame_config_index = 9,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 1,
        },
        .expect = {
            .cycle = 1+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 11,
                .list = {
                    /* Cycle 0. */
                    {
                        .slot_id = 7,
                        .frame_config_index = 0,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                    },
                    {
                        .slot_id = 7,
                        .frame_config_index = 1,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                    {
                        .slot_id = 11,
                        .frame_config_index = 2,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                    {
                        .slot_id = 11,
                        .frame_config_index = 3,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                    {
                        .slot_id = 18,
                        .frame_config_index = 9,
                        .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                        .cycle = 0,
                        .null_frame = true,
                    },
                    {
                        .slot_id = 39,
                        .frame_config_index = 4,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .macrotick = 2090,
                    },
                    {
                        .slot_id = 39,
                        .frame_config_index = 5,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .macrotick = 2090,
                    },
                    /* Cycle 1. */
                    {
                        .slot_id = 7,
                        .frame_config_index = 1,
                        .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                        .cycle = 1,
                        .null_frame = true,
                    },
                    {
                        .slot_id = 11,
                        .frame_config_index = 2,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 1,
                    },
                    {
                        .slot_id = 11,
                        .frame_config_index = 3,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 1,
                    },
                    {
                        .slot_id = 18,
                        .frame_config_index = 9,
                        .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                        .cycle = 1,
                        .null_frame = true,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void null_frame_inhibit(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                    /* Sent on Cycle 0, then Tx/Rx NULL on Cycle 1,2,3 ... */
                    {
                        .slot_id = 7,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                    {
                        .slot_id = 7,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                        .inhibit_null = true,
                    },
                }, },
            },
            },
        },
        .run = {
            .push_active = true,
            .pdu_map = { .map = {
                { .list = {
                {
                    .slot_id = 7,
                    .frame_config_index = 0,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 7,
                    .frame_config_index = 1,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 1,
        },
        .expect = {
            .cycle = 1+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 2,
                .list = {
                    /* Cycle 0. */
                    {
                        .slot_id = 7,
                        .frame_config_index = 0,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                    },
                    {
                        .slot_id = 7,
                        .frame_config_index = 1,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                    /* Cycle 1. */
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void null_config_inhibit(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_inhibit,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                    /* Sent on Cycle 0, then Tx/Rx NULL on Cycle 1,2,3 ... */
                    {
                        .slot_id = 7,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                    {
                        .slot_id = 7,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                }, },
            },
            },
        },
        .run = {
            .push_active = true,
            .pdu_map = {
                .map = {
                    { .list = {
                    {
                        .slot_id = 7,
                        .frame_config_index = 0,
                        .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                    {
                        .slot_id = 7,
                        .frame_config_index = 1,
                        .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                    },
                }, }, },
            },
            .cycles = 1,
        },
        .expect = {
            .cycle = 1+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 2,
                .list = {
                    /* Cycle 0. */
                    {
                        .slot_id = 7,
                        .frame_config_index = 0,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                    },
                    {
                        .slot_id = 7,
                        .frame_config_index = 1,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                    /* Cycle 1. */
                },
            },
        },
    };
    flexray_harness_run_test(&mock->test);
}

int run_pdu_flexray_null_frame_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;
#define T cmocka_unit_test_setup_teardown

    const struct CMUnitTest tests[] = {
        T(null_frames, s, t),
        T(null_frame_inhibit, s, t),
        T(null_config_inhibit, s, t),
    };

    return cmocka_run_group_tests_name(
        "PDU  FLEXRAY::NULL_FRAME", tests, NULL, NULL);
}
