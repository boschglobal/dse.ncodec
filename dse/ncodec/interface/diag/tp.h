// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_INTERFACE_DIAG_TP_H_
#define DSE_NCODEC_INTERFACE_DIAG_TP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/** NCODEC API - Diagnostic/TP
    ==========================

    Types relating to the implementation of the Diagnostic/TP (Transport
    Protocol) interface of the NCodec API.

    The root type is `NCodecDiagTpMessage` which may be substituted for the
   `NCodecMessage` type when calling NCodec API methods (e.g. `ncodec_write()`).
*/


/** TP : ISO-TP Interface
    ---------------------
*/

typedef enum {
    NCodecDiagIsoTpFlowStatusContinueToSend = 0,
    NCodecDiagIsoTpFlowStatusWait = 1,
    NCodecDiagIsoTpFlowStatusOverflowOrAbort = 2,
} NCodecDiagIsoTpFlowStatus;

typedef struct NCodecDiagIsoTpSingleFrame {
    uint16_t       data_length;
    const uint8_t* payload;
    size_t         payload_len;
} NCodecDiagIsoTpSingleFrame;

typedef struct NCodecDiagIsoTpFirstFrame {
    uint32_t       total_length;
    const uint8_t* payload;
    size_t         payload_len;
} NCodecDiagIsoTpFirstFrame;

typedef struct NCodecDiagIsoTpConsecutiveFrame {
    uint8_t        sequence_number;
    const uint8_t* payload;
    size_t         payload_len;
} NCodecDiagIsoTpConsecutiveFrame;

typedef struct NCodecDiagIsoTpFlowControl {
    NCodecDiagIsoTpFlowStatus status;
    uint8_t                   block_size;
    uint8_t                   st_min_ms;
} NCodecDiagIsoTpFlowControl;

typedef enum {
    NCodecDiagIsoTpFramePayloadTypeNone = 0,
    NCodecDiagIsoTpFramePayloadTypeSingleFrame = 1,
    NCodecDiagIsoTpFramePayloadTypeFirstFrame = 2,
    NCodecDiagIsoTpFramePayloadTypeConsecutiveFrame = 3,
    NCodecDiagIsoTpFramePayloadTypeFlowControl = 4,
} NCodecDiagIsoTpFramePayloadType;

typedef struct NCodecDiagIsoTpFrame {
    NCodecDiagIsoTpFramePayloadType frame_type;
    union {
        struct {
        } none;
        NCodecDiagIsoTpSingleFrame      single_frame;
        NCodecDiagIsoTpFirstFrame       first_frame;
        NCodecDiagIsoTpConsecutiveFrame consecutive_frame;
        NCodecDiagIsoTpFlowControl      flow_control;
    } frame;
    uint64_t timestamp_us;
} NCodecDiagIsoTpFrame;

typedef struct NCodecDiagIsoTpMetadata {
    uint32_t can_id;
    uint32_t total_length;
} NCodecDiagIsoTpMetadata;


/** TP : DoIP Interface
    -------------------
*/

typedef enum {
    NCodecDiagDoIpPayloadTypeGenericHeaderNegativeAcknowledge = 0x0000,
    NCodecDiagDoIpPayloadTypeVehicleIdentificationRequest = 0x0001,
    NCodecDiagDoIpPayloadTypeVehicleIdentificationResponse = 0x0004,
    NCodecDiagDoIpPayloadTypeRoutingActivationRequest = 0x0005,
    NCodecDiagDoIpPayloadTypeRoutingActivationResponse = 0x0006,
    NCodecDiagDoIpPayloadTypeAliveCheckRequest = 0x0007,
    NCodecDiagDoIpPayloadTypeAliveCheckResponse = 0x0008,
    NCodecDiagDoIpPayloadTypeDiagnosticMessage = 0x8001,
    NCodecDiagDoIpPayloadTypeDiagnosticMessageAcknowledge = 0x8002,
    NCodecDiagDoIpPayloadTypeDiagnosticMessageNegativeAck = 0x8003,
} NCodecDiagDoIpPayloadType;

typedef struct NCodecDiagDoIpMetadata {
    uint8_t                   protocol_version;
    NCodecDiagDoIpPayloadType payload_type;
    uint16_t                  source_address;
    uint16_t                  target_address;
} NCodecDiagDoIpMetadata;


/** TP : Diagnostic Message Interface
    ---------------------------------
*/

typedef enum {
    NCodecDiagTpTransportMetadataTypeNone = 0,
    NCodecDiagTpTransportMetadataTypeIsoTp = 1,
    NCodecDiagTpTransportMetadataTypeDoIp = 2,
} NCodecDiagTpTransportMetadataType;

typedef struct NCodecDiagTpMessage {
    /* Transport Metadata. */
    NCodecDiagTpTransportMetadataType transport_meta_type;
    union {
        struct {
        } none;
        NCodecDiagIsoTpMetadata iso_tp;
        NCodecDiagDoIpMetadata  do_ip;
    } transport_meta;

    const uint8_t* payload;
    size_t         payload_len;

    /* ISO-TP frames representing this message. */
    const NCodecDiagIsoTpFrame* frames;
    size_t                      frames_len;

    /* Message Metadata. */
    uint64_t start_timestamp_us;
    uint64_t end_timestamp_us;
} NCodecDiagTpMessage;


#endif  // DSE_NCODEC_INTERFACE_DIAG_TP_H_
