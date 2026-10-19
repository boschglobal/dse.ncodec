// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_INTERFACE_DIAG_SRPC_H_
#define DSE_NCODEC_INTERFACE_DIAG_SRPC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/** NCODEC API - Diagnostic/Service RPC
    ===================================

    Types relating to the implementation of the Diagnostic/Service RPC
    interface of the NCodec API.

    The root type is `NCodecDiagSrpcMessage` which may be substituted for the
   `NCodecMessage` type when calling NCodec API methods (e.g. `ncodec_write()`).
*/


typedef struct NCodecDiagSrpcMessageContext {
    const uint8_t* request;
    size_t         request_len;
    const uint8_t* response;
    size_t         response_len;

    uint8_t request_type;
    uint8_t request_source;

    bool response_include_request;
    /* Schema default is true, set explicitly (zero value is false). */
    bool response_required;

    uint8_t  context_id;
    uint32_t rx_pdu_id;
} NCodecDiagSrpcMessageContext;

typedef struct NCodecDiagSrpcInvokeCall {
    const char*                  service;  /* Required. */
    const char*                  function; /* Required. */
    uint8_t                      op_status;
    NCodecDiagSrpcMessageContext ctx;
} NCodecDiagSrpcInvokeCall;

typedef struct NCodecDiagSrpcInvokeReturn {
    uint8_t                      return_value;
    uint8_t                      error_code;
    NCodecDiagSrpcMessageContext ctx;
} NCodecDiagSrpcInvokeReturn;

typedef enum {
    NCodecDiagSrpcPayloadTypeNone = 0,
    NCodecDiagSrpcPayloadTypeInvokeCall = 1,
    NCodecDiagSrpcPayloadTypeInvokeReturn = 2,
} NCodecDiagSrpcPayloadType;

typedef struct NCodecDiagSrpcMessage {
    uint32_t call_id;

    /* Payload. */
    NCodecDiagSrpcPayloadType payload_type;
    union {
        struct {
        } none;
        NCodecDiagSrpcInvokeCall   invoke_call;
        NCodecDiagSrpcInvokeReturn invoke_return;
    } payload;

    /* Target identifiers (optional), default values are taken from the
       stream MIME Type parameters (ecu_name from `name`). */
    const char* ecu_name;
    uint32_t    ecu_id;
    uint32_t    swc_id;
    uint32_t    frame_id;

    /* Sender identifier (optional), default value is taken from the
       stream MIME Type parameter `node_id`. */
    uint32_t node_uid;

    /* Message Metadata. */
    double   simulation_time;
    uint64_t utc_time;
} NCodecDiagSrpcMessage;


#endif  // DSE_NCODEC_INTERFACE_DIAG_SRPC_H_
