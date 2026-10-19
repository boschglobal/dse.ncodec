// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_INTERFACE_DIAG_UDS_H_
#define DSE_NCODEC_INTERFACE_DIAG_UDS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/** NCODEC API - Diagnostic/UDS
    ===========================

    Types relating to the implementation of the Diagnostic/UDS (ISO 14229)
    interface of the NCodec API.

    The root type is `NCodecDiagUdsMessage` which may be substituted for the
   `NCodecMessage` type when calling NCodec API methods (e.g. `ncodec_write()`).
*/


typedef enum NCodecDiagUdsNegativeResponseCode {
    NCodecDiagUdsNegativeResponseCodeUninitialized = 0,
    NCodecDiagUdsNegativeResponseCodeGeneralReject = 16,
    NCodecDiagUdsNegativeResponseCodeServiceNotSupported = 17,
    NCodecDiagUdsNegativeResponseCodeSubFunctionNotSupported = 18,
    NCodecDiagUdsNegativeResponseCodeIncorrectMessageLengthOrInvalidFormat = 19,
    NCodecDiagUdsNegativeResponseCodeResponseTooLong = 20,
    NCodecDiagUdsNegativeResponseCodeBusyRepeatRequest = 33,
    NCodecDiagUdsNegativeResponseCodeConditionsNotCorrect = 34,
    NCodecDiagUdsNegativeResponseCodeRequestSequenceError = 36,
    NCodecDiagUdsNegativeResponseCodeRequestOutOfRange = 49,
    NCodecDiagUdsNegativeResponseCodeSecurityAccessDenied = 51,
    NCodecDiagUdsNegativeResponseCodeInvalidKey = 53,
    NCodecDiagUdsNegativeResponseCodeExceededNumberOfAttempts = 54,
    NCodecDiagUdsNegativeResponseCodeRequiredTimeDelayNotExpired = 55,
    NCodecDiagUdsNegativeResponseCodeUploadDownloadNotAccepted = 112,
    NCodecDiagUdsNegativeResponseCodeTransferDataSuspended = 113,
    NCodecDiagUdsNegativeResponseCodeGeneralProgrammingFailure = 114,
    NCodecDiagUdsNegativeResponseCodeWrongBlockSequenceCounter = 115,
    NCodecDiagUdsNegativeResponseCodeRequestCorrectlyReceived_ResponsePending =
        120,
} NCodecDiagUdsNegativeResponseCode;

typedef enum NCodecDiagUdsDiagnosticSessionType {
    NCodecDiagUdsDiagnosticSessionTypeUninitialized = 0,
    NCodecDiagUdsDiagnosticSessionTypeDefaultSession = 1,
    NCodecDiagUdsDiagnosticSessionTypeProgrammingSession = 2,
    NCodecDiagUdsDiagnosticSessionTypeExtendedDiagnostic = 3,
    NCodecDiagUdsDiagnosticSessionTypeSafetySystemDiagnostic = 4,
} NCodecDiagUdsDiagnosticSessionType;

typedef enum NCodecDiagUdsResetType {
    NCodecDiagUdsResetTypeUninitialized = 0,
    NCodecDiagUdsResetTypeHardReset = 1,
    NCodecDiagUdsResetTypeKeyOffOnReset = 2,
    NCodecDiagUdsResetTypeSoftReset = 3,
    NCodecDiagUdsResetTypeEnableRapidPowerShutDown = 4,
    NCodecDiagUdsResetTypeDisableRapidPowerShutDown = 5,
} NCodecDiagUdsResetType;

typedef enum NCodecDiagUdsCommunicationControlType {
    NCodecDiagUdsCommunicationControlTypeEnableRxAndTx = 0,
    NCodecDiagUdsCommunicationControlTypeEnableRxAndDisableTx = 1,
    NCodecDiagUdsCommunicationControlTypeDisableRxAndEnableTx = 2,
    NCodecDiagUdsCommunicationControlTypeDisableRxAndTx = 3,
    NCodecDiagUdsCommunicationControlTypeEnableRxAndDisableTxWithEnhancedAddress =
        4,
    NCodecDiagUdsCommunicationControlTypeEnableRxAndTxWithEnhancedAddress = 5,
} NCodecDiagUdsCommunicationControlType;

typedef enum NCodecDiagUdsAddressSizing {
    NCodecDiagUdsAddressSizingUninitialized = 0,
    NCodecDiagUdsAddressSizingTwoByteAddress = 2,
    NCodecDiagUdsAddressSizingThreeByteAddress = 3,
    NCodecDiagUdsAddressSizingFourByteAddress = 4,
    NCodecDiagUdsAddressSizingEightByteAddress = 8,
} NCodecDiagUdsAddressSizing;

typedef enum NCodecDiagUdsLengthSizing {
    NCodecDiagUdsLengthSizingUninitialized = 0,
    NCodecDiagUdsLengthSizingOneByteLength = 1,
    NCodecDiagUdsLengthSizingTwoByteLength = 2,
    NCodecDiagUdsLengthSizingThreeByteLength = 3,
    NCodecDiagUdsLengthSizingFourByteLength = 4,
    NCodecDiagUdsLengthSizingEightByteLength = 8,
} NCodecDiagUdsLengthSizing;

typedef enum NCodecDiagUdsDtcGroupLiteral {
    NCodecDiagUdsDtcGroupLiteralPowertrainDtcGroup = 0,
    NCodecDiagUdsDtcGroupLiteralChassisDtcGroup = 4194304,
    NCodecDiagUdsDtcGroupLiteralBodyDtcGroup = 8388608,
    NCodecDiagUdsDtcGroupLiteralNetworkDtcGroup = 12582912,
    NCodecDiagUdsDtcGroupLiteralAllDtcGroup = 16777215,
} NCodecDiagUdsDtcGroupLiteral;

typedef enum NCodecDiagUdsDtcSubFunction {
    NCodecDiagUdsDtcSubFunctionUninitialized = 0,
    NCodecDiagUdsDtcSubFunctionReportNumberOfDTCByStatusMask = 1,
    NCodecDiagUdsDtcSubFunctionReportDTCByStatusMask = 2,
    NCodecDiagUdsDtcSubFunctionReportDTCSnapshotIdentification = 3,
    NCodecDiagUdsDtcSubFunctionReportDTCSnapshotRecordByDTCNumber = 4,
    NCodecDiagUdsDtcSubFunctionReportDTCStoredDataByRecordNumber = 5,
    NCodecDiagUdsDtcSubFunctionReportDTCExtendedDataRecordByDTCNumber = 6,
    NCodecDiagUdsDtcSubFunctionReportNumberOfDTCBySeverityMaskRecord = 7,
    NCodecDiagUdsDtcSubFunctionReportDTCBySeverityMaskRecord = 8,
    NCodecDiagUdsDtcSubFunctionReportSeverityInformationOfDtc = 9,
    NCodecDiagUdsDtcSubFunctionReportSupportedDtc = 10,
    NCodecDiagUdsDtcSubFunctionReportPermanentStatusDtc = 21,
} NCodecDiagUdsDtcSubFunction;

typedef enum NCodecDiagUdsDtcStatusFlags {
    NCodecDiagUdsDtcStatusFlagsUninitialized = 0,
    NCodecDiagUdsDtcStatusFlagsTestFailed = 1,
    NCodecDiagUdsDtcStatusFlagsTestFailedThisMonitoringCycle = 2,
    NCodecDiagUdsDtcStatusFlagsPendingDtc = 4,
    NCodecDiagUdsDtcStatusFlagsConfirmedDtc = 8,
    NCodecDiagUdsDtcStatusFlagsTestNotCompletedSinceLastClear = 16,
    NCodecDiagUdsDtcStatusFlagsTestFailedSinceLastClear = 32,
    NCodecDiagUdsDtcStatusFlagsTestNotCompletedThisMonitoringCycle = 64,
    NCodecDiagUdsDtcStatusFlagsWarningIndicatorRequested = 128,
} NCodecDiagUdsDtcStatusFlags;

typedef enum NCodecDiagUdsInputOutputControlParameter {
    NCodecDiagUdsInputOutputControlParameterReturnControlToEcu = 0,
    NCodecDiagUdsInputOutputControlParameterResetToDefault = 1,
    NCodecDiagUdsInputOutputControlParameterFreezeCurrentState = 2,
    NCodecDiagUdsInputOutputControlParameterShortTermAdjustment = 3,
} NCodecDiagUdsInputOutputControlParameter;

typedef enum NCodecDiagUdsRoutineControlType {
    NCodecDiagUdsRoutineControlTypeUninitialized = 0,
    NCodecDiagUdsRoutineControlTypeStartRoutine = 1,
    NCodecDiagUdsRoutineControlTypeStopRoutine = 2,
    NCodecDiagUdsRoutineControlTypeRequestRoutineResults = 3,
} NCodecDiagUdsRoutineControlType;

typedef enum NCodecDiagUdsCompressionMethod {
    NCodecDiagUdsCompressionMethodNoCompression = 0,
} NCodecDiagUdsCompressionMethod;

typedef enum NCodecDiagUdsEncryptionMethod {
    NCodecDiagUdsEncryptionMethodNoEncryption = 0,
} NCodecDiagUdsEncryptionMethod;

typedef enum NCodecDiagUdsBlockLengthSizing {
    NCodecDiagUdsBlockLengthSizingUninitialized = 0,
    NCodecDiagUdsBlockLengthSizingOneByteLength = 1,
    NCodecDiagUdsBlockLengthSizingTwoByteLength = 2,
    NCodecDiagUdsBlockLengthSizingThreeByteLength = 3,
    NCodecDiagUdsBlockLengthSizingFourByteLength = 4,
} NCodecDiagUdsBlockLengthSizing;

typedef enum NCodecDiagUdsServiceId {
    NCodecDiagUdsServiceIdUninitialized = 0,
    NCodecDiagUdsServiceIdDiagnosticSessionControlRequest = 16,
    NCodecDiagUdsServiceIdEcuResetRequest = 17,
    NCodecDiagUdsServiceIdClearDiagnosticInformationRequest = 20,
    NCodecDiagUdsServiceIdReadDtcInformationRequest = 25,
    NCodecDiagUdsServiceIdReadDataByIdentifierRequest = 34,
    NCodecDiagUdsServiceIdReadMemoryByAddressRequest = 35,
    NCodecDiagUdsServiceIdSecurityAccessRequest = 39,
    NCodecDiagUdsServiceIdCommunicationControlRequest = 40,
    NCodecDiagUdsServiceIdWriteDataByIdentifierRequest = 46,
    NCodecDiagUdsServiceIdInputOutputControlByIdentifierRequest = 47,
    NCodecDiagUdsServiceIdRoutineControlRequest = 49,
    NCodecDiagUdsServiceIdRequestDownloadRequest = 52,
    NCodecDiagUdsServiceIdRequestUploadRequest = 53,
    NCodecDiagUdsServiceIdTransferDataRequest = 54,
    NCodecDiagUdsServiceIdRequestTransferExitRequest = 55,
    NCodecDiagUdsServiceIdWriteMemoryByAddressRequest = 61,
    NCodecDiagUdsServiceIdTesterPresentRequest = 62,
    NCodecDiagUdsServiceIdDiagnosticSessionControlResponse = 80,
    NCodecDiagUdsServiceIdEcuResetResponse = 81,
    NCodecDiagUdsServiceIdClearDiagnosticInformationResponse = 84,
    NCodecDiagUdsServiceIdReadDtcInformationResponse = 89,
    NCodecDiagUdsServiceIdReadDataByIdentifierResponse = 98,
    NCodecDiagUdsServiceIdReadMemoryByAddressResponse = 99,
    NCodecDiagUdsServiceIdSecurityAccessResponse = 103,
    NCodecDiagUdsServiceIdCommunicationControlResponse = 104,
    NCodecDiagUdsServiceIdWriteDataByIdentifierResponse = 110,
    NCodecDiagUdsServiceIdInputOutputControlByIdentifierResponse = 111,
    NCodecDiagUdsServiceIdRoutineControlResponse = 113,
    NCodecDiagUdsServiceIdRequestDownloadResponse = 116,
    NCodecDiagUdsServiceIdRequestUploadResponse = 117,
    NCodecDiagUdsServiceIdTransferDataResponse = 118,
    NCodecDiagUdsServiceIdRequestTransferExitResponse = 119,
    NCodecDiagUdsServiceIdWriteMemoryByAddressResponse = 125,
    NCodecDiagUdsServiceIdTesterPresentResponse = 126,
    NCodecDiagUdsServiceIdNegativeResponse = 127,
} NCodecDiagUdsServiceId;


typedef struct NCodecDiagUdsAddressLengthFormatIdentifier {
    NCodecDiagUdsAddressSizing address;
    NCodecDiagUdsLengthSizing  size;
} NCodecDiagUdsAddressLengthFormatIdentifier;

typedef struct NCodecDiagUdsDataFormatIdentifier {
    NCodecDiagUdsCompressionMethod compression;
    NCodecDiagUdsEncryptionMethod  encryption;
} NCodecDiagUdsDataFormatIdentifier;

typedef struct NCodecDiagUdsLengthFormatIdentifier {
    NCodecDiagUdsBlockLengthSizing max_block_length_size;
    uint8_t                        reserved;
} NCodecDiagUdsLengthFormatIdentifier;

typedef struct NCodecDiagUdsNegativeResponse {
    uint8_t                           rejected_sid;
    NCodecDiagUdsNegativeResponseCode nrc;
} NCodecDiagUdsNegativeResponse;

typedef struct NCodecDiagUdsDiagnosticSessionControlRequest {
    NCodecDiagUdsDiagnosticSessionType diagnostic_session_type;
} NCodecDiagUdsDiagnosticSessionControlRequest;

typedef struct NCodecDiagUdsDiagnosticSessionControlResponse {
    NCodecDiagUdsDiagnosticSessionType diagnostic_session_type;
    const uint8_t*                     session_parameter_record;
    size_t                             session_parameter_record_len;
} NCodecDiagUdsDiagnosticSessionControlResponse;

typedef struct NCodecDiagUdsEcuResetRequest {
    NCodecDiagUdsResetType reset_type;
} NCodecDiagUdsEcuResetRequest;

typedef struct NCodecDiagUdsEcuResetResponse {
    NCodecDiagUdsResetType reset_type;
    uint8_t                power_down_time_sec;
} NCodecDiagUdsEcuResetResponse;

typedef struct NCodecDiagUdsSecurityAccessRequest {
    uint8_t        security_level;
    const uint8_t* access_data_record;
    size_t         access_data_record_len;
} NCodecDiagUdsSecurityAccessRequest;

typedef struct NCodecDiagUdsSecurityAccessResponse {
    uint8_t        security_level;
    const uint8_t* security_seed_or_key_status;
    size_t         security_seed_or_key_status_len;
} NCodecDiagUdsSecurityAccessResponse;

typedef struct NCodecDiagUdsCommunicationControlRequest {
    NCodecDiagUdsCommunicationControlType control_type;
    uint8_t                               communication_type;
    uint16_t                              node_identification_number;
} NCodecDiagUdsCommunicationControlRequest;

typedef struct NCodecDiagUdsCommunicationControlResponse {
    NCodecDiagUdsCommunicationControlType control_type;
} NCodecDiagUdsCommunicationControlResponse;

typedef struct NCodecDiagUdsTesterPresentRequest {
    uint8_t sub_function;
} NCodecDiagUdsTesterPresentRequest;

typedef struct NCodecDiagUdsTesterPresentResponse {
    uint8_t sub_function;
} NCodecDiagUdsTesterPresentResponse;

typedef struct NCodecDiagUdsReadDataByIdentifierRequest {
    const uint16_t* data_id;
    size_t          data_id_len;
} NCodecDiagUdsReadDataByIdentifierRequest;

typedef struct NCodecDiagUdsReadDataByIdentifierRecord {
    uint16_t       data_id;
    const uint8_t* data_record;
    size_t         data_record_len;
} NCodecDiagUdsReadDataByIdentifierRecord;

typedef struct NCodecDiagUdsReadDataByIdentifierResponse {
    const NCodecDiagUdsReadDataByIdentifierRecord* records;
    size_t                                         records_len;
} NCodecDiagUdsReadDataByIdentifierResponse;

typedef struct NCodecDiagUdsWriteDataByIdentifierRequest {
    uint16_t       data_id;
    const uint8_t* data_record;
    size_t         data_record_len;
} NCodecDiagUdsWriteDataByIdentifierRequest;

typedef struct NCodecDiagUdsWriteDataByIdentifierResponse {
    uint16_t data_id;
} NCodecDiagUdsWriteDataByIdentifierResponse;

typedef struct NCodecDiagUdsReadMemoryByAddressRequest {
    NCodecDiagUdsAddressLengthFormatIdentifier alfid;
    uint64_t                                   memory_address;
    uint64_t                                   memory_size;
} NCodecDiagUdsReadMemoryByAddressRequest;

typedef struct NCodecDiagUdsReadMemoryByAddressResponse {
    const uint8_t* data_record;
    size_t         data_record_len;
} NCodecDiagUdsReadMemoryByAddressResponse;

typedef struct NCodecDiagUdsWriteMemoryByAddressRequest {
    NCodecDiagUdsAddressLengthFormatIdentifier alfid;
    uint64_t                                   memory_address;
    uint64_t                                   memory_size;
    const uint8_t*                             data_record;
    size_t                                     data_record_len;
} NCodecDiagUdsWriteMemoryByAddressRequest;

typedef struct NCodecDiagUdsWriteMemoryByAddressResponse {
    NCodecDiagUdsAddressLengthFormatIdentifier alfid;
    uint64_t                                   memory_address;
    uint64_t                                   memory_size;
} NCodecDiagUdsWriteMemoryByAddressResponse;

typedef struct NCodecDiagUdsClearDiagnosticInformationRequest {
    uint32_t group_of_dtc;
} NCodecDiagUdsClearDiagnosticInformationRequest;

typedef struct NCodecDiagUdsReadDtcInformationRequest {
    NCodecDiagUdsDtcSubFunction sub_function;
    NCodecDiagUdsDtcStatusFlags status_mask;
    uint32_t                    mask_num;
    uint8_t                     snapshot_record_number;
    uint8_t                     severity_mask;
} NCodecDiagUdsReadDtcInformationRequest;

typedef struct NCodecDiagUdsReadDtcInformationResponse {
    NCodecDiagUdsDtcSubFunction sub_function;
    NCodecDiagUdsDtcStatusFlags status_availability_mask;
    const uint8_t*              dtc_and_status_record;
    size_t                      dtc_and_status_record_len;
} NCodecDiagUdsReadDtcInformationResponse;

typedef struct NCodecDiagUdsInputOutputControlByIdentifierRequest {
    uint16_t                                 data_id;
    NCodecDiagUdsInputOutputControlParameter parameter;
    const uint8_t*                           state;
    size_t                                   state_len;
    const uint8_t*                           enable_mask_record;
    size_t                                   enable_mask_record_len;
} NCodecDiagUdsInputOutputControlByIdentifierRequest;

typedef struct NCodecDiagUdsInputOutputControlByIdentifierResponse {
    uint16_t                                 data_id;
    NCodecDiagUdsInputOutputControlParameter parameter;
    const uint8_t*                           status_record;
    size_t                                   status_record_len;
} NCodecDiagUdsInputOutputControlByIdentifierResponse;

typedef struct NCodecDiagUdsRoutineControlRequest {
    NCodecDiagUdsRoutineControlType control_type;
    uint16_t                        id;
    const uint8_t*                  control_option_record;
    size_t                          control_option_record_len;
} NCodecDiagUdsRoutineControlRequest;

typedef struct NCodecDiagUdsRoutineControlResponse {
    NCodecDiagUdsRoutineControlType control_type;
    uint16_t                        id;
    const uint8_t*                  status_record;
    size_t                          status_record_len;
} NCodecDiagUdsRoutineControlResponse;

typedef struct NCodecDiagUdsRequestDownloadRequest {
    NCodecDiagUdsDataFormatIdentifier          dfid;
    NCodecDiagUdsAddressLengthFormatIdentifier alfid;
    uint64_t                                   memory_address;
    uint64_t                                   memory_size;
} NCodecDiagUdsRequestDownloadRequest;

typedef struct NCodecDiagUdsRequestDownloadResponse {
    NCodecDiagUdsLengthFormatIdentifier lfid;
    uint32_t                            max_number_of_block_length;
} NCodecDiagUdsRequestDownloadResponse;

typedef struct NCodecDiagUdsRequestUploadRequest {
    NCodecDiagUdsDataFormatIdentifier          dfid;
    NCodecDiagUdsAddressLengthFormatIdentifier alfid;
    uint64_t                                   memory_address;
    uint64_t                                   memory_size;
} NCodecDiagUdsRequestUploadRequest;

typedef struct NCodecDiagUdsRequestUploadResponse {
    NCodecDiagUdsLengthFormatIdentifier lfid;
    uint32_t                            max_number_of_block_length;
} NCodecDiagUdsRequestUploadResponse;

typedef struct NCodecDiagUdsTransferDataRequest {
    uint8_t        block_sequence_counter;
    const uint8_t* parameter_record;
    size_t         parameter_record_len;
} NCodecDiagUdsTransferDataRequest;

typedef struct NCodecDiagUdsTransferDataResponse {
    uint8_t        block_sequence_counter;
    const uint8_t* parameter_record;
    size_t         parameter_record_len;
} NCodecDiagUdsTransferDataResponse;

typedef struct NCodecDiagUdsRequestTransferExitRequest {
    const uint8_t* parameter_record;
    size_t         parameter_record_len;
} NCodecDiagUdsRequestTransferExitRequest;

typedef struct NCodecDiagUdsRequestTransferExitResponse {
    const uint8_t* parameter_record;
    size_t         parameter_record_len;
} NCodecDiagUdsRequestTransferExitResponse;

typedef enum {
    NCodecDiagUdsServicePrimitiveTypeNone = 0,
    NCodecDiagUdsServicePrimitiveTypeNegativeResponse = 1,
    NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlRequest = 2,
    NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlResponse = 3,
    NCodecDiagUdsServicePrimitiveTypeEcuResetRequest = 4,
    NCodecDiagUdsServicePrimitiveTypeEcuResetResponse = 5,
    NCodecDiagUdsServicePrimitiveTypeSecurityAccessRequest = 6,
    NCodecDiagUdsServicePrimitiveTypeSecurityAccessResponse = 7,
    NCodecDiagUdsServicePrimitiveTypeCommunicationControlRequest = 8,
    NCodecDiagUdsServicePrimitiveTypeCommunicationControlResponse = 9,
    NCodecDiagUdsServicePrimitiveTypeTesterPresentRequest = 10,
    NCodecDiagUdsServicePrimitiveTypeTesterPresentResponse = 11,
    NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierRequest = 12,
    NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierResponse = 13,
    NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierRequest = 14,
    NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierResponse = 15,
    NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressRequest = 16,
    NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressResponse = 17,
    NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressRequest = 18,
    NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressResponse = 19,
    NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationRequest = 20,
    NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationResponse = 21,
    NCodecDiagUdsServicePrimitiveTypeReadDtcInformationRequest = 22,
    NCodecDiagUdsServicePrimitiveTypeReadDtcInformationResponse = 23,
    NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierRequest = 24,
    NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierResponse =
        25,
    NCodecDiagUdsServicePrimitiveTypeRoutineControlRequest = 26,
    NCodecDiagUdsServicePrimitiveTypeRoutineControlResponse = 27,
    NCodecDiagUdsServicePrimitiveTypeRequestDownloadRequest = 28,
    NCodecDiagUdsServicePrimitiveTypeRequestDownloadResponse = 29,
    NCodecDiagUdsServicePrimitiveTypeRequestUploadRequest = 30,
    NCodecDiagUdsServicePrimitiveTypeRequestUploadResponse = 31,
    NCodecDiagUdsServicePrimitiveTypeTransferDataRequest = 32,
    NCodecDiagUdsServicePrimitiveTypeTransferDataResponse = 33,
    NCodecDiagUdsServicePrimitiveTypeRequestTransferExitRequest = 34,
    NCodecDiagUdsServicePrimitiveTypeRequestTransferExitResponse = 35,
} NCodecDiagUdsServicePrimitiveType;

typedef struct NCodecDiagUdsMessage {
    bool                   is_request;
    NCodecDiagUdsServiceId service_id;

    /* Service Primitive. */
    NCodecDiagUdsServicePrimitiveType primitive_type;
    union {
        struct {
        } none;
        NCodecDiagUdsNegativeResponse negative_response;
        NCodecDiagUdsDiagnosticSessionControlRequest
            diagnostic_session_control_request;
        NCodecDiagUdsDiagnosticSessionControlResponse
                                            diagnostic_session_control_response;
        NCodecDiagUdsEcuResetRequest        ecu_reset_request;
        NCodecDiagUdsEcuResetResponse       ecu_reset_response;
        NCodecDiagUdsSecurityAccessRequest  security_access_request;
        NCodecDiagUdsSecurityAccessResponse security_access_response;
        NCodecDiagUdsCommunicationControlRequest communication_control_request;
        NCodecDiagUdsCommunicationControlResponse
                                           communication_control_response;
        NCodecDiagUdsTesterPresentRequest  tester_present_request;
        NCodecDiagUdsTesterPresentResponse tester_present_response;
        NCodecDiagUdsReadDataByIdentifierRequest
            read_data_by_identifier_request;
        NCodecDiagUdsReadDataByIdentifierResponse
            read_data_by_identifier_response;
        NCodecDiagUdsWriteDataByIdentifierRequest
            write_data_by_identifier_request;
        NCodecDiagUdsWriteDataByIdentifierResponse
            write_data_by_identifier_response;
        NCodecDiagUdsReadMemoryByAddressRequest read_memory_by_address_request;
        NCodecDiagUdsReadMemoryByAddressResponse
            read_memory_by_address_response;
        NCodecDiagUdsWriteMemoryByAddressRequest
            write_memory_by_address_request;
        NCodecDiagUdsWriteMemoryByAddressResponse
            write_memory_by_address_response;
        NCodecDiagUdsClearDiagnosticInformationRequest
            clear_diagnostic_information_request;
        NCodecDiagUdsReadDtcInformationRequest  read_dtc_information_request;
        NCodecDiagUdsReadDtcInformationResponse read_dtc_information_response;
        NCodecDiagUdsInputOutputControlByIdentifierRequest
            input_output_control_by_identifier_request;
        NCodecDiagUdsInputOutputControlByIdentifierResponse
            input_output_control_by_identifier_response;
        NCodecDiagUdsRoutineControlRequest       routine_control_request;
        NCodecDiagUdsRoutineControlResponse      routine_control_response;
        NCodecDiagUdsRequestDownloadRequest      request_download_request;
        NCodecDiagUdsRequestDownloadResponse     request_download_response;
        NCodecDiagUdsRequestUploadRequest        request_upload_request;
        NCodecDiagUdsRequestUploadResponse       request_upload_response;
        NCodecDiagUdsTransferDataRequest         transfer_data_request;
        NCodecDiagUdsTransferDataResponse        transfer_data_response;
        NCodecDiagUdsRequestTransferExitRequest  request_transfer_exit_request;
        NCodecDiagUdsRequestTransferExitResponse request_transfer_exit_response;
    } primitive;
} NCodecDiagUdsMessage;


#endif  // DSE_NCODEC_INTERFACE_DIAG_UDS_H_
