// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

/*
FlexRay TxRx Example
====================

FlexRay Network (Simulated)
└── Bootloader / Runnable (Leading Coldstart Node)
    └── NCodec ── FlexRay PDU Stream Interface
    └── Virtual ECU
        └── ECU API
        └── FlexRay "Any Cpu" API
└── Virtual Node (Leading Coldstart Node, simulated via NCodec)
└── Virtual Node (Following Coldstart Node, simulated via NCodec)
*/

#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <ecu_stub.h>
#include <flexray_anycpu.h>

#define UNUSED(x)  ((void)x)
#define BUFFER_LEN 1024


/* FlexRay with NCodec.
Configure with 2 Virtual Coldstart nodes, immediate state: Normal Active*/
#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=1;cc_id=0;swc_id=1;vcn=2"

static NCODEC* nc = NULL;
extern NCODEC* setup_ncodec(const char* mime_type, size_t buffer_size);
extern NCodecPduFlexrayStatus get_status(NCODEC* nc);
extern void flexray_anycpu_push_lpdu(uint16_t config_index, uint16_t lpdu_index,
    const uint8_t* data, uint8_t len);
extern const uint8_t* flexray_anycpu_pull_lpdu(
    uint16_t* config_index, uint8_t* len);

NCodecPduFlexrayLpduConfig lookup(uint32_t id)
{
    UNUSED(id);
    NCodecPduFlexrayLpduConfig config = { 0 };
    return config;
}

void do_step(double simulation_time)
{
    /* Get the Flexray Bus status from NCodec. */
    NCodecPduFlexrayStatus status = get_status(nc);
    flexray_anycpu_set_poc_state(
        FLEXRAY_CC_INDEX, FLEXRAY_CH_A, status.channel[FLEXRAY_CH_A].poc_state);
    flexray_anycpu_set_sync(FLEXRAY_CC_INDEX, status.macrotick, status.cycle);

    /* Read LPDU's from the NCodec Object and push to FlexRay Interface.
    There will only be LPDU's if the POC State is Normal Active. Other frames
    (Startup) are consumed by the NCodec. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    for (;;) {
        NCodecPdu pdu = {};
        if (ncodec_read(nc, &pdu) < 0) break;
        if (pdu.transport_type == NCodecPduTransportTypeFlexray &&
            pdu.transport.flexray.metadata_type ==
                NCodecPduFlexrayMetadataTypeLpdu) {
            NCodecPduFlexrayLpduConfig lpdu_config = lookup(pdu.id);
            flexray_anycpu_push_lpdu(lpdu_config.index.frame_table,
                lpdu_config.index.lpdu_table, pdu.payload,
                (uint8_t)pdu.payload_len);
        }
    }

    goto do_ecu_run;
do_ecu_run:
    /* Always truncate the NCodec object!
    Truncate when finished reading, before writing, and/or even when not using
    the NCodec object in a simulation step. */
    ncodec_truncate(nc);

    /* Run the ECU and FlexRay Interface. */
    ecu_run(simulation_time);
    flexray_anycpu_run();

    /* Write LPDUs to the NCodec.
    The FlexRay Interface will only update LPDU buffers in the NCodec in
    response to Tx requests from ECU software. Existing LPDU buffers in the
    NCodec remain unchanged (and pending transmission). */
    const uint8_t* buffer = NULL;
    uint8_t        buffer_len = 0;
    uint16_t       config_index = 0;
    while ((buffer = flexray_anycpu_pull_lpdu(&config_index, &buffer_len)) !=
           NULL) {
        ncodec_write(nc, &(struct NCodecPdu){
            .id = 0x42,
            .payload = buffer,
            .payload_len = buffer_len,
            .transport_type = NCodecPduTransportTypeFlexray,
            .transport.flexray = {
                .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                .metadata.lpdu = {
                    .status = NCodecPduFlexrayLpduStatusNotTransmitted,
                },
            },
        });
    }

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
