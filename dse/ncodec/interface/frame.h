// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_INTERFACE_FRAME_H_
#define DSE_NCODEC_INTERFACE_FRAME_H_

#include <stdint.h>


/** NCODEC API - CAN Frame/Stream
    =============================

    Types relating to the implementation of the Stream/Frame interface of
    the NCodec API for transmission of CAN Messages.

    The root type is `NCodecCanMessage` which may be substituted for the
    `NCodecMessage` type when calling NCodec API methods (e.g.
   `ncodec_write()`).
*/

typedef enum NCodecCanFrameType {
    CAN_BASE_FRAME = 0,
    CAN_EXTENDED_FRAME = 1,
    CAN_FD_BASE_FRAME = 2,
    CAN_FD_EXTENDED_FRAME = 3,
} NCodecCanFrameType;

typedef struct NCodecCanMessage {
    uint32_t           frame_id;
    uint8_t*           buffer;
    size_t             len;
    NCodecCanFrameType frame_type;

    /* Reserved. */
    uint64_t __reserved__[2];

    /* Sender metadata (optional). */
    struct {
        /* RX node identification. */
        uint8_t bus_id;
        uint8_t node_id;
        uint8_t interface_id;
    } sender;

    /* Timing metadata (optional), values in nSec. */
    struct {
        uint64_t send; /* When the message is delivered to the Codec. */
        uint64_t arb;  /* When the message is sent by the Codec. */
        uint64_t recv; /* When the message is received from the Codec. */
    } timing;
} NCodecCanMessage;

#endif  // DSE_NCODEC_INTERFACE_FRAME_H_
