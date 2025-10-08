// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/stream/stream.h>

#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "swc_id=1;ecu_id=1"
#define BUFFER_LEN 1024  // Initial buffer size, will grow as needed.
#define VR_RX      1     // RX from perspective of FMU
#define VR_TX      2     // TX from perspective of FMU

extern int fmi2GetString(
    void* c, const unsigned int vr[], size_t nvr, char* value[]);
extern int fmi2SetString(
    void* c, const unsigned int vr[], size_t nvr, const char* value[]);
extern int fmi2DoStep(void* c, double currentCommunicationPoint,
    double communicationStepSize, bool noSetFMUStatePriorToCurrentPoint);

static void _log(const char* prefix, const char* format, ...)
{
    if (prefix != NULL) {
        printf("%s: ", prefix);
    }
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

int _ncodec_fault(const char* call, int rc)
{
    _log("Error", "call:%s rc=%d", call, rc);
    return rc;
}

int main(void)
{
    int         rc;
    const char* greeting = "Hello World";
    uint8_t*    buffer;
    size_t      buffer_len;
    char*       fmi_string;

    /* Create the NCODEC object with a simple buffer stream. */
    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    NCODEC*             nc = ncodec_open(MIMETYPE, stream);

    /* Write some messages to the NCodec. */
    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting,
                              .payload_len = strlen(greeting) });
    if (rc < 0) return _ncodec_fault("ncodec_write", rc);
    rc = ncodec_flush(nc);
    if (rc < 0) return _ncodec_fault("ncodec_flush", rc);

    /* Intercept the stream buffer, and encode for FMI 2 String Variable. */
    buffer = NULL;
    buffer_len = 0;
    rc = ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    if (rc) return _ncodec_fault("ncodec_seek", rc);
    stream->read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
    fmi_string = ncodec_ascii85_encode((char*)buffer, buffer_len);
    _log("BUFFER TX", "(%d)", buffer_len);
    _log("ASCII85 TX", "(%d) %s", strlen(fmi_string), fmi_string);

    /* Interact with the FMU for a single Co-Simulation step. */
    rc = ncodec_truncate(nc);
    if (rc) return _ncodec_fault("ncodec_truncate", rc);
    fmi2SetString(
        NULL, (unsigned int[]){ VR_RX }, 1, (const char*[]){ fmi_string });
    free(fmi_string); /* Release the fmi_string, FMU will have a copy. */
    fmi_string = NULL;
    fmi2DoStep(NULL, 0.0, 0.0005, false);
    char* v[] = { NULL };
    fmi2GetString(NULL, (unsigned int[]){ VR_TX }, 1, v);
    if (v[0] == NULL) return _ncodec_fault("fmi2GetString - no data", -ENODATA);

    /* Decode the FMI 2 String Variable and inject into the stream buffer. */
    _log("ASCII85 RX", "(%d) %s", strlen(v[0]), v[0]);
    buffer = (uint8_t*)ncodec_ascii85_decode(v[0], &buffer_len);
    _log("BUFFER RX", "(%d)", buffer_len);
    stream->write(nc, buffer, buffer_len);
    free(buffer);
    buffer = NULL;
    buffer_len = 0;
    rc = ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    if (rc) return _ncodec_fault("ncodec_seek", rc);

    /* Read the messages from the NCodec. */
    while (1) {
        NCodecPdu msg = {};
        rc = ncodec_read(nc, &msg);
        if (rc == -ENOMSG) break;
        if (rc < 0) return _ncodec_fault("ncodec_read", rc);
        printf("Message is: %s\n", (char*)msg.payload);
    }

    return 0;
}
