// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/logger.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

/*
POC State is a result of POC Commands (for a node).

Transceiver State is what is produced on the Bus (by a node).

Consider that a POC State results in a Transceiver State, and then the
transceiver state of _all_ nodes results in a bus condition. And that
bus condition then reflects again on the POC State - since a bus
condition might not be possible for a POC state - a post adjustment
may be required.

Technique:
Apply POC Commands to each node.
    Adjust transceiver state
Determine Bus Condition.
Adjust POC Commands according to Bus Condition.

*/

static void __poc_state_transition(
    FlexRayNodeState* state, NCodecPduFlexrayPocState target)
{
    const char* _t[] = {
        [NCodecPduFlexrayPocStateDefaultConfig] = "DefaultConfig",
        [NCodecPduFlexrayPocStateConfig] = "Config",
        [NCodecPduFlexrayPocStateReady] = "Ready",
        [NCodecPduFlexrayPocStateWakeup] = "Wakeup",
        [NCodecPduFlexrayPocStateStartup] = "Startup",
        [NCodecPduFlexrayPocStateNormalActive] = "NormalActive",
        [NCodecPduFlexrayPocStateNormalPassive] = "NormalPassive",
        [NCodecPduFlexrayPocStateHalt] = "Halt",
        [NCodecPduFlexrayPocStateFreeze] = "Freeze",
        [NCodecPduFlexrayPocStateUndefined] = "Undefined",
    };
    if (target > ARRAY_SIZE(_t)) {
        log_error("Invalid target POC State (%d)", _t[state->poc_state]);
        return;
    }
    log_debug(
        "POC State Transition %s -> %s", _t[state->poc_state], _t[target]);
    state->poc_state = target;
}

static void __set_transceiver_state(FlexRayNodeState* state)
{
    const char* _t[] = {
        [NCodecPduFlexrayTransceiverStateNoPower] = "NoPower",
        [NCodecPduFlexrayTransceiverStateNoConnection] = "NoConnection",
        [NCodecPduFlexrayTransceiverStateNoSignal] = "NoSignal",
        [NCodecPduFlexrayTransceiverStateCAS] = "CAS",
        [NCodecPduFlexrayTransceiverStateWUP] = "WUP",
        [NCodecPduFlexrayTransceiverStateFrameSync] = "FrameSync",
        [NCodecPduFlexrayTransceiverStateFrameError] = "FrameError",
    };
    if (state->tcvr_state == NCodecPduFlexrayTransceiverStateNoPower) {
        /* No Power, adjustment based on POC state not valid. */
        log_debug("Tranceiver State: %s", _t[state->tcvr_state]);
        return;
    }

    switch (state->poc_state) {
    case NCodecPduFlexrayPocStateDefaultConfig:
    case NCodecPduFlexrayPocStateConfig:
        state->tcvr_state = NCodecPduFlexrayTransceiverStateNoSignal;
        break;

    case NCodecPduFlexrayPocStateReady:
    case NCodecPduFlexrayPocStateStartup:
    case NCodecPduFlexrayPocStateNormalPassive:
        state->tcvr_state = NCodecPduFlexrayTransceiverStateFrameError;
        break;

    case NCodecPduFlexrayPocStateWakeup:
        state->tcvr_state = NCodecPduFlexrayTransceiverStateWUP;
        break;

    case NCodecPduFlexrayPocStateNormalActive:
        state->tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync;
        break;

    case NCodecPduFlexrayPocStateHalt:
    case NCodecPduFlexrayPocStateFreeze:
        state->tcvr_state = NCodecPduFlexrayTransceiverStateNoConnection;
        break;

    case NCodecPduFlexrayPocStateUndefined:
        state->tcvr_state = NCodecPduFlexrayTransceiverStateNoConnection;
        break;

    default:
        break;
    }

    if (state->tcvr_state > ARRAY_SIZE(_t)) {
        log_error("Invalid Transceiver State (%d)", state->tcvr_state);
        return;
    }
    log_debug("Tranceiver State: %s", _t[state->tcvr_state]);
}

void set_node_power(
    FlexRayState* state, NCodecPduFlexrayNodeIdentifier nid, bool power_on)
{
    /* Node states are consolidated per Node by zeroing out the `swc_id`. */
    nid.node.swc_id = 0;
    FlexRayNodeState* node_state = vector_find(
        &state->node_state, &(FlexRayNodeState){ .node_ident = nid }, 0, NULL);
    if (node_state) {
        if (power_on &&
            node_state->tcvr_state == NCodecPduFlexrayTransceiverStateNoPower) {
            node_state->tcvr_state =
                NCodecPduFlexrayTransceiverStateNoConnection;
            node_state->poc_state = NCodecPduFlexrayPocStateDefaultConfig;
            log_debug("Power On");
        } else if (power_on == false) {
            node_state->tcvr_state = NCodecPduFlexrayTransceiverStateNoPower;
            node_state->poc_state = NCodecPduFlexrayPocStateDefaultConfig;
            log_debug("Power Off");
        }
    }
}

int process_poc_command(
    FlexRayNodeState* state, NCodecPduFlexrayPocCommand command)
{
    log_debug("POC Command=%d, POC State=%u, Tcrv State=%u", command,
        state->poc_state, state->tcvr_state);
    if (command == NCodecPduFlexrayCommandNone) return 0;

    switch (state->poc_state) {
    case NCodecPduFlexrayPocStateDefaultConfig:
        if (command == NCodecPduFlexrayCommandConfig) {
            __poc_state_transition(state, NCodecPduFlexrayPocStateConfig);
        }
        break;

    case NCodecPduFlexrayPocStateConfig:
        if (command == NCodecPduFlexrayCommandReady) {
            __poc_state_transition(state, NCodecPduFlexrayPocStateReady);
        }
        break;

    case NCodecPduFlexrayPocStateReady:
        switch (command) {
        case NCodecPduFlexrayCommandConfig:
            __poc_state_transition(state, NCodecPduFlexrayPocStateConfig);
            break;
        case NCodecPduFlexrayCommandRun:
            __poc_state_transition(state, NCodecPduFlexrayPocStateNormalActive);
            break;
        default:
            break;
        }
        break;

    case NCodecPduFlexrayPocStateWakeup:
    case NCodecPduFlexrayPocStateStartup:
    case NCodecPduFlexrayPocStateNormalPassive:
        __poc_state_transition(state, NCodecPduFlexrayPocStateNormalActive);
        break;

    case NCodecPduFlexrayPocStateNormalActive:
        switch (command) {
        case NCodecPduFlexrayCommandHalt:
            __poc_state_transition(state, NCodecPduFlexrayPocStateHalt);
            break;
        case NCodecPduFlexrayCommandFreeze:
            __poc_state_transition(state, NCodecPduFlexrayPocStateFreeze);
            break;
        default:
            break;
        }
        break;

    case NCodecPduFlexrayPocStateHalt:
        switch (command) {
        case NCodecPduFlexrayCommandConfig:
            __poc_state_transition(
                state, NCodecPduFlexrayPocStateDefaultConfig);
            break;
        default:
            break;
        }
        break;

    case NCodecPduFlexrayPocStateFreeze:
    case NCodecPduFlexrayPocStateUndefined:
        break;

    default:
        log_error("Unknown POC State (%d)", state->poc_state);
        break;
    }

    __set_transceiver_state(state);

    return 0;
}

static int __node_ident_compar(const void* left, const void* right)
{
    NCodecPduFlexrayNodeIdentifier l = ((FlexRayNodeState*)left)->node_ident;
    NCodecPduFlexrayNodeIdentifier r = ((FlexRayNodeState*)right)->node_ident;

    if (l.node_id < r.node_id) return -1;
    if (l.node_id > r.node_id) return 1;
    return 0;
}


void register_node_state(FlexRayState* state,
    NCodecPduFlexrayNodeIdentifier nid, bool pwr_on, bool pwr_off)
{
    /* Node states are consolidated per Node by zeroing out the `swc_id`. */
    if (state->node_state.capacity == 0) {
        state->node_state =
            vector_make(sizeof(FlexRayNodeState), 0, __node_ident_compar);
    }
    nid.node.swc_id = 0;
    FlexRayNodeState* node_state = vector_find(
        &state->node_state, &(FlexRayNodeState){ .node_ident = nid }, 0, NULL);
    if (node_state == NULL) {
        /* Force power state, typically set via MIME type parameter `pon`. */
        NCodecPduFlexrayTransceiverState tcvr_state =
            NCodecPduFlexrayTransceiverStateNoConnection;
        if (pwr_on) {
            tcvr_state = NCodecPduFlexrayTransceiverStateNoConnection;
        } else if (pwr_off) {
            tcvr_state = NCodecPduFlexrayTransceiverStateNoPower;
        }
        vector_push(&state->node_state,
            &(FlexRayNodeState){ .node_ident = nid, .tcvr_state = tcvr_state });
    } else {
        /* Force power state, typically set via MIME type parameter `pon`. */
        if (pwr_on) {
            node_state->tcvr_state =
                NCodecPduFlexrayTransceiverStateNoConnection;
        } else if (pwr_off) {
            node_state->tcvr_state = NCodecPduFlexrayTransceiverStateNoPower;
        }
    }
}

void register_vcs_node_state(
    FlexRayState* state, NCodecPduFlexrayNodeIdentifier nid)
{
    if (state->vcs_node.capacity == 0) {
        state->vcs_node =
            vector_make(sizeof(FlexRayNodeState), 0, __node_ident_compar);
    }
    if (vector_find(&state->vcs_node, &(FlexRayNodeState){ .node_ident = nid },
            0, NULL) == NULL) {
        vector_push(&state->vcs_node,
            &(FlexRayNodeState){ .node_ident = nid,
                .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync });
    }
}

void push_node_state(FlexRayState* state, NCodecPduFlexrayNodeIdentifier nid,
    NCodecPduFlexrayPocCommand command)
{
    /* Node states are consolidated per Node by zeroing out the `swc_id`. */
    nid.node.swc_id = 0;
    FlexRayNodeState* node_state = vector_find(
        &state->node_state, &(FlexRayNodeState){ .node_ident = nid }, 0, NULL);
    if (node_state) {
        process_poc_command(node_state, command);
        __set_transceiver_state(node_state);
    }
}

FlexRayNodeState get_node_state(
    FlexRayState* state, NCodecPduFlexrayNodeIdentifier nid)
{
    /* Node states are consolidated per Node by zeroing out the `swc_id`. */
    nid.node.swc_id = 0;
    FlexRayNodeState node_state = { 0 };
    vector_find(&state->node_state, &(FlexRayNodeState){ .node_ident = nid }, 0,
        &node_state);
    return node_state;
}

void calculate_bus_condition(FlexRayState* state)
{
    int frame_sync_node_count = 0;
    /* Range over Virtual Coldstart Nodes. */
    for (size_t i = i; i < vector_len(&state->vcs_node); i++) {
        FlexRayNodeState node_state = { 0 };
        vector_at(&state->vcs_node, i, &node_state);
        if (node_state.tcvr_state ==
            NCodecPduFlexrayTransceiverStateFrameSync) {
            frame_sync_node_count++;
        }
    }
    /* Range over Nodes. */
    for (size_t i = i; i < vector_len(&state->node_state); i++) {
        FlexRayNodeState node_state = { 0 };
        vector_at(&state->node_state, i, &node_state);
        if (node_state.tcvr_state ==
            NCodecPduFlexrayTransceiverStateFrameSync) {
            frame_sync_node_count++;
        }
    }

    switch (frame_sync_node_count) {
    case 0:
        state->bus_condition = NCodecPduFlexrayTransceiverStateNoSignal;
        break;
    case 1:
        state->bus_condition = NCodecPduFlexrayTransceiverStateFrameError;
        /* Push ActiveNormal nodes to ActivePassive. */
        for (size_t i = i; i < vector_len(&state->node_state); i++) {
            FlexRayNodeState* node_state =
                vector_at(&state->node_state, i, NULL);
            if (node_state->poc_state == NCodecPduFlexrayPocStateNormalActive) {
                node_state->poc_state = NCodecPduFlexrayPocStateNormalPassive;
                __set_transceiver_state(node_state);
            }
        }
        break;
    default:
        state->bus_condition = NCodecPduFlexrayTransceiverStateFrameSync;
        break;
    }
}

void release_state(FlexRayState* state)
{
    vector_reset(&state->node_state);
    vector_reset(&state->vcs_node);
}
