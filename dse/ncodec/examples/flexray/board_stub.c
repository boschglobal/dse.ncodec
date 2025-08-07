// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <board_stub.h>

static PowerState flexray_cc_trcv_power_state = PowerOff;
static PinState   board_cc0_trcv_wup_pin = PinFloat;
static PinState   flexray_wup_event_pin = PinLow;

PowerState board_get_power_state(int pin)
{
    switch (pin) {
    case PIN_FR_CC0_TRCV:
        return flexray_cc_trcv_power_state;
    default:
        return PowerNC;
    }
}

void board_set_power_state(int pin, PowerState state)
{
    switch (pin) {
    case PIN_FR_CC0_TRCV:
        flexray_cc_trcv_power_state = state;
        break;
    default:
        break;
    }
}

PinState board_get_pin_state(int pin)
{
    switch (pin) {
    case PIN_FR_CC0_WUP:
        return board_cc0_trcv_wup_pin;
        break;
    default:
        return PinFloat;
    }
}

void board_set_pin_state(int pin, PinState state)
{
    switch (pin) {
    case PIN_FR_WAKEUP_EVENT:
        flexray_wup_event_pin = state;
        break;
    default:
        break;
    }
}
