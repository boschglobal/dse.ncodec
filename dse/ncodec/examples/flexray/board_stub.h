// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_EXAMPLES_FLEXRAY_BOARD_STUB_H_
#define DSE_NCODEC_EXAMPLES_FLEXRAY_BOARD_STUB_H_

#define PIN_FR_CC0_TRCV     42
#define PIN_FR_CC0_WUP      43
#define PIN_FR_WAKEUP_EVENT 44 /* Virtual PIN. */

typedef enum {
    PowerOn = 0,
    PowerOff = 1,
    PowerNC = 2,
} PowerState;

typedef enum {
    PinLow = 0,
    PinHigh = 1,
    PinFloat = 2,
} PinState;

PowerState board_get_power_state(int pin);
void       board_set_power_state(int pin, PowerState state);
PinState   board_get_pin_state(int pin);
void       board_set_pin_state(int pin, PinState state);

#endif  // DSE_NCODEC_EXAMPLES_FLEXRAY_BOARD_STUB_H_
