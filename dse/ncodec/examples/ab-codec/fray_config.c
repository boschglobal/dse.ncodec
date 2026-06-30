// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <dse/ncodec/interface/pdu.h>


NCodecPduFlexrayConfig fray_config = {
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
};

NCodecPduFlexrayLpduConfig fray_frames[] = {
    {
        .slot_id = 7,
        .payload_length = 64,
        .base_cycle = 0,
        .cycle_repetition = 1,
        .direction = NCodecPduFlexrayDirectionRx,
        .index = {
            .frame_table = 0,  /* Self index. */
        },
    },
    {
        .slot_id = 8,
        .payload_length = 64,
        .base_cycle = 0,
        .cycle_repetition = 1,
        .direction = NCodecPduFlexrayDirectionRx,
        .index = {
            .frame_table = 1,  /* Self index. */
        },
    },
    {
        .slot_id = 9,
        .payload_length = 64,
        .base_cycle = 1,
        .cycle_repetition = 2,
        .direction = NCodecPduFlexrayDirectionRx,
        .index = {
            .frame_table = 2,  /* Self index. */
        },
    },
    { .slot_id = 0 }, /* End of table marker. */
};

size_t frame_count(void)
{
    size_t count = 0;
    while (fray_frames[count].slot_id != 0) {
        count++;
    }
    return count;
}
