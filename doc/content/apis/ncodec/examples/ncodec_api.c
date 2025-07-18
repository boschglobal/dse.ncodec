// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>


#define MIMETYPE   "application/x-codec-example"
#define UNUSED(x)  ((void)x)
#define BUFFER_LEN 1024


static void trace_read(NCODEC* nc, NCodecMessage* m)
{
    UNUSED(nc);
    NCodecCanMessage* msg = m;
    printf("TRACE RX: %02d (length=%lu)\n", msg->frame_id, msg->len);
}

static void trace_write(NCODEC* nc, NCodecMessage* m)
{
    UNUSED(nc);
    NCodecCanMessage* msg = m;
    printf("TRACE TX: %02d (length=%lu)\n", msg->frame_id, msg->len);
}


int main(int argc, char* argv[])
{
    int                rc;
    static const char* greeting = "Hello World";

    if (argc > 1) {
        rc = ncodec_load(argv[1], NULL);
        if (rc) {
            printf("Load failed (rc %d)\n", rc);
            return rc;
        }
    }

    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);

    NCODEC* nc = ncodec_open(MIMETYPE, stream);
    if (nc == NULL) {
        printf("Open failed (errno %d)\n", errno);
        return errno;
    }
    ncodec_config(nc, (struct NCodecConfigItem){
                          .name = "name", .value = "simple network codec" });

    /* Install trace functions. */
    NCodecInstance* _nc = (NCodecInstance*)nc;
    _nc->trace.read = trace_read;
    _nc->trace.write = trace_write;

    /* Write a message to the Network Codec. */
    ncodec_write(nc, &(struct NCodecCanMessage){ .frame_id = 42,
                         .frame_type = CAN_EXTENDED_FRAME,
                         .buffer = (uint8_t*)greeting,
                         .len = strlen(greeting) });
    ncodec_flush(nc);

    /* Reposition to start of stream. */
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);

    /* Read the response from the Network Codec. */
    NCodecCanMessage msg = {};
    rc = ncodec_read(nc, &msg);
    if (rc > 0) {
        printf("Message is: %s\n", (char*)msg.buffer);
    } else {
        printf("There was no message! (reason %d)\n", rc);
    }

    /* Close the Network Codec. */
    ncodec_close(nc);

    return 0;
}
