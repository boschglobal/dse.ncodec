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
        { .list = {
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
        }, },
    },
};
#define PAYLOAD_5  "hello 5 world"
#define PAYLOAD_10 "hello 10 world"
static TestPduMap pdu_map_POP_A = {
    .map = {
        /* POP */
        { .list = {
            {
                .slot_id = 10,
                .frame_config_index = 1,
                .lpdu_status = NCodecPduFlexrayLpduStatusReceived,
                .payload = PAYLOAD_10,
                .payload_len = strlen(PAYLOAD_10),
            },
        }, },
        /* Node A */
        { .list = {
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
        }, },
    },
};


void single_node__read_no_messages(void** state)
{
    // Codec with no messages, Bus Model shall return status NC regardless.
    // POP <--> A (ECU_ID__1) ,
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
        },
        .run = {
            .no_push = true,
            .steps = 1,
        },
        .expect = {
            .trace_map = {
                .map = {
                    /* Node PoP */
                    {
                    },
                    /* Node A */
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
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateNoSignal,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 5,
                        .transport.flexray.metadata.status.macrotick = 1400,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller has responded to previous POC commands. */
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
                    /* Node status. */
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

/*
Macrotick estimation
--------------------
Extract primary parameters from PoP Config message.
    - On state change to NormalActive or FrameSync: reset to 0.
    - On cycle count change: reset to 0, add offset-adjust.
    - Otherwise, increment by step-macrotick (on basis 0.0005).
    - On Static LPDU TxRx: calculate PDU Macrotick, shift Macrotick upwards
      only (no negative adjust in-cycle, negative adjust only on reset-to-0).
    - Track offset-adjust (i.e. calculated PDU Macrotick % step-macrotick).

PoP (node identifier 0:0:0) provdes Status message with current cycle count.

PoP Bus Model tracks only messages from PoP (node identifier 0:0:0) when
estimating the Macrotick (i.e. inspection of LPDU messages).

PoP Bus Model injects the calculated Macrotick into Status messages sent
to ECU's _only_ when the PoP does not provide a Macrotick value in its
own Status message.
*/

void macrotick_estimation__state__not_frame_sync(void** state)
{
    /*
    When reported bus state (not node state) is not frame_sync, then force
    cycle and macrotick to 0.
    */
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateNoConnection,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateNoConnection,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateConfig,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateNoSignal,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateNoSignal,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateReady,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameError,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameError,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalPassive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
            },
        },
        .expect = {
            .force_trace_checks = true,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                                NCodecPduFlexrayTransceiverStateNoConnection,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateConfig,
                        },
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
                                NCodecPduFlexrayPocStateReady,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameError,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalPassive,
                        },
                    },
                },
            },
        },
    };
    flexray_harness_run_pop_test(&mock->test);
}

void macrotick_estimation__state__transition_frame_sync__cycle_0(void** state)
{
    /*
    Ensure on the transition to FrameSync, both cycle and macrotick start
    with 0 values.
    */
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameError,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameError,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateConfig,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
            },
        },
        .expect = {
            .force_trace_checks = true,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                                NCodecPduFlexrayTransceiverStateFrameError,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateConfig,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 338,
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

void macrotick_estimation__state__transition_frame_sync__mid_cycle(void** state)
{
    /*
    Ensure on the transition to FrameSync, mid cycle, cycle is not change, and
    macrotick starts with 0 values.
    */
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 7,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameError,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 7,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameError,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateConfig,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 7,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 7,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 7,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 7,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
            },
        },
        .expect = {
            .force_trace_checks = true,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                                NCodecPduFlexrayTransceiverStateFrameError,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateConfig,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 7,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 7,
                            .transport.flexray.metadata.status.macrotick = 338,
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

void macrotick_estimation__cycle_change(void** state)
{
    // Connect to a FrameSync Bus
    // Push the node to NormalActive
    // Node should get cycle and MA from POP/Controller
    // Cycle increments, MA should go to 0
    //
    // POP <--> A (ECU_ID__1) ,
    // Controller -> config -> PoP
    // Controller -> status -> PoP ** cycle = 3
    // A -> config -> POP -> Controller
    // A -> status -> POP -> Controller
    // --- -> POP -> status -> A  **  Macrotick = 338, cycle = 3
    // Controller -> status -> PoP ** cycle = 4
    // --- -> POP -> status -> A  **  Macrotick = 0, cycle = 4
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 4,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 4,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 338,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 4,
                            .transport.flexray.metadata.status.macrotick = 0,
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

void macrotick_estimation__step_increment(void** state)
{
    // Connect to a FrameSync Bus
    // Push the node to NormalActive
    // Node should get cycle and MA from POP/Controller
    // No Tx (but LPDU are pushed)
    //
    // POP <--> A (ECU_ID__1) ,
    // Controller -> config -> PoP
    // Controller -> status -> PoP
    // A -> config -> POP -> Controller
    // A -> status -> POP -> Controller
    // --- -> POP -> status -> A  **  Macrotick = 0
    // Controller -> status -> PoP
    // --- -> POP -> status -> A  **  Macrotick = 0 + step-ma
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 338,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 676,
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

void macrotick_estimation__no_config(void** state)
{
    /* Check div 0 and other conditions. */
    //__log_level__ = LOG_DEBUG;
    testnode_POP.config = (NCodecPduFlexrayConfig){ 0 };
    testnode_A.config = config;

    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                testnode_POP,
                testnode_A,
            },
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
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

void macrotick_estimation__partial_config(void** state)
{
    /* Check div 0 and other conditions. */
    //__log_level__ = LOG_DEBUG;
    testnode_POP.config =
        (NCodecPduFlexrayConfig){ .bit_rate = NCodecPduFlexrayBitrate10 };
    testnode_A.config = config;

    Mock* mock = *state;
    mock->test = (TestTxRx){
        .config = {
            .node = {
                testnode_POP,
                testnode_A,
            },
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 3,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 3,
                            .transport.flexray.metadata.status.macrotick = 0,
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

void macrotick_estimation__lpdu_adjust__static_part(void** state)
{
    /* Macrotick is at 0, LPDU in the dynamic part, push MT to start dynamic
     * part.*/
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : LPDU in static part */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                        .id = 32,
                        .transport.flexray.metadata.lpdu.cycle = 0,
                        .transport.flexray.metadata.lpdu.frame_config_index = 0,
                        .transport.flexray.metadata.lpdu.status =
                            NCodecPduFlexrayLpduStatusTransmitted,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : LPDU in static part */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                        .id = 30,  /* The earlier (higher) value shall prevail. */
                        .transport.flexray.metadata.lpdu.cycle = 0,
                        .transport.flexray.metadata.lpdu.frame_config_index = 0,
                        .transport.flexray.metadata.lpdu.status =
                            NCodecPduFlexrayLpduStatusTransmitted,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 1815,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeLpdu,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeLpdu,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 2153,
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

void macrotick_estimation__lpdu_adjust__dynamic_part(void** state)
{
    /* Macrotick is at 0, LPDU in the dynamic part, push MT to start dynamic
     * part.*/
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : LPDU in dynamic part */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                        .id = 100,
                        .transport.flexray.metadata.lpdu.cycle = 0,
                        .transport.flexray.metadata.lpdu.frame_config_index = 0,
                        .transport.flexray.metadata.lpdu.status =
                            NCodecPduFlexrayLpduStatusTransmitted,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 2145,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeLpdu,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 2483,
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
void macrotick_estimation__lpdu_adjust__retard(void** state)
{
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
            //.frame_table = frame_table_POP_A,
        },
        .run = {
            .push_active = false,
            //.pdu_map = NULL,
            .steps = 3,
            .pop_playback_list = {
                /* Step 0 */
                {
                    /* Controller -> PoP : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.macrotick = 1375,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 0,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 0,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 1 */
                {
                    /* Controller -> PoP : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 1,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 1,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    },
                },
                /* Step 2 */
                {
                    /* Controller -> PoP : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : Status */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .transport.flexray.metadata.status.cycle = 0,
                        .transport.flexray.metadata.status.channel[0].tcvr_state =
                            NCodecPduFlexrayTransceiverStateFrameSync,
                        .transport.flexray.metadata.status.channel[0].poc_state =
                            NCodecPduFlexrayPocStateNormalActive,
                    },
                },
                {
                    /* Controller -> PoP -> ECU A : LPDU in static part */
                    .step = 2,
                    .node_idx = 0,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                        .id = 5,
                        .transport.flexray.metadata.lpdu.cycle = 0,
                        .transport.flexray.metadata.lpdu.frame_config_index = 0,
                        .transport.flexray.metadata.lpdu.status =
                            NCodecPduFlexrayLpduStatusTransmitted,
                    },
                },
                {
                    /* ECU A -> PoP -> Controller : Status */
                    .step = 2,
                    .node_idx = 1,
                    .pdu = {
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray.node_ident = { .node.ecu_id = 1 },
                        .transport.flexray.pop_node_ident = { .node.ecu_id = 0 },
                        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus,
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
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
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
                            .transport.flexray.metadata.status.macrotick = 1375,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 1713,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeStatus,
                            .transport.flexray.metadata.status.cycle = 0,
                            .transport.flexray.metadata.status.macrotick = 330,
                            .transport.flexray.metadata.status.channel[0].tcvr_state =
                                NCodecPduFlexrayTransceiverStateFrameSync,
                            .transport.flexray.metadata.status.channel[0].poc_state =
                                NCodecPduFlexrayPocStateNormalActive,
                        },
                        {
                            .transport_type = NCodecPduTransportTypeFlexray,
                            .transport.flexray.node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.pop_node_ident = { .node.ecu_id = 1 },
                            .transport.flexray.metadata_type =
                                NCodecPduFlexrayMetadataTypeLpdu,
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
        T(single_node__read_no_messages, s, t),
        T(single_node__no_controller, s, t),
        T(single_node__controller_normal_active, s, t),
        T(macrotick_estimation__no_config, s, t),
        T(macrotick_estimation__partial_config, s, t),
        T(macrotick_estimation__state__not_frame_sync, s, t),
        T(macrotick_estimation__state__transition_frame_sync__cycle_0, s, t),
        T(macrotick_estimation__state__transition_frame_sync__mid_cycle, s, t),
        T(macrotick_estimation__cycle_change, s, t),
        T(macrotick_estimation__step_increment, s, t),
        T(macrotick_estimation__lpdu_adjust__static_part, s, t),
        T(macrotick_estimation__lpdu_adjust__dynamic_part, s, t),
        T(macrotick_estimation__lpdu_adjust__retard, s, t),
        T(single_node__tx_rx, s, t),
        T(multi_node__tx_rx, s, t),
        T(single_node__wup, s, t),
        T(single_node__coldstart, s, t),
    };

    return cmocka_run_group_tests_name("PDU  FLEXRAY_POP", tests, NULL, NULL);
}
