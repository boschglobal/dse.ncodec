// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

/* Generated from the public header: one message per union variant
   with every field populated, and deep-compare helpers. */

#include <dse/testing.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <dse/ncodec/interface/diag/uds.h>
#include <tests/cmocka/codec/diag/test_diag.h>


static void assert_address_length_format_identifier_equal(
    const NCodecDiagUdsAddressLengthFormatIdentifier* a,
    const NCodecDiagUdsAddressLengthFormatIdentifier* b);
static void assert_data_format_identifier_equal(
    const NCodecDiagUdsDataFormatIdentifier* a,
    const NCodecDiagUdsDataFormatIdentifier* b);
static void assert_length_format_identifier_equal(
    const NCodecDiagUdsLengthFormatIdentifier* a,
    const NCodecDiagUdsLengthFormatIdentifier* b);
static void assert_negative_response_equal(
    const NCodecDiagUdsNegativeResponse* a,
    const NCodecDiagUdsNegativeResponse* b);
static void assert_diagnostic_session_control_request_equal(
    const NCodecDiagUdsDiagnosticSessionControlRequest* a,
    const NCodecDiagUdsDiagnosticSessionControlRequest* b);
static void assert_diagnostic_session_control_response_equal(
    const NCodecDiagUdsDiagnosticSessionControlResponse* a,
    const NCodecDiagUdsDiagnosticSessionControlResponse* b);
static void assert_ecu_reset_request_equal(
    const NCodecDiagUdsEcuResetRequest* a,
    const NCodecDiagUdsEcuResetRequest* b);
static void assert_ecu_reset_response_equal(
    const NCodecDiagUdsEcuResetResponse* a,
    const NCodecDiagUdsEcuResetResponse* b);
static void assert_security_access_request_equal(
    const NCodecDiagUdsSecurityAccessRequest* a,
    const NCodecDiagUdsSecurityAccessRequest* b);
static void assert_security_access_response_equal(
    const NCodecDiagUdsSecurityAccessResponse* a,
    const NCodecDiagUdsSecurityAccessResponse* b);
static void assert_communication_control_request_equal(
    const NCodecDiagUdsCommunicationControlRequest* a,
    const NCodecDiagUdsCommunicationControlRequest* b);
static void assert_communication_control_response_equal(
    const NCodecDiagUdsCommunicationControlResponse* a,
    const NCodecDiagUdsCommunicationControlResponse* b);
static void assert_tester_present_request_equal(
    const NCodecDiagUdsTesterPresentRequest* a,
    const NCodecDiagUdsTesterPresentRequest* b);
static void assert_tester_present_response_equal(
    const NCodecDiagUdsTesterPresentResponse* a,
    const NCodecDiagUdsTesterPresentResponse* b);
static void assert_read_data_by_identifier_request_equal(
    const NCodecDiagUdsReadDataByIdentifierRequest* a,
    const NCodecDiagUdsReadDataByIdentifierRequest* b);
static void assert_read_data_by_identifier_record_equal(
    const NCodecDiagUdsReadDataByIdentifierRecord* a,
    const NCodecDiagUdsReadDataByIdentifierRecord* b);
static void assert_read_data_by_identifier_response_equal(
    const NCodecDiagUdsReadDataByIdentifierResponse* a,
    const NCodecDiagUdsReadDataByIdentifierResponse* b);
static void assert_write_data_by_identifier_request_equal(
    const NCodecDiagUdsWriteDataByIdentifierRequest* a,
    const NCodecDiagUdsWriteDataByIdentifierRequest* b);
static void assert_write_data_by_identifier_response_equal(
    const NCodecDiagUdsWriteDataByIdentifierResponse* a,
    const NCodecDiagUdsWriteDataByIdentifierResponse* b);
static void assert_read_memory_by_address_request_equal(
    const NCodecDiagUdsReadMemoryByAddressRequest* a,
    const NCodecDiagUdsReadMemoryByAddressRequest* b);
static void assert_read_memory_by_address_response_equal(
    const NCodecDiagUdsReadMemoryByAddressResponse* a,
    const NCodecDiagUdsReadMemoryByAddressResponse* b);
static void assert_write_memory_by_address_request_equal(
    const NCodecDiagUdsWriteMemoryByAddressRequest* a,
    const NCodecDiagUdsWriteMemoryByAddressRequest* b);
static void assert_write_memory_by_address_response_equal(
    const NCodecDiagUdsWriteMemoryByAddressResponse* a,
    const NCodecDiagUdsWriteMemoryByAddressResponse* b);
static void assert_clear_diagnostic_information_request_equal(
    const NCodecDiagUdsClearDiagnosticInformationRequest* a,
    const NCodecDiagUdsClearDiagnosticInformationRequest* b);
static void assert_read_dtc_information_request_equal(
    const NCodecDiagUdsReadDtcInformationRequest* a,
    const NCodecDiagUdsReadDtcInformationRequest* b);
static void assert_read_dtc_information_response_equal(
    const NCodecDiagUdsReadDtcInformationResponse* a,
    const NCodecDiagUdsReadDtcInformationResponse* b);
static void assert_input_output_control_by_identifier_request_equal(
    const NCodecDiagUdsInputOutputControlByIdentifierRequest* a,
    const NCodecDiagUdsInputOutputControlByIdentifierRequest* b);
static void assert_input_output_control_by_identifier_response_equal(
    const NCodecDiagUdsInputOutputControlByIdentifierResponse* a,
    const NCodecDiagUdsInputOutputControlByIdentifierResponse* b);
static void assert_routine_control_request_equal(
    const NCodecDiagUdsRoutineControlRequest* a,
    const NCodecDiagUdsRoutineControlRequest* b);
static void assert_routine_control_response_equal(
    const NCodecDiagUdsRoutineControlResponse* a,
    const NCodecDiagUdsRoutineControlResponse* b);
static void assert_request_download_request_equal(
    const NCodecDiagUdsRequestDownloadRequest* a,
    const NCodecDiagUdsRequestDownloadRequest* b);
static void assert_request_download_response_equal(
    const NCodecDiagUdsRequestDownloadResponse* a,
    const NCodecDiagUdsRequestDownloadResponse* b);
static void assert_request_upload_request_equal(
    const NCodecDiagUdsRequestUploadRequest* a,
    const NCodecDiagUdsRequestUploadRequest* b);
static void assert_request_upload_response_equal(
    const NCodecDiagUdsRequestUploadResponse* a,
    const NCodecDiagUdsRequestUploadResponse* b);
static void assert_transfer_data_request_equal(
    const NCodecDiagUdsTransferDataRequest* a,
    const NCodecDiagUdsTransferDataRequest* b);
static void assert_transfer_data_response_equal(
    const NCodecDiagUdsTransferDataResponse* a,
    const NCodecDiagUdsTransferDataResponse* b);
static void assert_request_transfer_exit_request_equal(
    const NCodecDiagUdsRequestTransferExitRequest* a,
    const NCodecDiagUdsRequestTransferExitRequest* b);
static void assert_request_transfer_exit_response_equal(
    const NCodecDiagUdsRequestTransferExitResponse* a,
    const NCodecDiagUdsRequestTransferExitResponse* b);

static void assert_address_length_format_identifier_equal(
    const NCodecDiagUdsAddressLengthFormatIdentifier* a,
    const NCodecDiagUdsAddressLengthFormatIdentifier* b)
{
    assert_int_equal(a->address, b->address);
    assert_int_equal(a->size, b->size);
}

static void assert_data_format_identifier_equal(
    const NCodecDiagUdsDataFormatIdentifier* a,
    const NCodecDiagUdsDataFormatIdentifier* b)
{
    assert_int_equal(a->compression, b->compression);
    assert_int_equal(a->encryption, b->encryption);
}

static void assert_length_format_identifier_equal(
    const NCodecDiagUdsLengthFormatIdentifier* a,
    const NCodecDiagUdsLengthFormatIdentifier* b)
{
    assert_int_equal(a->max_block_length_size, b->max_block_length_size);
    assert_int_equal(a->reserved, b->reserved);
}

static void assert_negative_response_equal(
    const NCodecDiagUdsNegativeResponse* a,
    const NCodecDiagUdsNegativeResponse* b)
{
    assert_int_equal(a->rejected_sid, b->rejected_sid);
    assert_int_equal(a->nrc, b->nrc);
}

static void assert_diagnostic_session_control_request_equal(
    const NCodecDiagUdsDiagnosticSessionControlRequest* a,
    const NCodecDiagUdsDiagnosticSessionControlRequest* b)
{
    assert_int_equal(a->diagnostic_session_type, b->diagnostic_session_type);
}

static void assert_diagnostic_session_control_response_equal(
    const NCodecDiagUdsDiagnosticSessionControlResponse* a,
    const NCodecDiagUdsDiagnosticSessionControlResponse* b)
{
    assert_int_equal(a->diagnostic_session_type, b->diagnostic_session_type);
    assert_vector_or_null(a->session_parameter_record,
        a->session_parameter_record_len, b->session_parameter_record,
        b->session_parameter_record_len);
}

static void assert_ecu_reset_request_equal(
    const NCodecDiagUdsEcuResetRequest* a,
    const NCodecDiagUdsEcuResetRequest* b)
{
    assert_int_equal(a->reset_type, b->reset_type);
}

static void assert_ecu_reset_response_equal(
    const NCodecDiagUdsEcuResetResponse* a,
    const NCodecDiagUdsEcuResetResponse* b)
{
    assert_int_equal(a->reset_type, b->reset_type);
    assert_int_equal(a->power_down_time_sec, b->power_down_time_sec);
}

static void assert_security_access_request_equal(
    const NCodecDiagUdsSecurityAccessRequest* a,
    const NCodecDiagUdsSecurityAccessRequest* b)
{
    assert_int_equal(a->security_level, b->security_level);
    assert_vector_or_null(a->access_data_record, a->access_data_record_len,
        b->access_data_record, b->access_data_record_len);
}

static void assert_security_access_response_equal(
    const NCodecDiagUdsSecurityAccessResponse* a,
    const NCodecDiagUdsSecurityAccessResponse* b)
{
    assert_int_equal(a->security_level, b->security_level);
    assert_vector_or_null(a->security_seed_or_key_status,
        a->security_seed_or_key_status_len, b->security_seed_or_key_status,
        b->security_seed_or_key_status_len);
}

static void assert_communication_control_request_equal(
    const NCodecDiagUdsCommunicationControlRequest* a,
    const NCodecDiagUdsCommunicationControlRequest* b)
{
    assert_int_equal(a->control_type, b->control_type);
    assert_int_equal(a->communication_type, b->communication_type);
    assert_int_equal(
        a->node_identification_number, b->node_identification_number);
}

static void assert_communication_control_response_equal(
    const NCodecDiagUdsCommunicationControlResponse* a,
    const NCodecDiagUdsCommunicationControlResponse* b)
{
    assert_int_equal(a->control_type, b->control_type);
}

static void assert_tester_present_request_equal(
    const NCodecDiagUdsTesterPresentRequest* a,
    const NCodecDiagUdsTesterPresentRequest* b)
{
    assert_int_equal(a->sub_function, b->sub_function);
}

static void assert_tester_present_response_equal(
    const NCodecDiagUdsTesterPresentResponse* a,
    const NCodecDiagUdsTesterPresentResponse* b)
{
    assert_int_equal(a->sub_function, b->sub_function);
}

static void assert_read_data_by_identifier_request_equal(
    const NCodecDiagUdsReadDataByIdentifierRequest* a,
    const NCodecDiagUdsReadDataByIdentifierRequest* b)
{
    assert_vector_or_null(
        a->data_id, a->data_id_len, b->data_id, b->data_id_len);
}

static void assert_read_data_by_identifier_record_equal(
    const NCodecDiagUdsReadDataByIdentifierRecord* a,
    const NCodecDiagUdsReadDataByIdentifierRecord* b)
{
    assert_int_equal(a->data_id, b->data_id);
    assert_vector_or_null(
        a->data_record, a->data_record_len, b->data_record, b->data_record_len);
}

static void assert_read_data_by_identifier_response_equal(
    const NCodecDiagUdsReadDataByIdentifierResponse* a,
    const NCodecDiagUdsReadDataByIdentifierResponse* b)
{
    assert_true((a->records == NULL) == (b->records == NULL));
    assert_int_equal(a->records_len, b->records_len);
    for (size_t i = 0; i < a->records_len; i++) {
        assert_read_data_by_identifier_record_equal(
            &a->records[i], &b->records[i]);
    }
}

static void assert_write_data_by_identifier_request_equal(
    const NCodecDiagUdsWriteDataByIdentifierRequest* a,
    const NCodecDiagUdsWriteDataByIdentifierRequest* b)
{
    assert_int_equal(a->data_id, b->data_id);
    assert_vector_or_null(
        a->data_record, a->data_record_len, b->data_record, b->data_record_len);
}

static void assert_write_data_by_identifier_response_equal(
    const NCodecDiagUdsWriteDataByIdentifierResponse* a,
    const NCodecDiagUdsWriteDataByIdentifierResponse* b)
{
    assert_int_equal(a->data_id, b->data_id);
}

static void assert_read_memory_by_address_request_equal(
    const NCodecDiagUdsReadMemoryByAddressRequest* a,
    const NCodecDiagUdsReadMemoryByAddressRequest* b)
{
    assert_address_length_format_identifier_equal(&a->alfid, &b->alfid);
    assert_int_equal(a->memory_address, b->memory_address);
    assert_int_equal(a->memory_size, b->memory_size);
}

static void assert_read_memory_by_address_response_equal(
    const NCodecDiagUdsReadMemoryByAddressResponse* a,
    const NCodecDiagUdsReadMemoryByAddressResponse* b)
{
    assert_vector_or_null(
        a->data_record, a->data_record_len, b->data_record, b->data_record_len);
}

static void assert_write_memory_by_address_request_equal(
    const NCodecDiagUdsWriteMemoryByAddressRequest* a,
    const NCodecDiagUdsWriteMemoryByAddressRequest* b)
{
    assert_address_length_format_identifier_equal(&a->alfid, &b->alfid);
    assert_int_equal(a->memory_address, b->memory_address);
    assert_int_equal(a->memory_size, b->memory_size);
    assert_vector_or_null(
        a->data_record, a->data_record_len, b->data_record, b->data_record_len);
}

static void assert_write_memory_by_address_response_equal(
    const NCodecDiagUdsWriteMemoryByAddressResponse* a,
    const NCodecDiagUdsWriteMemoryByAddressResponse* b)
{
    assert_address_length_format_identifier_equal(&a->alfid, &b->alfid);
    assert_int_equal(a->memory_address, b->memory_address);
    assert_int_equal(a->memory_size, b->memory_size);
}

static void assert_clear_diagnostic_information_request_equal(
    const NCodecDiagUdsClearDiagnosticInformationRequest* a,
    const NCodecDiagUdsClearDiagnosticInformationRequest* b)
{
    assert_int_equal(a->group_of_dtc, b->group_of_dtc);
}

static void assert_read_dtc_information_request_equal(
    const NCodecDiagUdsReadDtcInformationRequest* a,
    const NCodecDiagUdsReadDtcInformationRequest* b)
{
    assert_int_equal(a->sub_function, b->sub_function);
    assert_int_equal(a->status_mask, b->status_mask);
    assert_int_equal(a->mask_num, b->mask_num);
    assert_int_equal(a->snapshot_record_number, b->snapshot_record_number);
    assert_int_equal(a->severity_mask, b->severity_mask);
}

static void assert_read_dtc_information_response_equal(
    const NCodecDiagUdsReadDtcInformationResponse* a,
    const NCodecDiagUdsReadDtcInformationResponse* b)
{
    assert_int_equal(a->sub_function, b->sub_function);
    assert_int_equal(a->status_availability_mask, b->status_availability_mask);
    assert_vector_or_null(a->dtc_and_status_record,
        a->dtc_and_status_record_len, b->dtc_and_status_record,
        b->dtc_and_status_record_len);
}

static void assert_input_output_control_by_identifier_request_equal(
    const NCodecDiagUdsInputOutputControlByIdentifierRequest* a,
    const NCodecDiagUdsInputOutputControlByIdentifierRequest* b)
{
    assert_int_equal(a->data_id, b->data_id);
    assert_int_equal(a->parameter, b->parameter);
    assert_vector_or_null(a->state, a->state_len, b->state, b->state_len);
    assert_vector_or_null(a->enable_mask_record, a->enable_mask_record_len,
        b->enable_mask_record, b->enable_mask_record_len);
}

static void assert_input_output_control_by_identifier_response_equal(
    const NCodecDiagUdsInputOutputControlByIdentifierResponse* a,
    const NCodecDiagUdsInputOutputControlByIdentifierResponse* b)
{
    assert_int_equal(a->data_id, b->data_id);
    assert_int_equal(a->parameter, b->parameter);
    assert_vector_or_null(a->status_record, a->status_record_len,
        b->status_record, b->status_record_len);
}

static void assert_routine_control_request_equal(
    const NCodecDiagUdsRoutineControlRequest* a,
    const NCodecDiagUdsRoutineControlRequest* b)
{
    assert_int_equal(a->control_type, b->control_type);
    assert_int_equal(a->id, b->id);
    assert_vector_or_null(a->control_option_record,
        a->control_option_record_len, b->control_option_record,
        b->control_option_record_len);
}

static void assert_routine_control_response_equal(
    const NCodecDiagUdsRoutineControlResponse* a,
    const NCodecDiagUdsRoutineControlResponse* b)
{
    assert_int_equal(a->control_type, b->control_type);
    assert_int_equal(a->id, b->id);
    assert_vector_or_null(a->status_record, a->status_record_len,
        b->status_record, b->status_record_len);
}

static void assert_request_download_request_equal(
    const NCodecDiagUdsRequestDownloadRequest* a,
    const NCodecDiagUdsRequestDownloadRequest* b)
{
    assert_data_format_identifier_equal(&a->dfid, &b->dfid);
    assert_address_length_format_identifier_equal(&a->alfid, &b->alfid);
    assert_int_equal(a->memory_address, b->memory_address);
    assert_int_equal(a->memory_size, b->memory_size);
}

static void assert_request_download_response_equal(
    const NCodecDiagUdsRequestDownloadResponse* a,
    const NCodecDiagUdsRequestDownloadResponse* b)
{
    assert_length_format_identifier_equal(&a->lfid, &b->lfid);
    assert_int_equal(
        a->max_number_of_block_length, b->max_number_of_block_length);
}

static void assert_request_upload_request_equal(
    const NCodecDiagUdsRequestUploadRequest* a,
    const NCodecDiagUdsRequestUploadRequest* b)
{
    assert_data_format_identifier_equal(&a->dfid, &b->dfid);
    assert_address_length_format_identifier_equal(&a->alfid, &b->alfid);
    assert_int_equal(a->memory_address, b->memory_address);
    assert_int_equal(a->memory_size, b->memory_size);
}

static void assert_request_upload_response_equal(
    const NCodecDiagUdsRequestUploadResponse* a,
    const NCodecDiagUdsRequestUploadResponse* b)
{
    assert_length_format_identifier_equal(&a->lfid, &b->lfid);
    assert_int_equal(
        a->max_number_of_block_length, b->max_number_of_block_length);
}

static void assert_transfer_data_request_equal(
    const NCodecDiagUdsTransferDataRequest* a,
    const NCodecDiagUdsTransferDataRequest* b)
{
    assert_int_equal(a->block_sequence_counter, b->block_sequence_counter);
    assert_vector_or_null(a->parameter_record, a->parameter_record_len,
        b->parameter_record, b->parameter_record_len);
}

static void assert_transfer_data_response_equal(
    const NCodecDiagUdsTransferDataResponse* a,
    const NCodecDiagUdsTransferDataResponse* b)
{
    assert_int_equal(a->block_sequence_counter, b->block_sequence_counter);
    assert_vector_or_null(a->parameter_record, a->parameter_record_len,
        b->parameter_record, b->parameter_record_len);
}

static void assert_request_transfer_exit_request_equal(
    const NCodecDiagUdsRequestTransferExitRequest* a,
    const NCodecDiagUdsRequestTransferExitRequest* b)
{
    assert_vector_or_null(a->parameter_record, a->parameter_record_len,
        b->parameter_record, b->parameter_record_len);
}

static void assert_request_transfer_exit_response_equal(
    const NCodecDiagUdsRequestTransferExitResponse* a,
    const NCodecDiagUdsRequestTransferExitResponse* b)
{
    assert_vector_or_null(a->parameter_record, a->parameter_record_len,
        b->parameter_record, b->parameter_record_len);
}


static const uint8_t  _vec_1[] = { 2, 3, 4 };
static const uint8_t  _vec_2[] = { 5, 6, 7 };
static const uint8_t  _vec_3[] = { 7, 8, 9 };
static const uint16_t _vec_4[] = { 524, 525, 526 };
static const uint8_t  _vec_5[] = { 14, 15, 16 };
static const uint8_t  _vec_6[] = { 16, 17, 18 };
static const NCodecDiagUdsReadDataByIdentifierRecord _vec_7[] = {
    {
        .data_id = 269,
        .data_record = _vec_5,
        .data_record_len = ARRAY_SIZE(_vec_5),
    },
    {
        .data_id = 271,
        .data_record = _vec_6,
        .data_record_len = ARRAY_SIZE(_vec_6),
    },
};
static const uint8_t _vec_8[] = { 18, 19, 20 };
static const uint8_t _vec_9[] = { 22, 23, 24 };
static const uint8_t _vec_10[] = { 25, 26, 27 };
static const uint8_t _vec_11[] = { 32, 33, 34 };
static const uint8_t _vec_12[] = { 34, 35, 36 };
static const uint8_t _vec_13[] = { 35, 36, 37 };
static const uint8_t _vec_14[] = { 37, 38, 39 };
static const uint8_t _vec_15[] = { 39, 40, 41 };
static const uint8_t _vec_16[] = { 41, 42, 43 };
static const uint8_t _vec_17[] = { 51, 52, 53 };
static const uint8_t _vec_18[] = { 53, 54, 55 };
static const uint8_t _vec_19[] = { 54, 55, 56 };
static const uint8_t _vec_20[] = { 55, 56, 57 };

const NCodecDiagUdsMessage uds_variants[] = {
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeNegativeResponse,
        .primitive.negative_response = {
            .rejected_sid = 2,
            .nrc = NCodecDiagUdsNegativeResponseCodeRequestCorrectlyReceived_ResponsePending,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlRequest,
        .primitive.diagnostic_session_control_request = {
            .diagnostic_session_type = NCodecDiagUdsDiagnosticSessionTypeSafetySystemDiagnostic,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlResponse,
        .primitive.diagnostic_session_control_response = {
            .diagnostic_session_type = NCodecDiagUdsDiagnosticSessionTypeSafetySystemDiagnostic,
            .session_parameter_record = _vec_1,
            .session_parameter_record_len = ARRAY_SIZE(_vec_1),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeEcuResetRequest,
        .primitive.ecu_reset_request = {
            .reset_type = NCodecDiagUdsResetTypeDisableRapidPowerShutDown,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeEcuResetResponse,
        .primitive.ecu_reset_response = {
            .reset_type = NCodecDiagUdsResetTypeDisableRapidPowerShutDown,
            .power_down_time_sec = 4,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeSecurityAccessRequest,
        .primitive.security_access_request = {
            .security_level = 5,
            .access_data_record = _vec_2,
            .access_data_record_len = ARRAY_SIZE(_vec_2),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeSecurityAccessResponse,
        .primitive.security_access_response = {
            .security_level = 7,
            .security_seed_or_key_status = _vec_3,
            .security_seed_or_key_status_len = ARRAY_SIZE(_vec_3),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeCommunicationControlRequest,
        .primitive.communication_control_request = {
            .control_type = NCodecDiagUdsCommunicationControlTypeEnableRxAndTxWithEnhancedAddress,
            .communication_type = 9,
            .node_identification_number = 265,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeCommunicationControlResponse,
        .primitive.communication_control_response = {
            .control_type = NCodecDiagUdsCommunicationControlTypeEnableRxAndTxWithEnhancedAddress,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeTesterPresentRequest,
        .primitive.tester_present_request = {
            .sub_function = 11,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeTesterPresentResponse,
        .primitive.tester_present_response = {
            .sub_function = 12,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierRequest,
        .primitive.read_data_by_identifier_request = {
            .data_id = _vec_4,
            .data_id_len = ARRAY_SIZE(_vec_4),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierResponse,
        .primitive.read_data_by_identifier_response = {
            .records = _vec_7,
            .records_len = ARRAY_SIZE(_vec_7),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierRequest,
        .primitive.write_data_by_identifier_request = {
            .data_id = 273,
            .data_record = _vec_8,
            .data_record_len = ARRAY_SIZE(_vec_8),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierResponse,
        .primitive.write_data_by_identifier_response = {
            .data_id = 275,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressRequest,
        .primitive.read_memory_by_address_request = {
            .alfid = {
                .address = NCodecDiagUdsAddressSizingEightByteAddress,
                .size = NCodecDiagUdsLengthSizingEightByteLength,
            },
            .memory_address = UINT64_C(4294967316),
            .memory_size = UINT64_C(4294967317),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressResponse,
        .primitive.read_memory_by_address_response = {
            .data_record = _vec_9,
            .data_record_len = ARRAY_SIZE(_vec_9),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressRequest,
        .primitive.write_memory_by_address_request = {
            .alfid = {
                .address = NCodecDiagUdsAddressSizingEightByteAddress,
                .size = NCodecDiagUdsLengthSizingEightByteLength,
            },
            .memory_address = UINT64_C(4294967319),
            .memory_size = UINT64_C(4294967320),
            .data_record = _vec_10,
            .data_record_len = ARRAY_SIZE(_vec_10),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressResponse,
        .primitive.write_memory_by_address_response = {
            .alfid = {
                .address = NCodecDiagUdsAddressSizingEightByteAddress,
                .size = NCodecDiagUdsLengthSizingEightByteLength,
            },
            .memory_address = UINT64_C(4294967322),
            .memory_size = UINT64_C(4294967323),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationRequest,
        .primitive.clear_diagnostic_information_request = {
            .group_of_dtc = 65564,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationResponse,
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeReadDtcInformationRequest,
        .primitive.read_dtc_information_request = {
            .sub_function = NCodecDiagUdsDtcSubFunctionReportPermanentStatusDtc,
            .status_mask = NCodecDiagUdsDtcStatusFlagsWarningIndicatorRequested,
            .mask_num = 65565,
            .snapshot_record_number = 31,
            .severity_mask = 32,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeReadDtcInformationResponse,
        .primitive.read_dtc_information_response = {
            .sub_function = NCodecDiagUdsDtcSubFunctionReportPermanentStatusDtc,
            .status_availability_mask = NCodecDiagUdsDtcStatusFlagsWarningIndicatorRequested,
            .dtc_and_status_record = _vec_11,
            .dtc_and_status_record_len = ARRAY_SIZE(_vec_11),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierRequest,
        .primitive.input_output_control_by_identifier_request = {
            .data_id = 289,
            .parameter = NCodecDiagUdsInputOutputControlParameterShortTermAdjustment,
            .state = _vec_12,
            .state_len = ARRAY_SIZE(_vec_12),
            .enable_mask_record = _vec_13,
            .enable_mask_record_len = ARRAY_SIZE(_vec_13),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierResponse,
        .primitive.input_output_control_by_identifier_response = {
            .data_id = 292,
            .parameter = NCodecDiagUdsInputOutputControlParameterShortTermAdjustment,
            .status_record = _vec_14,
            .status_record_len = ARRAY_SIZE(_vec_14),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRoutineControlRequest,
        .primitive.routine_control_request = {
            .control_type = NCodecDiagUdsRoutineControlTypeRequestRoutineResults,
            .id = 294,
            .control_option_record = _vec_15,
            .control_option_record_len = ARRAY_SIZE(_vec_15),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRoutineControlResponse,
        .primitive.routine_control_response = {
            .control_type = NCodecDiagUdsRoutineControlTypeRequestRoutineResults,
            .id = 296,
            .status_record = _vec_16,
            .status_record_len = ARRAY_SIZE(_vec_16),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRequestDownloadRequest,
        .primitive.request_download_request = {
            .dfid = {
                .compression = NCodecDiagUdsCompressionMethodNoCompression,
                .encryption = NCodecDiagUdsEncryptionMethodNoEncryption,
            },
            .alfid = {
                .address = NCodecDiagUdsAddressSizingEightByteAddress,
                .size = NCodecDiagUdsLengthSizingEightByteLength,
            },
            .memory_address = UINT64_C(4294967338),
            .memory_size = UINT64_C(4294967339),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRequestDownloadResponse,
        .primitive.request_download_response = {
            .lfid = {
                .max_block_length_size = NCodecDiagUdsBlockLengthSizingFourByteLength,
                .reserved = 45,
            },
            .max_number_of_block_length = 65581,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRequestUploadRequest,
        .primitive.request_upload_request = {
            .dfid = {
                .compression = NCodecDiagUdsCompressionMethodNoCompression,
                .encryption = NCodecDiagUdsEncryptionMethodNoEncryption,
            },
            .alfid = {
                .address = NCodecDiagUdsAddressSizingEightByteAddress,
                .size = NCodecDiagUdsLengthSizingEightByteLength,
            },
            .memory_address = UINT64_C(4294967342),
            .memory_size = UINT64_C(4294967343),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRequestUploadResponse,
        .primitive.request_upload_response = {
            .lfid = {
                .max_block_length_size = NCodecDiagUdsBlockLengthSizingFourByteLength,
                .reserved = 49,
            },
            .max_number_of_block_length = 65585,
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeTransferDataRequest,
        .primitive.transfer_data_request = {
            .block_sequence_counter = 51,
            .parameter_record = _vec_17,
            .parameter_record_len = ARRAY_SIZE(_vec_17),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeTransferDataResponse,
        .primitive.transfer_data_response = {
            .block_sequence_counter = 53,
            .parameter_record = _vec_18,
            .parameter_record_len = ARRAY_SIZE(_vec_18),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRequestTransferExitRequest,
        .primitive.request_transfer_exit_request = {
            .parameter_record = _vec_19,
            .parameter_record_len = ARRAY_SIZE(_vec_19),
        },
    },
    {
        .primitive_type = NCodecDiagUdsServicePrimitiveTypeRequestTransferExitResponse,
        .primitive.request_transfer_exit_response = {
            .parameter_record = _vec_20,
            .parameter_record_len = ARRAY_SIZE(_vec_20),
        },
    },
};
const size_t uds_variants_count = ARRAY_SIZE(uds_variants);

void assert_uds_primitive_equal(
    const NCodecDiagUdsMessage* a, const NCodecDiagUdsMessage* b)
{
    assert_int_equal(a->primitive_type, b->primitive_type);
    switch (a->primitive_type) {
    case NCodecDiagUdsServicePrimitiveTypeNegativeResponse:
        assert_negative_response_equal(
            &a->primitive.negative_response, &b->primitive.negative_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlRequest:
        assert_diagnostic_session_control_request_equal(
            &a->primitive.diagnostic_session_control_request,
            &b->primitive.diagnostic_session_control_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlResponse:
        assert_diagnostic_session_control_response_equal(
            &a->primitive.diagnostic_session_control_response,
            &b->primitive.diagnostic_session_control_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeEcuResetRequest:
        assert_ecu_reset_request_equal(
            &a->primitive.ecu_reset_request, &b->primitive.ecu_reset_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeEcuResetResponse:
        assert_ecu_reset_response_equal(
            &a->primitive.ecu_reset_response, &b->primitive.ecu_reset_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeSecurityAccessRequest:
        assert_security_access_request_equal(
            &a->primitive.security_access_request,
            &b->primitive.security_access_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeSecurityAccessResponse:
        assert_security_access_response_equal(
            &a->primitive.security_access_response,
            &b->primitive.security_access_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeCommunicationControlRequest:
        assert_communication_control_request_equal(
            &a->primitive.communication_control_request,
            &b->primitive.communication_control_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeCommunicationControlResponse:
        assert_communication_control_response_equal(
            &a->primitive.communication_control_response,
            &b->primitive.communication_control_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeTesterPresentRequest:
        assert_tester_present_request_equal(
            &a->primitive.tester_present_request,
            &b->primitive.tester_present_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeTesterPresentResponse:
        assert_tester_present_response_equal(
            &a->primitive.tester_present_response,
            &b->primitive.tester_present_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierRequest:
        assert_read_data_by_identifier_request_equal(
            &a->primitive.read_data_by_identifier_request,
            &b->primitive.read_data_by_identifier_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierResponse:
        assert_read_data_by_identifier_response_equal(
            &a->primitive.read_data_by_identifier_response,
            &b->primitive.read_data_by_identifier_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierRequest:
        assert_write_data_by_identifier_request_equal(
            &a->primitive.write_data_by_identifier_request,
            &b->primitive.write_data_by_identifier_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierResponse:
        assert_write_data_by_identifier_response_equal(
            &a->primitive.write_data_by_identifier_response,
            &b->primitive.write_data_by_identifier_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressRequest:
        assert_read_memory_by_address_request_equal(
            &a->primitive.read_memory_by_address_request,
            &b->primitive.read_memory_by_address_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressResponse:
        assert_read_memory_by_address_response_equal(
            &a->primitive.read_memory_by_address_response,
            &b->primitive.read_memory_by_address_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressRequest:
        assert_write_memory_by_address_request_equal(
            &a->primitive.write_memory_by_address_request,
            &b->primitive.write_memory_by_address_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressResponse:
        assert_write_memory_by_address_response_equal(
            &a->primitive.write_memory_by_address_response,
            &b->primitive.write_memory_by_address_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationRequest:
        assert_clear_diagnostic_information_request_equal(
            &a->primitive.clear_diagnostic_information_request,
            &b->primitive.clear_diagnostic_information_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeReadDtcInformationRequest:
        assert_read_dtc_information_request_equal(
            &a->primitive.read_dtc_information_request,
            &b->primitive.read_dtc_information_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeReadDtcInformationResponse:
        assert_read_dtc_information_response_equal(
            &a->primitive.read_dtc_information_response,
            &b->primitive.read_dtc_information_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierRequest:
        assert_input_output_control_by_identifier_request_equal(
            &a->primitive.input_output_control_by_identifier_request,
            &b->primitive.input_output_control_by_identifier_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierResponse:
        assert_input_output_control_by_identifier_response_equal(
            &a->primitive.input_output_control_by_identifier_response,
            &b->primitive.input_output_control_by_identifier_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRoutineControlRequest:
        assert_routine_control_request_equal(
            &a->primitive.routine_control_request,
            &b->primitive.routine_control_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRoutineControlResponse:
        assert_routine_control_response_equal(
            &a->primitive.routine_control_response,
            &b->primitive.routine_control_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRequestDownloadRequest:
        assert_request_download_request_equal(
            &a->primitive.request_download_request,
            &b->primitive.request_download_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRequestDownloadResponse:
        assert_request_download_response_equal(
            &a->primitive.request_download_response,
            &b->primitive.request_download_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRequestUploadRequest:
        assert_request_upload_request_equal(
            &a->primitive.request_upload_request,
            &b->primitive.request_upload_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRequestUploadResponse:
        assert_request_upload_response_equal(
            &a->primitive.request_upload_response,
            &b->primitive.request_upload_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeTransferDataRequest:
        assert_transfer_data_request_equal(&a->primitive.transfer_data_request,
            &b->primitive.transfer_data_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeTransferDataResponse:
        assert_transfer_data_response_equal(
            &a->primitive.transfer_data_response,
            &b->primitive.transfer_data_response);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRequestTransferExitRequest:
        assert_request_transfer_exit_request_equal(
            &a->primitive.request_transfer_exit_request,
            &b->primitive.request_transfer_exit_request);
        break;
    case NCodecDiagUdsServicePrimitiveTypeRequestTransferExitResponse:
        assert_request_transfer_exit_response_equal(
            &a->primitive.request_transfer_exit_response,
            &b->primitive.request_transfer_exit_response);
        break;
    default:
        break;
    }
}
