// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>


#define BUFFER_LEN 4096
#define MIMETYPE                                                               \
    "application/x-automotive-bus;interface=diagnostic;type=srpc;schema=fbs;"  \
    "ecu_name=vECU;ecu_id=1;swc_id=0;loopback=1"


int run_example(NCODEC* nc);


static NCODEC* _ncodec_open(const char* mime_type, NSTREAM* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc) ((NCodecInstance*)nc)->stream = stream;
    return nc;
}


int main(void)
{
    NSTREAM* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    NCODEC*  nc = _ncodec_open(MIMETYPE, stream);
    if (nc == NULL) return EXIT_FAILURE;

    int rc = run_example(nc);
    if (rc != 0) {
        fprintf(stderr, "plant example failed: %d\n", rc);
    }
    ncodec_close(nc);
    return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
