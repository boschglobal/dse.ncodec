// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/stream/stream.h>

#define UNUSED(x) ((void)x)
#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "swc_id=2;ecu_id=1"
#define VR_RX 1  // RX from perspective of FMU
#define VR_TX 2  // TX from perspective of FMU

static char* _rx_tx_buffer = NULL;

int fmi2GetString(void* c, const unsigned int vr[], size_t nvr, char* value[])
{
    UNUSED(c);
    if ((nvr == 1) && (vr[0] == VR_TX)) {
        value[0] = _rx_tx_buffer;
    }
    return 0;
}

int fmi2SetString(
    void* c, const unsigned int vr[], size_t nvr, const char* value[])
{
    UNUSED(c);
    if ((nvr == 1) && (vr[0] == VR_RX)) {
        free(_rx_tx_buffer);
        _rx_tx_buffer = strdup(value[0]);
    }
    return 0;
}

int fmi2DoStep(void* c, double currentCommunicationPoint,
    double communicationStepSize, bool noSetFMUStatePriorToCurrentPoint)
{
    UNUSED(c);
    UNUSED(currentCommunicationPoint);
    UNUSED(communicationStepSize);
    UNUSED(noSetFMUStatePriorToCurrentPoint);
    uint8_t* buffer = NULL;
    size_t   buffer_len = 0;

    /* RX Codec - setup buffer and prime for reading. */
    buffer = (uint8_t*)ncodec_ascii85_decode(_rx_tx_buffer, &buffer_len);
    free(_rx_tx_buffer);
    NCODEC* rx_nc = ncodec_open(MIMETYPE, ncodec_buffer_stream_create(0));
    ((NCodecInstance*)rx_nc)->stream->write(rx_nc, buffer, buffer_len);
    ncodec_seek(rx_nc, 0, NCODEC_SEEK_SET);

    /* TX Codec - note `swc_id` is different from main.c to avoid filtering. */
    NCODEC* tx_nc = ncodec_open(MIMETYPE, ncodec_buffer_stream_create(0));

    /* RX -> TX operation using NCodec objects. */
    int rc;
    while (1) {
        NCodecPdu msg = {};
        rc = ncodec_read(rx_nc, &msg);
        if (rc < 0) break;
        /* Shallow copy to avoid copying RX metadata. */
        ncodec_write(tx_nc, &(struct NCodecPdu){ .id = 24,
                                .payload = msg.payload,
                                .payload_len = msg.payload_len });
    }
    ncodec_flush(tx_nc);

    /* TX - extract buffer and encode. */
    ncodec_seek(tx_nc, 0, NCODEC_SEEK_SET);
    ((NCodecInstance*)tx_nc)
        ->stream->read(tx_nc, &buffer, &buffer_len, NCODEC_POS_NC);
    _rx_tx_buffer = ncodec_ascii85_encode((char*)buffer, buffer_len);

    /* Destroy the NCodec objects. */
    ncodec_close(rx_nc);
    ncodec_close(tx_nc);

    return 0;
}
