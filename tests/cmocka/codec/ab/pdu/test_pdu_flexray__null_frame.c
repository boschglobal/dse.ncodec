// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/logger.h>
#include <errno.h>
#include <stdio.h>
#include <dse/clib/collections/vector.h>
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


static NCodecPduFlexrayConfig cc_config = {
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
    },
};

#define PAYLOAD_1 "hello world"

void null_frames(void** state)
{
    // configure both dynamic and static, only null for static
    skip();
}

void null_frame_inhibit(void** state)
{
    // configure both dynamic and static, inhibit, no null for static
    skip();
}

void null_config_disable(void** state)
{
    // configure both dynamic and static, disable, no null for static
    skip();
}

int run_pdu_flexray_null_frame_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;
#define T cmocka_unit_test_setup_teardown

    const struct CMUnitTest tests[] = {
        T(null_frames, s, t),
        T(null_frame_inhibit, s, t),
        T(null_config_disable, s, t),
    };

    return cmocka_run_group_tests_name(
        "PDU  FLEXRAY::NULL_FRAME", tests, NULL, NULL);
}
