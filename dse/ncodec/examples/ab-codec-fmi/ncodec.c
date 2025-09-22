// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <dse/logger.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>

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
