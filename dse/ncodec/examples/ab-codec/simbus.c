// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>


int simbus(void* nodes, size_t node_size, size_t nc_offset, size_t count)
{
    uint8_t* buffer = NULL;
    size_t   buffer_len = 0;

    /* Consolidate streams from all nodes. */
    for (size_t i = 0; i < count; i++) {
        uint8_t*         node = (uint8_t*)nodes + (i * node_size);
        NCODEC**         nc_ptr = (NCODEC**)(node + nc_offset);
        NCODEC*          ncodec = *nc_ptr;
        ABCodecInstance* nc = (ABCodecInstance*)ncodec;
        NCodecStreamVTable* stream = (NCodecStreamVTable*)nc->c.stream;

        ncodec_seek(ncodec, 0, NCODEC_SEEK_SET);
        uint8_t* data = NULL;
        size_t   len = 0;
        stream->read(ncodec, &data, &len, NCODEC_POS_NC);
        if (len == 0) continue;

        buffer = realloc(buffer, buffer_len + len);
        memcpy(&buffer[buffer_len], data, len);
        buffer_len += len;
    }

    /* Push consolidated stream to all nodes. */
    for (size_t i = 0; i < count; i++) {
        uint8_t*         node = (uint8_t*)nodes + (i * node_size);
        NCODEC**         nc_ptr = (NCODEC**)(node + nc_offset);
        NCODEC*          ncodec = *nc_ptr;
        ABCodecInstance* nc = (ABCodecInstance*)ncodec;
        NCodecStreamVTable* stream = (NCodecStreamVTable*)nc->c.stream;

        ncodec_seek(ncodec, 0, NCODEC_SEEK_RESET);
        stream->write(ncodec, buffer, buffer_len);
        ncodec_seek(ncodec, 0, NCODEC_SEEK_SET);
    }

    free(buffer);
    return 0;
}
