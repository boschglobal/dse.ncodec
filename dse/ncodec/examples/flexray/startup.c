// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

/*
FlexRay Startup Example
=======================

FlexRay Network (Simulated)
└── Bootloader / Runnable (Leading Coldstart Node)
    └── NCodec ── FlexRay PDU Stream Interface
    └── Virtual ECU
        └── ECU API
        └── FlexRay "Any Cpu" API
└── Virtual Node (Following Coldstart Node, simulated via NCodec)
*/

#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <ecu_stub.h>
#include <flexray_anycpu.h>

#define UNUSED(x)  ((void)x)
#define BUFFER_LEN 1024


/* FlexRay Startup sequence with NCodec*/
#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=1;cc_id=0;swc_id=1;vcn=1;vcn_fid=4"

static NCODEC* nc = NULL;
extern NCODEC* setup_ncodec(const char* mime_type, size_t buffer_size);
extern NCodecPduFlexrayStatus get_status(NCODEC* nc);
extern void                   push_config(NCODEC* nc);

static void push_command(NCodecPduFlexrayPocCommand poc_cmd)
{
    if (poc_cmd == NCodecPduFlexrayCommandNone) return;
    /* clang-format off */
    ncodec_write(nc, &(struct NCodecPdu){
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray = {
            .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
            .metadata.status = {
                .channel[FLEXRAY_CH_A].poc_command = poc_cmd,
            },
        },
    });
    /* clang-format on */
    printf("POC Command: %d\n", poc_cmd);
}

void do_step(double simulation_time)
{
    NCodecPduFlexrayPocCommand poc_cmd;
    NCodecPduFlexrayStatus     fr_status;

    /* Get the Flexray Bus status from NCodec. */
    fr_status = get_status(nc);

    goto do_ecu_run;
do_ecu_run:
    /* Always truncate the NCodec object!
    Truncate when finished reading, before writing, and/or even when not using
    the NCodec object in a simulation step. */
    ncodec_truncate(nc);

    flexray_anycpu_set_poc_state(FLEXRAY_CC_INDEX, FLEXRAY_CH_A,
        fr_status.channel[NCodecPduFlexrayChannelStatusA].poc_state);
    ecu_run(simulation_time);
    flexray_anycpu_run();

    /* Process/forward any commands from the FlexRay Interface. */
    poc_cmd = flexray_get_poc_command(FLEXRAY_CC_INDEX, FLEXRAY_CH_A);
    if (poc_cmd == NCodecPduFlexrayCommandConfig) {
        push_config(nc);
    }
    push_command(poc_cmd);

    /* Flush the NCodec object!
    Flush when finished pushing items to write those items to the underlying
    NCodec stream. A NCodec object can be flushed numerous times, each time
    the contents are appended to the underlying NCodec stream. */
    ncodec_flush(nc);
}

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    nc = setup_ncodec(MIMETYPE, BUFFER_LEN);

    double simulation_time = 0.0;
    for (int i = 0; i < 100; i++) {
        do_step(simulation_time);
        simulation_time += 0.0005;
    }

    return 0;
}
