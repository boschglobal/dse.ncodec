// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/stream/stream.h>


#define UNUSED(x) ((void)x)
#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "swc_id=1;ecu_id=1;loopback=1"
#define BUFFER_LEN 1024  // Initial buffer size, will grow as needed.

#define CHECK_RC(call)                                                         \
    do {                                                                       \
        rc = (call);                                                           \
        if (rc < 0 && rc != -ENOMSG) {                                         \
            printf("Error: call:%s rc=%d\n", #call, rc);                       \
            return rc;                                                         \
        }                                                                      \
    } while (0)

static const char* greeting = "Hello World";

int step(NCODEC* nc)
{
    int rc = 0;

    /* Read PDUs from the NCodec. */
    while (1) {
        NCodecPdu msg = {};
        CHECK_RC(ncodec_read(nc, &msg));
        if (rc == -ENOMSG) break;
        printf("Message is: %s\n", (char*)msg.payload);
    }
    CHECK_RC(ncodec_truncate(nc)); /* Always call once per step. */

    /* Write PDUs to the NCodec. */
    CHECK_RC(ncodec_write(nc, &(struct NCodecPdu){
                                  .id = 42,
                                  .payload = (uint8_t*)greeting,
                                  .payload_len = strlen(greeting),
                              }));
    CHECK_RC(ncodec_flush(nc)); /* Call once after writing PDUs. */

    return rc;
}

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    int rc = 0;

    /* Open NCodec with buffer stream. */
    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    NCODEC*             nc = ncodec_open(MIMETYPE, stream);

    /* Push a PDU onto the stream (note operating in loopback mode). */
    CHECK_RC(ncodec_write(nc, &(struct NCodecPdu){
                                  .id = 42,
                                  .payload = (uint8_t*)greeting,
                                  .payload_len = strlen(greeting),
                              }));
    CHECK_RC(ncodec_flush(nc));
    CHECK_RC(ncodec_seek(nc, 0, NCODEC_SEEK_SET));

    /* Complete a typical Co-Simulation step. */
    for (int i = 0; i < 1; i++) {
        if ((rc = step(nc)) != 0) break;
    }

    /* Close the NCodec. */
    ncodec_close(nc);

    return rc;
}
