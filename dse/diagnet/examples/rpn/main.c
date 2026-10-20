// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>


#define BUFFER_LEN 4096
#define MIMETYPE                                                               \
    "application/x-automotive-bus;"                                            \
    "interface=diagnostic;type=srpc;schema=fbs;"                               \
    "ecu_id=1;loopback=1"


int run_example(NCODEC* nc);


NCODEC* ncodec_open(const char* mime_type, NSTREAM* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc) {
        NCodecInstance* _nc = (NCodecInstance*)nc;
        _nc->stream = stream;
    }
    return nc;
}


int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        return 1;
    }

    /* Create NCodec. */
    NSTREAM* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    NCODEC*  nc = ncodec_open(MIMETYPE, stream);
    if (nc == NULL) return 1;

    /* Run the example. */
    run_example(nc);

    /* Cleanup. */
    ncodec_close(nc);
    return 0;
}
