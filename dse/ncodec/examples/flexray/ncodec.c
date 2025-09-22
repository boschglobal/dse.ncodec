// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <dse/logger.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/stream/stream.h>
#include <flexray_anycpu.h>

#define UNUSED(x) ((void)x)


uint8_t __log_level__ = LOG_QUIET; /* LOG_QUIET LOG_INFO LOG_DEBUG LOG_TRACE */


static void trace_read(NCODEC* nc, NCodecMessage* m)
{
    UNUSED(nc);
    NCodecPdu* msg = m;
    printf("TRACE RX: %02d (length=%lu)\n", msg->id, msg->payload_len);
}

static void trace_write(NCODEC* nc, NCodecMessage* m)
{
    UNUSED(nc);
    NCodecPdu* msg = m;
    printf("TRACE TX: %02d (length=%lu)\n", msg->id, msg->payload_len);
}

NCODEC* ncodec_open(const char* mime_type, NCodecStreamVTable* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc == NULL || stream == NULL) {
        errno = EINVAL;
        return NULL;
    }
    NCodecInstance* _nc = (NCodecInstance*)nc;
    _nc->stream = stream;
    _nc->trace.read = trace_read;
    _nc->trace.write = trace_write;
    return nc;
}

/* Common functions used in these examples. */
NCODEC* setup_ncodec(const char* mime_type, size_t buffer_size)
{
    NCodecStreamVTable* stream = ncodec_buffer_stream_create(buffer_size);
    NCODEC*             nc = ncodec_open(mime_type, stream);
    if (nc == NULL) {
        printf("Open failed (errno %d)\n", errno);
        exit(errno);
    }
    return nc;
}

NCodecPduFlexrayStatus get_status(NCODEC* nc)
{
    NCodecPduFlexrayStatus fr_status = {};

    /* Search for status metadata, last one wins. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    for (;;) {
        NCodecPdu pdu = {};
        if (ncodec_read(nc, &pdu) < 0) break;
        if (pdu.transport_type != NCodecPduTransportTypeFlexray ||
            pdu.transport.flexray.metadata_type ==
                NCodecPduFlexrayMetadataTypeStatus) {
            fr_status = pdu.transport.flexray.metadata.status;
        }
    }
    if (fr_status.channel[NCodecPduFlexrayChannelStatusA].tcvr_state ==
        NCodecPduFlexrayTransceiverStateNoSignal) {
        /* The ncodec_read() did not return a status metadata block. */
        fr_status.channel[NCodecPduFlexrayChannelStatusA].poc_state =
            NCodecPduFlexrayPocStateUndefined;
    }

    /* Return status. */
    printf("Txcvr State: %d\n",
        fr_status.channel[NCodecPduFlexrayChannelStatusA].tcvr_state);
    printf("POC State: %d\n",
        fr_status.channel[NCodecPduFlexrayChannelStatusA].poc_state);
    return fr_status;
}

void push_config(NCODEC* nc)
{
    FlexrayControllerConfig* config = flexray_anycpu_get_config();
    if (config == NULL) return;

    NCodecPduFlexrayLpduConfig* frame_table =
        calloc(config->frame_config_length, sizeof(NCodecPduFlexrayLpduConfig));
    for (size_t i = 0; i < config->frame_config_length; i++) {
        FlexrayFrameConfig* frame = &config->frame_config_table[i];
        frame_table[i] = (NCodecPduFlexrayLpduConfig){
            .slot_id = frame->slot_id,
            .payload_length = frame->payload_length,
            .cycle_repetition = frame->cycle_config & 0x0f,
            .base_cycle = (frame->cycle_config & 0xf0) >> 4,
            .index = {
                .frame_table = i,
                .lpdu_table = frame->lpdu_table,
            },
            .direction = frame->direction,
            .channel = frame->channel,
            .transmit_mode = frame->transmit_mode,
        };
    }

    /* clang-format off */
    ncodec_write(nc, &(struct NCodecPdu){
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray = {
            .metadata_type = NCodecPduFlexrayMetadataTypeConfig,
            .metadata.config = {
                .macrotick_per_cycle = config->macrotick_per_cycle,
                .microtick_per_cycle = config->microtick_per_cycle,
                .network_idle_start = config->network_idle_start,
                .static_slot_length = config->static_slot_length,
                .static_slot_count = config->static_slot_count,
                .minislot_length = config->minislot_length,
                .minislot_count = config->minislot_count,
                .static_slot_payload_length = config->static_slot_payload_length,

                .bit_rate = config->bit_rate,
                .channel_enable = config->channel_enable,

                .coldstart_node = config->coldstart_node,
                .sync_node = config->sync_node,
                .coldstart_attempts = config->coldstart_attempts,
                .wakeup_channel_select = config->wakeup_channel_select,
                .single_slot_enabled = config->single_slot_enabled,
                .key_slot_id = config->key_slot_id,

                .frame_config = {
                    .table = frame_table,
                    .count = config->frame_config_length,
                },
            },
        },
    });
    /* clang-format on */
}
