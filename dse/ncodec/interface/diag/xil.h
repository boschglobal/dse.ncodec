// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_INTERFACE_DIAG_XIL_H_
#define DSE_NCODEC_INTERFACE_DIAG_XIL_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/** NCODEC API - Diagnostic/XiL
    ===========================

    Types relating to the implementation of the Diagnostic/XiL (ASAM XiL
    DiagPort) interface of the NCodec API.

    The root type is `NCodecDiagXilMessage` which may be substituted for the
   `NCodecMessage` type when calling NCodec API methods (e.g. `ncodec_write()`).
*/


typedef enum NCodecDiagXilStatus {
    NCodecDiagXilStatusOk = 0,
    NCodecDiagXilStatusFailed = 1,
    NCodecDiagXilStatusTimeout = 2,
    NCodecDiagXilStatusInvalidArgument = 3,
    NCodecDiagXilStatusBusError = 4,
} NCodecDiagXilStatus;

typedef enum NCodecDiagXilDiagnosticValueKind {
    NCodecDiagXilDiagnosticValueKindUnspecified = 0,
    NCodecDiagXilDiagnosticValueKindBoolean = 1,
    NCodecDiagXilDiagnosticValueKindSignedInteger = 2,
    NCodecDiagXilDiagnosticValueKindUnsignedInteger = 3,
    NCodecDiagXilDiagnosticValueKindFloat = 4,
    NCodecDiagXilDiagnosticValueKindString = 5,
    NCodecDiagXilDiagnosticValueKindBytes = 6,
    NCodecDiagXilDiagnosticValueKindVector = 7,
    NCodecDiagXilDiagnosticValueKindMatrix = 8,
    NCodecDiagXilDiagnosticValueKindXy = 9,
    NCodecDiagXilDiagnosticValueKindCurve = 10,
    NCodecDiagXilDiagnosticValueKindMap = 11,
    NCodecDiagXilDiagnosticValueKindSignal = 12,
    NCodecDiagXilDiagnosticValueKindSignalGroup = 13,
} NCodecDiagXilDiagnosticValueKind;

typedef enum NCodecDiagXilDidSelectorKind {
    NCodecDiagXilDidSelectorKindUnspecified = 0,
    NCodecDiagXilDidSelectorKindIdentifier = 1,
    NCodecDiagXilDidSelectorKindName = 2,
} NCodecDiagXilDidSelectorKind;

typedef enum NCodecDiagXilDtcFilterType {
    NCodecDiagXilDtcFilterTypeUnspecified = 0,
    NCodecDiagXilDtcFilterTypeFloating = 1,
    NCodecDiagXilDtcFilterTypePassive = 2,
    NCodecDiagXilDtcFilterTypeActive = 7,
    NCodecDiagXilDtcFilterTypePermanent = 16,
    NCodecDiagXilDtcFilterTypeSupported = 32,
} NCodecDiagXilDtcFilterType;


typedef struct NCodecDiagXilDiagnosticProperty {
    const char* name; /* Required. */
    const char* value;
} NCodecDiagXilDiagnosticProperty;

typedef struct NCodecDiagXilDiagnosticValue {
    const char*                                 name;
    NCodecDiagXilDiagnosticValueKind            kind;
    bool                                        boolean_value;
    int64_t                                     signed_value;
    uint64_t                                    unsigned_value;
    double                                      float_value;
    const char*                                 string_value;
    const uint8_t*                              bytes_value;
    size_t                                      bytes_value_len;
    const struct NCodecDiagXilDiagnosticValue*  elements;
    size_t                                      elements_len;
    uint32_t                                    row_count;
    uint32_t                                    column_count;
    const double*                               x_values;
    size_t                                      x_values_len;
    const double*                               y_values;
    size_t                                      y_values_len;
    const struct NCodecDiagXilDiagnosticSeries* series;
    size_t                                      series_len;
    const char*                                 unit;
    const char*                                 description;
    const NCodecDiagXilDiagnosticProperty*      properties;
    size_t                                      properties_len;
} NCodecDiagXilDiagnosticValue;

typedef struct NCodecDiagXilDiagnosticSeries {
    const char*                         name;
    const NCodecDiagXilDiagnosticValue* values;
    size_t                              values_len;
} NCodecDiagXilDiagnosticSeries;

typedef struct NCodecDiagXilNamedDiagnosticValue {
    const char*                  name; /* Required. */
    NCodecDiagXilDiagnosticValue value;
} NCodecDiagXilNamedDiagnosticValue;

typedef struct NCodecDiagXilDataValue {
    const char*                         path;
    const uint8_t*                      raw_value;
    size_t                              raw_value_len;
    const NCodecDiagXilDiagnosticValue* physical_value; /* Optional. */
} NCodecDiagXilDataValue;

typedef struct NCodecDiagXilDidSelector {
    NCodecDiagXilDidSelectorKind kind;
    uint16_t                     identifier;
    const char*                  name;
} NCodecDiagXilDidSelector;

typedef struct NCodecDiagXilDidIdentity {
    const char* name;
    uint16_t    identifier;
} NCodecDiagXilDidIdentity;

typedef struct NCodecDiagXilDidValue {
    NCodecDiagXilDidSelector        did;
    const uint8_t*                  raw_value;
    size_t                          raw_value_len;
    const NCodecDiagXilDataValue*   values;
    size_t                          values_len;
    const NCodecDiagXilDidIdentity* identity; /* Optional. */
} NCodecDiagXilDidValue;

typedef struct NCodecDiagXilDiagnosticTroubleCode {
    uint64_t                      value;
    uint8_t                       status;
    const char*                   short_name;
    const char*                   long_name;
    const char*                   description;
    const NCodecDiagXilDataValue* environment_data;
    size_t                        environment_data_len;
} NCodecDiagXilDiagnosticTroubleCode;

typedef struct NCodecDiagXilExecuteJobCall {
    const char*                              job_name; /* Required. */
    const NCodecDiagXilNamedDiagnosticValue* parameters;
    size_t                                   parameters_len;
} NCodecDiagXilExecuteJobCall;

typedef struct NCodecDiagXilExecuteJobReturn {
    NCodecDiagXilStatus           status;
    const NCodecDiagXilDataValue* values;
    size_t                        values_len;
    const char*                   info;
} NCodecDiagXilExecuteJobReturn;

typedef struct NCodecDiagXilSendHexServiceCall {
    const uint8_t* pdu; /* Required. */
    size_t         pdu_len;
    bool           response_required;
} NCodecDiagXilSendHexServiceCall;

typedef struct NCodecDiagXilSendHexServiceReturn {
    NCodecDiagXilStatus status;
    const uint8_t*      response_pdu;
    size_t              response_pdu_len;
    const char*         info;
} NCodecDiagXilSendHexServiceReturn;

typedef struct NCodecDiagXilReadFromAddressCall {
    uint64_t address;
    uint64_t number_of_bytes;
} NCodecDiagXilReadFromAddressCall;

typedef struct NCodecDiagXilReadFromAddressReturn {
    NCodecDiagXilStatus status;
    const uint8_t*      data;
    size_t              data_len;
    const char*         info;
} NCodecDiagXilReadFromAddressReturn;

typedef struct NCodecDiagXilWriteToAddressCall {
    uint64_t       address;
    const uint8_t* data; /* Required. */
    size_t         data_len;
} NCodecDiagXilWriteToAddressCall;

typedef struct NCodecDiagXilWriteToAddressReturn {
    NCodecDiagXilStatus status;
    const char*         info;
} NCodecDiagXilWriteToAddressReturn;

typedef struct NCodecDiagXilReadDIDValueCall {
    NCodecDiagXilDidSelector did;
} NCodecDiagXilReadDIDValueCall;

typedef struct NCodecDiagXilReadDIDValueReturn {
    NCodecDiagXilStatus          status;
    uint8_t                      negative_response_code;
    const NCodecDiagXilDidValue* value; /* Optional. */
    const char*                  info;
} NCodecDiagXilReadDIDValueReturn;

typedef struct NCodecDiagXilWriteDIDValueCall {
    NCodecDiagXilDidValue value;
} NCodecDiagXilWriteDIDValueCall;

typedef struct NCodecDiagXilWriteDIDValueReturn {
    NCodecDiagXilStatus status;
    uint8_t             negative_response_code;
    const char*         info;
} NCodecDiagXilWriteDIDValueReturn;

typedef struct NCodecDiagXilGetIdentificationDataReturn {
    NCodecDiagXilStatus             status;
    const NCodecDiagXilDidIdentity* identifiers;
    size_t                          identifiers_len;
    const char*                     info;
} NCodecDiagXilGetIdentificationDataReturn;

typedef struct NCodecDiagXilGetVariantDataReturn {
    NCodecDiagXilStatus          status;
    const NCodecDiagXilDidValue* values;
    size_t                       values_len;
    const char*                  info;
} NCodecDiagXilGetVariantDataReturn;

typedef struct NCodecDiagXilSetVariantDataCall {
    const NCodecDiagXilDidValue* values; /* Required. */
    size_t                       values_len;
} NCodecDiagXilSetVariantDataCall;

typedef struct NCodecDiagXilSetVariantDataReturn {
    NCodecDiagXilStatus status;
    const char*         info;
} NCodecDiagXilSetVariantDataReturn;

typedef struct NCodecDiagXilReadFaultMemoryCall {
    NCodecDiagXilDtcFilterType filter;
} NCodecDiagXilReadFaultMemoryCall;

typedef struct NCodecDiagXilReadFaultMemoryReturn {
    NCodecDiagXilStatus                       status;
    const NCodecDiagXilDiagnosticTroubleCode* trouble_codes;
    size_t                                    trouble_codes_len;
    const char*                               info;
} NCodecDiagXilReadFaultMemoryReturn;

typedef struct NCodecDiagXilClearFaultMemoryReturn {
    NCodecDiagXilStatus status;
    const char*         info;
} NCodecDiagXilClearFaultMemoryReturn;

typedef enum {
    NCodecDiagXilMethodPayloadTypeNone = 0,
    NCodecDiagXilMethodPayloadTypeExecuteJobCall = 1,
    NCodecDiagXilMethodPayloadTypeExecuteJobReturn = 2,
    NCodecDiagXilMethodPayloadTypeSendHexServiceCall = 3,
    NCodecDiagXilMethodPayloadTypeSendHexServiceReturn = 4,
    NCodecDiagXilMethodPayloadTypeReadDIDValueCall = 5,
    NCodecDiagXilMethodPayloadTypeReadDIDValueReturn = 6,
    NCodecDiagXilMethodPayloadTypeWriteDIDValueCall = 7,
    NCodecDiagXilMethodPayloadTypeWriteDIDValueReturn = 8,
    NCodecDiagXilMethodPayloadTypeReadFaultMemoryCall = 9,
    NCodecDiagXilMethodPayloadTypeReadFaultMemoryReturn = 10,
    NCodecDiagXilMethodPayloadTypeClearFaultMemoryCall = 11,
    NCodecDiagXilMethodPayloadTypeClearFaultMemoryReturn = 12,
    NCodecDiagXilMethodPayloadTypeReadFromAddressCall = 13,
    NCodecDiagXilMethodPayloadTypeReadFromAddressReturn = 14,
    NCodecDiagXilMethodPayloadTypeWriteToAddressCall = 15,
    NCodecDiagXilMethodPayloadTypeWriteToAddressReturn = 16,
    NCodecDiagXilMethodPayloadTypeGetIdentificationDataCall = 17,
    NCodecDiagXilMethodPayloadTypeGetIdentificationDataReturn = 18,
    NCodecDiagXilMethodPayloadTypeGetVariantDataCall = 19,
    NCodecDiagXilMethodPayloadTypeGetVariantDataReturn = 20,
    NCodecDiagXilMethodPayloadTypeSetVariantDataCall = 21,
    NCodecDiagXilMethodPayloadTypeSetVariantDataReturn = 22,
} NCodecDiagXilMethodPayloadType;

typedef struct NCodecDiagXilMessage {
    uint32_t call_id;

    /* Method Payload. */
    NCodecDiagXilMethodPayloadType payload_type;
    union {
        struct {
        } none;
        NCodecDiagXilExecuteJobCall              execute_job_call;
        NCodecDiagXilExecuteJobReturn            execute_job_return;
        NCodecDiagXilSendHexServiceCall          send_hex_service_call;
        NCodecDiagXilSendHexServiceReturn        send_hex_service_return;
        NCodecDiagXilReadDIDValueCall            read_did_value_call;
        NCodecDiagXilReadDIDValueReturn          read_did_value_return;
        NCodecDiagXilWriteDIDValueCall           write_did_value_call;
        NCodecDiagXilWriteDIDValueReturn         write_did_value_return;
        NCodecDiagXilReadFaultMemoryCall         read_fault_memory_call;
        NCodecDiagXilReadFaultMemoryReturn       read_fault_memory_return;
        NCodecDiagXilClearFaultMemoryReturn      clear_fault_memory_return;
        NCodecDiagXilReadFromAddressCall         read_from_address_call;
        NCodecDiagXilReadFromAddressReturn       read_from_address_return;
        NCodecDiagXilWriteToAddressCall          write_to_address_call;
        NCodecDiagXilWriteToAddressReturn        write_to_address_return;
        NCodecDiagXilGetIdentificationDataReturn get_identification_data_return;
        NCodecDiagXilGetVariantDataReturn        get_variant_data_return;
        NCodecDiagXilSetVariantDataCall          set_variant_data_call;
        NCodecDiagXilSetVariantDataReturn        set_variant_data_return;
    } payload;

    /* Target identifiers, default value of `ecu_name` (required) is taken
       from the stream MIME Type parameter `name`. */
    const char* ecu_name;
    const char* logical_link_name;
    const char* ecu_information;

    /* Sender identifier (optional), default value is taken from the
       stream MIME Type parameter `node_id`. */
    uint32_t node_uid;

    /* Message Metadata. */
    double   simulation_time;
    uint64_t utc_time;
} NCodecDiagXilMessage;


#endif  // DSE_NCODEC_INTERFACE_DIAG_XIL_H_
