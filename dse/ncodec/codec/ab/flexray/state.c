// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/logger.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))


static void __poc_state_transition(
    FlexRayState* state, NCodecPduFlexrayPocState target)
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
    log_info("POC State Transition %s -> %s", _t[state->poc_state], _t[target]);
    state->poc_state = target;
}

int process_poc_command(FlexRayState* state, NCodecPduFlexrayPocCommand command)
{
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

    return 0;
}
