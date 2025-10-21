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
        .inhibit_null_frames = true,
    },
};

#define PAYLOAD_1 "hello world"

void single_node__static__single_frame__tx_rx(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
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
                },
            },
            },
            },
        },
        .run = {
            .push_active = true,
            .pdu_map = { .map = {
                { .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 7,
                        .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 7,
                        .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                    },
                }, }, },
            },
            .cycles = 1,
            .steps = 2,
        },
        .expect = {
            .cycle = 0,
            .macrotick = 660,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 2,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 7,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 7,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__static__single_frame__tx_multi_rx(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
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
                }, },
            },
        },
        },
        .run = {
            .push_active = true,
            .pdu_map = { .map = {
                { .list = {
                {
                    .slot_id = 11,
                    .frame_config_index = 0,
                  .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                  .payload = PAYLOAD_1,
                  .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 11,
                    .frame_config_index = 1,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 11,
                    .frame_config_index = 2,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 11,
                    .frame_config_index = 3,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 1,
            .steps = 2,
        },
        .expect = {
            .cycle = 0,
            .macrotick = 660,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 4,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                   {
                        .frame_config_index = 2,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__static__base_cycle(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 3,
                        .cycle_repetition = 16,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 12,
                        .payload_length = 64,
                        .base_cycle = 6,
                        .cycle_repetition = 32,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 13,
                        .payload_length = 64,
                        .base_cycle = 14,
                        .cycle_repetition = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 2,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 3,
                        .cycle_repetition = 16,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 3,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 12,
                        .payload_length = 64,
                        .base_cycle = 6,
                        .cycle_repetition = 32,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 4,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 13,
                        .payload_length = 64,
                        .base_cycle = 14,
                        .cycle_repetition = 64,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 5,  /* Self index. */
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
                    .slot_id = 11,
                    .frame_config_index = 0,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 12,
                    .frame_config_index = 1,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 13,
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
                    .slot_id = 12,
                    .frame_config_index = 4,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 13,
                    .frame_config_index = 5,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 16,
        },
        .expect = {
            .cycle = 16+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 6,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 3,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 3,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 6,
                    },
                   {
                        .frame_config_index = 4,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 6,
                    },
                   {
                        .frame_config_index = 2,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 14,
                    },
                   {
                        .frame_config_index = 5,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 14,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__static__cycle_repetition__short(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map ={
                { .list = { // Node 0
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 12,
                        .payload_length = 64,
                        .base_cycle = 1,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 13,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 2,
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
                   {
                        .slot_id = 12,
                        .payload_length = 64,
                        .base_cycle = 1,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 4,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 13,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 2,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 5,  /* Self index. */
                        },
                    },
                   {
                    /* This one should never be received as
                      NCodecPduFlexrayLpduStatusReceived is not set. */
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 6,  /* Self index. */
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
                    .slot_id = 11,
                    .frame_config_index = 0,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 12,
                    .frame_config_index = 1,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 13,
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
                    .slot_id = 12,
                    .frame_config_index = 4,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 13,
                    .frame_config_index = 5,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 2,
        },
        .expect = {
            .cycle = 2+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 16,
                .list = {
                    /* Cycle 0. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 4,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 2,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 5,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                    /* Cycle 1. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 1,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 1,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 1,
                    },
                   {
                        .frame_config_index = 4,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 1,
                    },
                    /* Cycle 2. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 2,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 2,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 2,
                    },
                   {
                        .frame_config_index = 4,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 2,
                    },
                   {
                        .frame_config_index = 2,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 2,
                    },
                   {
                        .frame_config_index = 5,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 2,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__static__cycle_repetition__long(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 16,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 16,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 12,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
                        .index = {
                            .frame_table = 2,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 12,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 64,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 3,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 13,
                        .payload_length = 64,
                        .base_cycle = 63,
                        .cycle_repetition = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
                        .index = {
                            .frame_table = 4,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 13,
                        .payload_length = 64,
                        .base_cycle = 63,
                        .cycle_repetition = 64,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 5,  /* Self index. */
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
                    .slot_id = 11,
                    .frame_config_index = 0,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 11,
                    .frame_config_index = 1,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 12,
                    .frame_config_index = 2,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 12,
                    .frame_config_index = 3,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 13,
                    .frame_config_index = 4,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 13,
                    .frame_config_index = 5,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 64,
        },
        .expect = {
            .cycle = 1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 16,
                .list =  {
                    /* Cycle 0. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 2,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                    /* Cycle 16. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 16,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 16,
                    },
                    /* Cycle 32. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 32,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 32,
                    },
                    /* Cycle 48. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 48,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 48,
                    },
                    /* Cycle 63. */
                   {
                        .frame_config_index = 4,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 63,
                    },
                   {
                        .frame_config_index = 5,
                        .slot_id = 13,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 63,
                    },
                    /* Cycle 64 (0). */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 2,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__static__transmit_mode(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 11,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 12,
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
                        .slot_id = 12,
                        .payload_length = 64,
                        .base_cycle = 0,
                        .cycle_repetition = 1,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 3,  /* Self index. */
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
                    .slot_id = 11,
                    .frame_config_index = 0,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 11,
                    .frame_config_index = 1,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .slot_id = 12,
                    .frame_config_index = 2,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .slot_id = 12,
                    .frame_config_index = 3,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 2,
        },
        .expect = {
            .cycle = 2+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 8,
                .list = {
                    /* Cycle 0. */
                   {
                        .frame_config_index = 0,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 11,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 2,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                    },
                    /* Cycle 1. */
                   {
                        .frame_config_index = 2,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 1,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 1,
                    },
                    /* Cycle 2. */
                   {
                        .frame_config_index = 2,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 2,
                    },
                   {
                        .frame_config_index = 3,
                        .slot_id = 12,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 2,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__dynamic__single_frame__tx_rx(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 39,
                        .payload_length = 64,
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
            .pdu_map = { .map = {
                { .list = {
                {
                    .frame_config_index = 0,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .frame_config_index = 1,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 0,
        },
        .expect = {
            .cycle = 0+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 2,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .macrotick = 2090,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__dynamic__single_frame__mid_cycle(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 39,
                        .payload_length = 64,
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
            .push_at_cycle = 7,
            .pdu_map = { .map = {
                { .list = {
                {
                    .frame_config_index = 0,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .frame_config_index = 1,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 63,
        },
        .expect = {
            .cycle = 0,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 2,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 7,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 7,
                        .macrotick = 2090,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__dynamic__single_frame__end_cycle(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map ={
                { .list = { // Node 0
                   {
                        .slot_id = 38+211,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 38+211,
                        .payload_length = 64,
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
            .push_at_cycle = 63,
            .pdu_map = { .map = {
                { .list = {
                {
                    .frame_config_index = 0,
                    .slot_id = 38+211,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .frame_config_index = 1,
                    .slot_id = 38+211,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 63,
        },
        .expect = {
            .cycle = 0,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 2,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 38+211,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 63,
                        .macrotick = 3350,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 38+211,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 63,
                        .macrotick = 3350,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__dynamic__multi_frame__tx_rx(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 42,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 42,
                        .payload_length = 64,
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
            .pdu_map = { .map = {
                { .list = {
                {
                    .frame_config_index = 0,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .frame_config_index = 1,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
                {
                    .frame_config_index = 0,
                    .slot_id = 42,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .frame_config_index = 1,
                    .slot_id = 42,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 0,
        },
        .expect = {
            .cycle = 0+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 4,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 0,
                        .slot_id = 42,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                        .macrotick = 2144,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 42,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                        .macrotick = 2144,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}

void single_node__dynamic__transmit_mode(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                 testnode_A,
            },
            .frame_table = { .map = {
                { .list = { // Node 0
                   {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionTx,
                        .transmit_mode = NCodecPduFlexrayTransmitModeContinuous,
                        .index = {
                            .frame_table = 0,  /* Self index. */
                        },
                    },
                   {
                        .slot_id = 39,
                        .payload_length = 64,
                        .direction = NCodecPduFlexrayDirectionRx,
                        .index = {
                            .frame_table = 1,  /* Self index. */
                        },
                    },
                },
                },
            },
            },
        },
        .run = {
            .push_active = true,
            .pdu_map = { .map = {
                { .list = {
                {
                    .frame_config_index = 0,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
                    .payload = PAYLOAD_1,
                    .payload_len = strlen(PAYLOAD_1),
                },
                {
                    .frame_config_index = 1,
                    .slot_id = 39,
                    .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
                },
            }, }, },
            },
            .cycles = 2,
        },
        .expect = {
            .cycle = 2+1,
            .macrotick = 0,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .pdu = {
                .count = 6,
                .list = {
                   {
                        .frame_config_index = 0,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 0,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 0,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 0,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 1,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 1,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 0,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
                        .cycle = 2,
                        .macrotick = 2090,
                    },
                   {
                        .frame_config_index = 1,
                        .slot_id = 39,
                        .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                        .payload = PAYLOAD_1,
                        .payload_len = strlen(PAYLOAD_1),
                        .cycle = 2,
                        .macrotick = 2090,
                    },
                },
            },
        }
    };
    flexray_harness_run_test(&mock->test);
}


int run_pdu_flexray_single_node_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;
#define T cmocka_unit_test_setup_teardown

    const struct CMUnitTest tests[] = {
        T(single_node__static__single_frame__tx_rx, s, t),
        T(single_node__static__single_frame__tx_multi_rx, s, t),
        T(single_node__static__base_cycle, s, t),
        T(single_node__static__cycle_repetition__short, s, t),
        T(single_node__static__cycle_repetition__long, s, t),
        T(single_node__static__transmit_mode, s, t),
        T(single_node__dynamic__single_frame__tx_rx, s, t),
        T(single_node__dynamic__single_frame__mid_cycle, s, t),
        T(single_node__dynamic__single_frame__end_cycle, s, t),
        T(single_node__dynamic__multi_frame__tx_rx, s, t),
        T(single_node__dynamic__transmit_mode, s, t),
    };

    return cmocka_run_group_tests_name(
        "PDU  FLEXRAY::SINGLE_NODE", tests, NULL, NULL);
}
