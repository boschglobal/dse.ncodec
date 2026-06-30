// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>

#define UNUSED(x) ((void)x)

bool trace_off = false;

static void trace_payload(const uint8_t* payload, size_t len)
{
    if (payload == NULL) {
        return;
    }

    printf("  payload (hex):");
    for (size_t i = 0; i < len; ++i)
        printf(" %02X", payload[i]);
    printf("\n");

    printf("  payload (str): \"");
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = payload[i];
        if (c >= 32 && c < 127)
            printf("%c", c);
        else
            printf("\\x%02X", c);
    }
    printf("\"\n");
}

static void trace_read(NCODEC* nc, NCodecMessage* m)
{
    UNUSED(nc);
    if (trace_off) return;
    NCodecPdu* msg = m;
    if (msg->id == 0) return; /* Non payload carrying PDU. */
    printf("TRACE RX: %02d (length=%zu)\n", msg->id, msg->payload_len);
    trace_payload(msg->payload, msg->payload_len);
}

static void trace_write(NCODEC* nc, NCodecMessage* m)
{
    UNUSED(nc);
    if (trace_off) return;
    NCodecPdu* msg = m;
    if (msg->id == 0) return; /* Non payload carrying PDU. */
    printf("TRACE TX: %02d (length=%zu)\n", msg->id, msg->payload_len);
    trace_payload(msg->payload, msg->payload_len);
}

NCODEC* ncodec_open(const char* mime_type, NCodecStreamVTable* stream)
{
    if (stream == NULL) {
        errno = EINVAL;
        return NULL;
    }

    NCODEC* nc = ncodec_create(mime_type);
    if (nc == NULL) return NULL;

    NCodecInstance* _nc = (NCodecInstance*)nc;
    _nc->stream = stream;
    _nc->trace.read = trace_read;
    _nc->trace.write = trace_write;
    return nc;
}
