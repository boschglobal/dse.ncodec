// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>
#include <flexray_harness.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static TestNode testnode_AB = (TestNode){
    .mimetype = "application/x-automotive-bus; "                               \
        "interface=stream;type=pdu;schema=fbs;"                                \
        "ecu_id=1;vcn=2;model=flexray",
    .config = {
        .bit_rate = NCodecPduFlexrayBitrate10,
        .channel_enable = NCodecPduFlexrayChannelAB,
        .macrotick_per_cycle = 3361u,
        .microtick_per_cycle = 200000u,
        .network_idle_start = (3361u - 5u - 1u),
        .static_slot_length = 55u,
        .static_slot_count = 38u,
        .minislot_length = 6u,
        .minislot_count = 211u,
        .static_slot_payload_length = 64u,
        .inhibit_null_frames = true,
    },
};

#define PAYLOAD "channel test"

typedef struct {
    NCodecPduFlexrayChannel tx;
    NCodecPduFlexrayChannel rx;
    bool                    expect_rx;
} ChannelCheck;

static const ChannelCheck channel_checks[] = {
    { NCodecPduFlexrayChannelA, NCodecPduFlexrayChannelA, true },
    { NCodecPduFlexrayChannelB, NCodecPduFlexrayChannelB, true },
    { NCodecPduFlexrayChannelAB, NCodecPduFlexrayChannelA, true },
    { NCodecPduFlexrayChannelAB, NCodecPduFlexrayChannelB, true },
    { NCodecPduFlexrayChannelA, NCodecPduFlexrayChannelAB, true },
    { NCodecPduFlexrayChannelB, NCodecPduFlexrayChannelAB, true },
    { NCodecPduFlexrayChannelA, NCodecPduFlexrayChannelB, false },
    { NCodecPduFlexrayChannelB, NCodecPduFlexrayChannelA, false },
};

void flexray__channel_tx_rx(void** state)
{
    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = { testnode_AB },
        },
        .run = {
            .push_active = true,
        },
        .expect = {
            .cycle = 1,
            .poc_state = NCodecPduFlexrayPocStateNormalActive,
            .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
        },
    };

    TestTxRx* test = &mock->test;
    for (size_t i = 0; i < ARRAY_SIZE(channel_checks); i++) {
        uint16_t            slot = (uint16_t)(i + 1);
        uint16_t            tx_index = (uint16_t)(i * 2);
        uint16_t            rx_index = (uint16_t)(tx_index + 1);
        const ChannelCheck* check = &channel_checks[i];

        test->config.frame_table.map[0].list[tx_index] =
            (NCodecPduFlexrayLpduConfig){
                .slot_id = slot,
                .payload_length = 64,
                .base_cycle = 0,
                .cycle_repetition = 1,
                .index.frame_table = tx_index,
                .direction = NCodecPduFlexrayDirectionTx,
                .channel = check->tx,
            };
        test->config.frame_table.map[0].list[rx_index] =
            (NCodecPduFlexrayLpduConfig){
                .slot_id = slot,
                .payload_length = 64,
                .base_cycle = 0,
                .cycle_repetition = 1,
                .index.frame_table = rx_index,
                .direction = NCodecPduFlexrayDirectionRx,
                .channel = check->rx,
            };

        test->run.pdu_map.map[0].list[tx_index] = (TestPdu){
            .slot_id = slot,
            .frame_config_index = tx_index,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotTransmitted,
            .payload = PAYLOAD,
            .payload_len = strlen(PAYLOAD),
        };
        test->run.pdu_map.map[0].list[rx_index] = (TestPdu){
            .slot_id = slot,
            .frame_config_index = rx_index,
            .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
        };

        test->expect.pdu.list[test->expect.pdu.count++] = (TestPdu){
            .slot_id = slot,
            .frame_config_index = tx_index,
            .lpdu_status = NCodecPduFlexrayLpduStatusTransmitted,
        };
        if (check->expect_rx) {
            test->expect.pdu.list[test->expect.pdu.count++] = (TestPdu){
                .slot_id = slot,
                .frame_config_index = rx_index,
                .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                .payload = PAYLOAD,
                .payload_len = strlen(PAYLOAD),
            };
        }
    }

    flexray_harness_run_test(test);
}

int run_pdu_flexray_channel_tests(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            flexray__channel_tx_rx, test_setup, test_teardown),
    };

    return cmocka_run_group_tests_name(
        "PDU FLEXRAY::CHANNEL", tests, NULL, NULL);
}