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
#include <dse/ncodec/codec/ab/flexray_pop/flexray_pop.h>
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

static TestNode testnode_POP = (TestNode){
    .mimetype = "application/x-automotive-bus; "
                "interface=stream;type=pdu;schema=fbs;"
                "ecu_id=0;name=PoP;model=flexray;mode=pop",
    .node_ident = { .node.ecu_id = 0 },
};
static TestNode testnode_A = (TestNode){
    .mimetype = "application/x-automotive-bus; "
                "interface=stream;type=pdu;schema=fbs;"
                "ecu_id=1;name=ECU1;model=flexray;mode=pop",
    .node_ident = { .node.ecu_id = 1 },
};

static TestFrameMap frame_table_POP_A = {
    .map = {
        /* POP */
        { },
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
                .direction = NCodecPduFlexrayDirectionRx,
                .index = {
                    .frame_table = 1,
                },
            },
        },
    },
};
#define PAYLOAD_5  "hello 5 world"
#define PAYLOAD_10 "hello 10 world"
static TestPduMap pdu_map_POP_A = {
    .map = {
        /* POP */
        {
            // FIXME: check does the POP keep track of this or is it
            // direct from the Controller?
            // TODO: push at slot_id ... to create effect of bus.
            {
                .slot_id = 10,
                .frame_config_index = 1,
                .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                .payload = PAYLOAD_10,
                .payload_len = strlen(PAYLOAD_10),
            },
        },
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
                .lpdu_status = NCodecPduFlexrayLpduStatusNotReceived,
            },
        },
    },
};


void single_node__no_controller(void** state)
{
    // No controller, Bus Model shall return status NC regardless.
    // POP <--> A (ECU_ID__1) ,
    // A -> config -> POP -> ---
    // A -> status -> POP -> ---
    // A -> lpdu -> POP -> ---
    // --- -> POP -> status -> A -> model

    //__log_level__ = LOG_DEBUG;
    testnode_POP.config = config;
    testnode_A.config = config;

    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                testnode_POP,
                testnode_A,
            },
            .frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            .pdu_map = pdu_map_POP_A,
            .steps = 1,
        },
        .expect = {
            .trace_map = {
                .map = {
                    /* Node 0 */
                    {
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1} ,
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 0} ,
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeConfig,
                            .transport.flexray.metadata.config.node_name = "ECU1",
                        },
                    },
                    /* Node 1 */
                    {
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1} ,
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 0} ,
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateNoConnection,
                        }
                    },
                },
            },
        },
    };
    flexray_harness_run_pop_test(&mock->test);
}


void single_node__controller_normal_active(void** state)
{
    // Connect to a FrameSync Bus
    // Push the node to NormalActive
    // Node should get cycle and MA from POP/Controller
    // No Tx (but LPDU are pushed)
    //
    // POP <--> A (ECU_ID__1) ,
    // A -> config -> POP -> Controller
    // A -> status -> POP -> Controller
    // A -> lpdu -> POP -> Controller
    // Controller -> POP -> status -> A -> model

    //__log_level__ = LOG_DEBUG;
    testnode_POP.config = config;
    testnode_A.config = config;

    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                testnode_POP,
                testnode_A,
            },
            .frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = true,
            .pdu_map = pdu_map_POP_A,
            .steps = 2,
            .pop_playback_list = {
                {
                    /* Step 0 ... Controller setup (POC: config/ready/run), but 
                    has not responded via callbacks. */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type =
                            NCodecPduFlexrayMetadataTypeStatus,
                        /* Force to 0 while not in FrameSync. */
                        .transport.flexray.metadata.status.cycle = 0,
                        /* Force to 0 while not in FrameSync. */
                        .transport.flexray.metadata.status.macrotick = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateNoSignal,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalPassive,
                    },
                },
                {
                    /* Step 1 ... Controller has responded to previous POC commands. */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type =
                            NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 5,
                        .transport.flexray.metadata.status.macrotick = 1400,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                         NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* Step 1 ... Node status. */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type =
                            NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.channel[0].poc_command =
                            NCodecPduFlexrayCommandNone,
                    },
                },
            },
        },
        .expect = {
            .trace_map = {
                .map = {
                    /* Node 0 */
                    {
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeConfig,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.channel[0].poc_command =
                                NCodecPduFlexrayCommandConfig,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.channel[0].poc_command =
                                NCodecPduFlexrayCommandReady,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.channel[0].poc_command =
                                NCodecPduFlexrayCommandRun,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeLpdu,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeLpdu,
                        },
                    },
                    /* Node 1 */
                    {
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateNoSignal,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalPassive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 5,
                            .transport.flexray.metadata.status.macrotick = 1400,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                    },
                },
            },
        },
    };
    flexray_harness_run_pop_test(&mock->test);
}

void single_node__tx_rx(void** state)
{
    Mock* mock = *state;
    skip();
}

void multi_node__tx_rx(void** state)
{
    Mock* mock = *state;
    skip();
}

void single_node__wup(void** state)
{
    Mock* mock = *state;
    skip();
}

void single_node__coldstart(void** state)
{
    Mock* mock = *state;
    skip();
}


int run_pdu_flexray_pop(void)
{
    void* s = test_setup;
    void* t = test_teardown;
#define T cmocka_unit_test_setup_teardown

    const struct CMUnitTest tests[] = {
        T(single_node__no_controller, s, t),
        T(single_node__controller_normal_active, s, t),
        T(single_node__tx_rx, s, t),
        T(multi_node__tx_rx, s, t),
        T(single_node__wup, s, t),
        T(single_node__coldstart, s, t),
    };

    return cmocka_run_group_tests_name("PDU  FLEXRAY_POP", tests, NULL, NULL);
}
