// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

/*
FlexRay WUP Example
===================

FlexRay Network (Simulated)
└── Bootloader / Runnable
    └── NCodec ── FlexRay PDU Stream Interface
    └── Virtual ECU
        └── Board API
        └── ECU API
        └── FlexRay "Any Cpu" API
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <board_stub.h>
#include <ecu_stub.h>
#include <flexray_anycpu.h>

#define UNUSED(x)  ((void)x)
#define BUFFER_LEN 1024


/* FlexRay WUP implementation with NCodec*/
#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=1;cc_id=0;swc_id=1"

static NCODEC* nc = NULL;
extern NCODEC* setup_ncodec(const char* mime_type, size_t buffer_size);
extern NCodecPduFlexrayStatus get_status(NCODEC* nc);

void do_step(double simulation_time)
{
    NCodecPduFlexrayStatus fr_status = {};
    /* Power up the FlexRay transceiver of Communication Controller 0. */
    if (board_get_power_state(PIN_FR_CC0_TRCV) != PowerOff) {
        /* The transceiver will enter its default "power on" state and listen
        for WUP bus signals on its connected channels, or its External
        Wakeup Pin interface. */
        board_set_power_state(PIN_FR_CC0_TRCV, PowerOn);
        goto do_ecu_run;
    }

    /* Get the Flexray Bus status from NCodec. */
    fr_status = get_status(nc);
    if (fr_status.channel[NCodecPduFlexrayChannelStatusA].tcvr_state ==
        NCodecPduFlexrayTransceiverStateNoSignal) {
        /* The ncodec_read() did not return a status metadata block. */
        goto do_ecu_run;
    }

    /* Calculate WUP condition. */
    FrWupReasonType reason = FrWupNone;
    if (board_get_pin_state(PIN_FR_CC0_WUP) == PinHigh) {
        reason = FrWupPowerOn;
    }
    if (fr_status.channel[NCodecPduFlexrayChannelStatusA].tcvr_state ==
        NCodecPduFlexrayTransceiverStateWUP) {
        if (reason == FrWupPowerOn) {
            reason = FrWupBusAndPin;
        } else {
            reason = FrWupBus;
        }
    }

    /* Indicate WUP condition. */
    if (reason) {
        flexray_anycpu_set_wup(reason);
        board_set_pin_state(PIN_FR_WAKEUP_EVENT, PinHigh);
    }

do_ecu_run:
    /* Always truncate the NCodec object!
    Truncate when finished reading, before writing, and/or even when not using
    the NCodec object in a simulation step. */
    ncodec_truncate(nc);

    /* Set the FlexRay POC State. The POC State is maintained by the NCodec and
    adjusted based on commands from the FlexRay Interface _and_ interactions
    with the FlexRay Bus (i.e. Cold Start). */
    if (fr_status.channel[NCodecPduFlexrayChannelStatusA].tcvr_state ==
        NCodecPduFlexrayTransceiverStateNoSignal) {
        flexray_anycpu_set_poc_state(
            FLEXRAY_CC_INDEX, FLEXRAY_CH_A, NCodecPduFlexrayPocStateUndefined);
    } else {
        flexray_anycpu_set_poc_state(FLEXRAY_CC_INDEX, FLEXRAY_CH_A,
            fr_status.channel[NCodecPduFlexrayChannelStatusA].poc_state);
    }

    /* Run the ECU software. The WUP event will be detected via the board
    interface and the FlexRay part (of the ECU software) will react according
    to the provided WUP reason. */
    ecu_run(simulation_time);

    /* Depending on the implementation of an ECU software, the FlexRay Interface
    may need to be explicitly run so that the FlexRay Job List is executed (if
    the appropriate conditions have been reached). The exact sequence will
    depend on the implementation. */
    flexray_anycpu_run();
}

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    nc = setup_ncodec(MIMETYPE, BUFFER_LEN);

    double simulation_time = 0.0;
    for (int i = 0; i < 10; i++) {
        do_step(simulation_time);
        simulation_time += 0.0005;
    }

    return 0;
}
