// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <assert.h>
#include <dse/ncodec/interface/pdu.h>
#include <flexray_anycpu.h>

#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

void flexray_anycpu_set_sync(uint8_t cc, uint16_t macrotick, uint8_t cycle)
{
    UNUSED(cc);
    UNUSED(macrotick);
    UNUSED(cycle);
}

void flexray_anycpu_set_wup(FrWupReasonType reason)
{
    UNUSED(reason);
}

FlexrayControllerConfig* flexray_anycpu_get_config(void)
{
    static FlexrayFrameConfig frames[] = {
        {
            .slot_id = 10,
            .payload_length = 64,
            .cycle_config = 0x02,
            .frame_config_table = 0, /* Self index. */
            .lpdu_table = 44,        /* Index to LPDU. */
            .direction = NCodecPduFlexrayDirectionTx,
            .channel = NCodecPduFlexrayChannelA,
            .transmit_mode = NCodecPduFlexrayTransmitModeSingleShot,
        },
        {
            .slot_id = 12,
            .payload_length = 128,
            .cycle_config = 0x14,
            .frame_config_table = 1, /* Self index. */
            .lpdu_table = 45,        /* Index to LPDU. */
            .direction = NCodecPduFlexrayDirectionRx,
            .channel = NCodecPduFlexrayChannelA,
            .transmit_mode = NCodecPduFlexrayTransmitModeNone,
        },
    };
    static FlexrayControllerConfig config = {
        .cc_index = 0,

        /* Communication Cycle Config. */
        .macrotick_per_cycle = 100,
        .microtick_per_cycle = 6400,
        .network_idle_start = 4000,
        .static_slot_length = 20,
        .static_slot_count = 50,
        .minislot_length = 4,
        .minislot_count = 500,
        .static_slot_payload_length = 254,
        .bit_rate = NCodecPduFlexrayBitrate10,
        .transmit_mode = NCodecPduFlexrayTransmitModeSingleShot,
        .channel_enable = NCodecPduFlexrayChannelA,

        /* Codestart & Sync Config. */
        .coldstart_node = true,
        .sync_node = false,
        .coldstart_attempts = 4,
        .wakeup_channel_select = 0, /* Channel A. */
        .single_slot_enabled = true,
        .key_slot_id = 2,

        .frame_config_table = frames,
        .frame_config_length = ARRAY_SIZE(frames),
    };
    return &config;
}

static NCodecPduFlexrayPocState flexray_channel_poc_state[FLEXRAY_CH_A + 1] = {
    NCodecPduFlexrayPocStateDefaultConfig
};
void flexray_anycpu_set_poc_state(
    uint8_t cc, uint8_t ch, NCodecPduFlexrayPocState poc_state)
{
    assert(cc == FLEXRAY_CC_INDEX);
    assert(ch < ARRAY_SIZE(flexray_channel_poc_state));
    flexray_channel_poc_state[ch] = poc_state;
}

static NCodecPduFlexrayPocCommand flexray_channel_command[FLEXRAY_CH_A + 1] = {
    NCodecPduFlexrayCommandNone
};
NCodecPduFlexrayPocCommand flexray_get_poc_command(uint8_t cc, uint8_t ch)
{
    assert(cc == FLEXRAY_CC_INDEX);
    assert(ch < ARRAY_SIZE(flexray_channel_command));
    return flexray_channel_command[ch];
}

void flexray_anycpu_run(void)
{
    static NCodecPduFlexrayPocState last_state[FLEXRAY_CH_A + 1] = {
        NCodecPduFlexrayPocStateUndefined
    };

    int ch = FLEXRAY_CH_A;
    if (last_state[ch] == flexray_channel_poc_state[ch]) {
        /* Last command has not completed. */
        flexray_channel_command[ch] = NCodecPduFlexrayCommandNone;
    }
    /* This state-machine will push the controller/channel to Normal Active.*/
    flexray_channel_command[ch] = NCodecPduFlexrayCommandNone;
    switch (flexray_channel_poc_state[ch]) {
    case NCodecPduFlexrayPocStateDefaultConfig:
        flexray_channel_command[ch] = NCodecPduFlexrayCommandConfig;
        break;
    case NCodecPduFlexrayPocStateConfig:  // --> Ready
        flexray_channel_command[ch] = NCodecPduFlexrayCommandReady;
        break;
    case NCodecPduFlexrayPocStateReady:  // --> Wakeup / Startup
        if (1) {                         // TODO WUP_OK
            flexray_channel_command[ch] = NCodecPduFlexrayCommandRun;
        } else {
            flexray_channel_command[ch] = NCodecPduFlexrayCommandWakeup;
        }
        break;
    case NCodecPduFlexrayPocStateWakeup:  // --> Ready
        // TODO delay
        flexray_channel_command[ch] = NCodecPduFlexrayCommandReady;
        break;
    case NCodecPduFlexrayPocStateStartup:  // --> Normal Active
        /* Automatic state transition by controller. */
        break;
    case NCodecPduFlexrayPocStateNormalActive:
        flexray_channel_command[ch] = NCodecPduFlexrayCommandAllSlots;
        break;
    /* These states have no configured commands. */
    case NCodecPduFlexrayPocStateNormalPassive:
    case NCodecPduFlexrayPocStateHalt:
    case NCodecPduFlexrayPocStateFreeze:
    case NCodecPduFlexrayPocStateUndefined:
    default:
        break;
    }
}

void flexray_anycpu_push_lpdu(uint16_t config_index, uint16_t lpdu_index,
    const uint8_t* data, uint8_t len)
{
    UNUSED(config_index);
    UNUSED(lpdu_index);
    UNUSED(data);
    UNUSED(len);
}

const uint8_t* flexray_anycpu_pull_lpdu(uint16_t* config_index, uint8_t* len)
{
    UNUSED(config_index); /* Update with config index of returned LPDU. */
    UNUSED(len);
    return NULL; /* Return NULL when no more LPDU need pull/push to NCodec. */
}
