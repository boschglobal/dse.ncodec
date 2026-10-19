// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdint.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/diag/codec.h>
#include <dse/ncodec/interface/diag/uds.h>
#include <dse/ncodec/schema/abs/diagnostic/uds_builder.h>


#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Diagnostic_Uds, x)


extern void*    diag_alloc(DiagCodecInstance* nc, size_t count, size_t size);
extern uint8_t* diag_stream_read(
    DiagCodecInstance* nc, const char* identifier, size_t* length);
extern int32_t diag_stream_write(DiagCodecInstance* nc);


static uint32_t _emit_negative_response(
    flatcc_builder_t* B, const NCodecDiagUdsNegativeResponse* m)
{
    ns(NegativeResponse_start(B));
    ns(NegativeResponse_rejected_sid_add(B, m->rejected_sid));
    ns(NegativeResponse_nrc_add(B, m->nrc));
    return ns(NegativeResponse_end(B));
}

static uint32_t _emit_diagnostic_session_control_request(
    flatcc_builder_t* B, const NCodecDiagUdsDiagnosticSessionControlRequest* m)
{
    ns(DiagnosticSessionControlRequest_start(B));
    ns(DiagnosticSessionControlRequest_diagnostic_session_type_add(
        B, m->diagnostic_session_type));
    return ns(DiagnosticSessionControlRequest_end(B));
}

static uint32_t _emit_diagnostic_session_control_response(
    flatcc_builder_t* B, const NCodecDiagUdsDiagnosticSessionControlResponse* m)
{
    ns(DiagnosticSessionControlResponse_start(B));
    ns(DiagnosticSessionControlResponse_diagnostic_session_type_add(
        B, m->diagnostic_session_type));
    if (m->session_parameter_record) {
        ns(DiagnosticSessionControlResponse_session_parameter_record_add(
            B, flatbuffers_uint8_vec_create(B, m->session_parameter_record,
                   m->session_parameter_record_len)));
    }
    return ns(DiagnosticSessionControlResponse_end(B));
}

static uint32_t _emit_ecu_reset_request(
    flatcc_builder_t* B, const NCodecDiagUdsEcuResetRequest* m)
{
    ns(EcuResetRequest_start(B));
    ns(EcuResetRequest_reset_type_add(B, m->reset_type));
    return ns(EcuResetRequest_end(B));
}

static uint32_t _emit_ecu_reset_response(
    flatcc_builder_t* B, const NCodecDiagUdsEcuResetResponse* m)
{
    ns(EcuResetResponse_start(B));
    ns(EcuResetResponse_reset_type_add(B, m->reset_type));
    ns(EcuResetResponse_power_down_time_sec_add(B, m->power_down_time_sec));
    return ns(EcuResetResponse_end(B));
}

static uint32_t _emit_security_access_request(
    flatcc_builder_t* B, const NCodecDiagUdsSecurityAccessRequest* m)
{
    ns(SecurityAccessRequest_start(B));
    ns(SecurityAccessRequest_security_level_add(B, m->security_level));
    if (m->access_data_record) {
        ns(SecurityAccessRequest_access_data_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->access_data_record, m->access_data_record_len)));
    }
    return ns(SecurityAccessRequest_end(B));
}

static uint32_t _emit_security_access_response(
    flatcc_builder_t* B, const NCodecDiagUdsSecurityAccessResponse* m)
{
    ns(SecurityAccessResponse_start(B));
    ns(SecurityAccessResponse_security_level_add(B, m->security_level));
    if (m->security_seed_or_key_status) {
        ns(SecurityAccessResponse_security_seed_or_key_status_add(
            B, flatbuffers_uint8_vec_create(B, m->security_seed_or_key_status,
                   m->security_seed_or_key_status_len)));
    }
    return ns(SecurityAccessResponse_end(B));
}

static uint32_t _emit_communication_control_request(
    flatcc_builder_t* B, const NCodecDiagUdsCommunicationControlRequest* m)
{
    ns(CommunicationControlRequest_start(B));
    ns(CommunicationControlRequest_control_type_add(B, m->control_type));
    ns(CommunicationControlRequest_communication_type_add(
        B, m->communication_type));
    ns(CommunicationControlRequest_node_identification_number_add(
        B, m->node_identification_number));
    return ns(CommunicationControlRequest_end(B));
}

static uint32_t _emit_communication_control_response(
    flatcc_builder_t* B, const NCodecDiagUdsCommunicationControlResponse* m)
{
    ns(CommunicationControlResponse_start(B));
    ns(CommunicationControlResponse_control_type_add(B, m->control_type));
    return ns(CommunicationControlResponse_end(B));
}

static uint32_t _emit_tester_present_request(
    flatcc_builder_t* B, const NCodecDiagUdsTesterPresentRequest* m)
{
    ns(TesterPresentRequest_start(B));
    ns(TesterPresentRequest_sub_function_add(B, m->sub_function));
    return ns(TesterPresentRequest_end(B));
}

static uint32_t _emit_tester_present_response(
    flatcc_builder_t* B, const NCodecDiagUdsTesterPresentResponse* m)
{
    ns(TesterPresentResponse_start(B));
    ns(TesterPresentResponse_sub_function_add(B, m->sub_function));
    return ns(TesterPresentResponse_end(B));
}

static uint32_t _emit_read_data_by_identifier_request(
    flatcc_builder_t* B, const NCodecDiagUdsReadDataByIdentifierRequest* m)
{
    ns(ReadDataByIdentifierRequest_start(B));
    if (m->data_id) {
        ns(ReadDataByIdentifierRequest_data_id_add(
            B, flatbuffers_uint16_vec_create(B, m->data_id, m->data_id_len)));
    }
    return ns(ReadDataByIdentifierRequest_end(B));
}

static uint32_t _emit_read_data_by_identifier_record(
    flatcc_builder_t* B, const NCodecDiagUdsReadDataByIdentifierRecord* m)
{
    ns(ReadDataByIdentifierRecord_start(B));
    ns(ReadDataByIdentifierRecord_data_id_add(B, m->data_id));
    if (m->data_record) {
        ns(ReadDataByIdentifierRecord_data_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->data_record, m->data_record_len)));
    }
    return ns(ReadDataByIdentifierRecord_end(B));
}

static uint32_t _emit_read_data_by_identifier_response(
    flatcc_builder_t* B, const NCodecDiagUdsReadDataByIdentifierResponse* m)
{
    uint32_t records = 0;
    if (m->records) {
        ns(ReadDataByIdentifierRecord_vec_start(B));
        for (size_t i = 0; i < m->records_len; i++) {
            ns(ReadDataByIdentifierRecord_vec_push(
                B, _emit_read_data_by_identifier_record(B, &m->records[i])));
        }
        records = ns(ReadDataByIdentifierRecord_vec_end(B));
    }

    ns(ReadDataByIdentifierResponse_start(B));
    if (records) ns(ReadDataByIdentifierResponse_records_add(B, records));
    return ns(ReadDataByIdentifierResponse_end(B));
}

static uint32_t _emit_write_data_by_identifier_request(
    flatcc_builder_t* B, const NCodecDiagUdsWriteDataByIdentifierRequest* m)
{
    ns(WriteDataByIdentifierRequest_start(B));
    ns(WriteDataByIdentifierRequest_data_id_add(B, m->data_id));
    if (m->data_record) {
        ns(WriteDataByIdentifierRequest_data_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->data_record, m->data_record_len)));
    }
    return ns(WriteDataByIdentifierRequest_end(B));
}

static uint32_t _emit_write_data_by_identifier_response(
    flatcc_builder_t* B, const NCodecDiagUdsWriteDataByIdentifierResponse* m)
{
    ns(WriteDataByIdentifierResponse_start(B));
    ns(WriteDataByIdentifierResponse_data_id_add(B, m->data_id));
    return ns(WriteDataByIdentifierResponse_end(B));
}

static uint32_t _emit_read_memory_by_address_request(
    flatcc_builder_t* B, const NCodecDiagUdsReadMemoryByAddressRequest* m)
{
    ns(ReadMemoryByAddressRequest_start(B));
    ns(ReadMemoryByAddressRequest_alfid_create(
        B, m->alfid.address, m->alfid.size));
    ns(ReadMemoryByAddressRequest_memory_address_add(B, m->memory_address));
    ns(ReadMemoryByAddressRequest_memory_size_add(B, m->memory_size));
    return ns(ReadMemoryByAddressRequest_end(B));
}

static uint32_t _emit_read_memory_by_address_response(
    flatcc_builder_t* B, const NCodecDiagUdsReadMemoryByAddressResponse* m)
{
    ns(ReadMemoryByAddressResponse_start(B));
    if (m->data_record) {
        ns(ReadMemoryByAddressResponse_data_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->data_record, m->data_record_len)));
    }
    return ns(ReadMemoryByAddressResponse_end(B));
}

static uint32_t _emit_write_memory_by_address_request(
    flatcc_builder_t* B, const NCodecDiagUdsWriteMemoryByAddressRequest* m)
{
    ns(WriteMemoryByAddressRequest_start(B));
    ns(WriteMemoryByAddressRequest_alfid_create(
        B, m->alfid.address, m->alfid.size));
    ns(WriteMemoryByAddressRequest_memory_address_add(B, m->memory_address));
    ns(WriteMemoryByAddressRequest_memory_size_add(B, m->memory_size));
    if (m->data_record) {
        ns(WriteMemoryByAddressRequest_data_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->data_record, m->data_record_len)));
    }
    return ns(WriteMemoryByAddressRequest_end(B));
}

static uint32_t _emit_write_memory_by_address_response(
    flatcc_builder_t* B, const NCodecDiagUdsWriteMemoryByAddressResponse* m)
{
    ns(WriteMemoryByAddressResponse_start(B));
    ns(WriteMemoryByAddressResponse_alfid_create(
        B, m->alfid.address, m->alfid.size));
    ns(WriteMemoryByAddressResponse_memory_address_add(B, m->memory_address));
    ns(WriteMemoryByAddressResponse_memory_size_add(B, m->memory_size));
    return ns(WriteMemoryByAddressResponse_end(B));
}

static uint32_t _emit_clear_diagnostic_information_request(flatcc_builder_t* B,
    const NCodecDiagUdsClearDiagnosticInformationRequest*                    m)
{
    ns(ClearDiagnosticInformationRequest_start(B));
    ns(ClearDiagnosticInformationRequest_group_of_dtc_add(B, m->group_of_dtc));
    return ns(ClearDiagnosticInformationRequest_end(B));
}

static uint32_t _emit_read_dtc_information_request(
    flatcc_builder_t* B, const NCodecDiagUdsReadDtcInformationRequest* m)
{
    ns(ReadDtcInformationRequest_start(B));
    ns(ReadDtcInformationRequest_sub_function_add(B, m->sub_function));
    ns(ReadDtcInformationRequest_status_mask_add(B, m->status_mask));
    ns(ReadDtcInformationRequest_mask_num_add(B, m->mask_num));
    ns(ReadDtcInformationRequest_snapshot_record_number_add(
        B, m->snapshot_record_number));
    ns(ReadDtcInformationRequest_severity_mask_add(B, m->severity_mask));
    return ns(ReadDtcInformationRequest_end(B));
}

static uint32_t _emit_read_dtc_information_response(
    flatcc_builder_t* B, const NCodecDiagUdsReadDtcInformationResponse* m)
{
    ns(ReadDtcInformationResponse_start(B));
    ns(ReadDtcInformationResponse_sub_function_add(B, m->sub_function));
    ns(ReadDtcInformationResponse_status_availability_mask_add(
        B, m->status_availability_mask));
    if (m->dtc_and_status_record) {
        ns(ReadDtcInformationResponse_dtc_and_status_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->dtc_and_status_record, m->dtc_and_status_record_len)));
    }
    return ns(ReadDtcInformationResponse_end(B));
}

static uint32_t _emit_input_output_control_by_identifier_request(
    flatcc_builder_t*                                         B,
    const NCodecDiagUdsInputOutputControlByIdentifierRequest* m)
{
    ns(InputOutputControlByIdentifierRequest_start(B));
    ns(InputOutputControlByIdentifierRequest_data_id_add(B, m->data_id));
    ns(InputOutputControlByIdentifierRequest_parameter_add(B, m->parameter));
    if (m->state) {
        ns(InputOutputControlByIdentifierRequest_state_add(
            B, flatbuffers_uint8_vec_create(B, m->state, m->state_len)));
    }
    if (m->enable_mask_record) {
        ns(InputOutputControlByIdentifierRequest_enable_mask_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->enable_mask_record, m->enable_mask_record_len)));
    }
    return ns(InputOutputControlByIdentifierRequest_end(B));
}

static uint32_t _emit_input_output_control_by_identifier_response(
    flatcc_builder_t*                                          B,
    const NCodecDiagUdsInputOutputControlByIdentifierResponse* m)
{
    ns(InputOutputControlByIdentifierResponse_start(B));
    ns(InputOutputControlByIdentifierResponse_data_id_add(B, m->data_id));
    ns(InputOutputControlByIdentifierResponse_parameter_add(B, m->parameter));
    if (m->status_record) {
        ns(InputOutputControlByIdentifierResponse_status_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->status_record, m->status_record_len)));
    }
    return ns(InputOutputControlByIdentifierResponse_end(B));
}

static uint32_t _emit_routine_control_request(
    flatcc_builder_t* B, const NCodecDiagUdsRoutineControlRequest* m)
{
    ns(RoutineControlRequest_start(B));
    ns(RoutineControlRequest_control_type_add(B, m->control_type));
    ns(RoutineControlRequest_id_add(B, m->id));
    if (m->control_option_record) {
        ns(RoutineControlRequest_control_option_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->control_option_record, m->control_option_record_len)));
    }
    return ns(RoutineControlRequest_end(B));
}

static uint32_t _emit_routine_control_response(
    flatcc_builder_t* B, const NCodecDiagUdsRoutineControlResponse* m)
{
    ns(RoutineControlResponse_start(B));
    ns(RoutineControlResponse_control_type_add(B, m->control_type));
    ns(RoutineControlResponse_id_add(B, m->id));
    if (m->status_record) {
        ns(RoutineControlResponse_status_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->status_record, m->status_record_len)));
    }
    return ns(RoutineControlResponse_end(B));
}

static uint32_t _emit_request_download_request(
    flatcc_builder_t* B, const NCodecDiagUdsRequestDownloadRequest* m)
{
    ns(RequestDownloadRequest_start(B));
    ns(RequestDownloadRequest_dfid_create(
        B, m->dfid.compression, m->dfid.encryption));
    ns(RequestDownloadRequest_alfid_create(B, m->alfid.address, m->alfid.size));
    ns(RequestDownloadRequest_memory_address_add(B, m->memory_address));
    ns(RequestDownloadRequest_memory_size_add(B, m->memory_size));
    return ns(RequestDownloadRequest_end(B));
}

static uint32_t _emit_request_download_response(
    flatcc_builder_t* B, const NCodecDiagUdsRequestDownloadResponse* m)
{
    ns(RequestDownloadResponse_start(B));
    ns(RequestDownloadResponse_lfid_create(
        B, m->lfid.max_block_length_size, m->lfid.reserved));
    ns(RequestDownloadResponse_max_number_of_block_length_add(
        B, m->max_number_of_block_length));
    return ns(RequestDownloadResponse_end(B));
}

static uint32_t _emit_request_upload_request(
    flatcc_builder_t* B, const NCodecDiagUdsRequestUploadRequest* m)
{
    ns(RequestUploadRequest_start(B));
    ns(RequestUploadRequest_dfid_create(
        B, m->dfid.compression, m->dfid.encryption));
    ns(RequestUploadRequest_alfid_create(B, m->alfid.address, m->alfid.size));
    ns(RequestUploadRequest_memory_address_add(B, m->memory_address));
    ns(RequestUploadRequest_memory_size_add(B, m->memory_size));
    return ns(RequestUploadRequest_end(B));
}

static uint32_t _emit_request_upload_response(
    flatcc_builder_t* B, const NCodecDiagUdsRequestUploadResponse* m)
{
    ns(RequestUploadResponse_start(B));
    ns(RequestUploadResponse_lfid_create(
        B, m->lfid.max_block_length_size, m->lfid.reserved));
    ns(RequestUploadResponse_max_number_of_block_length_add(
        B, m->max_number_of_block_length));
    return ns(RequestUploadResponse_end(B));
}

static uint32_t _emit_transfer_data_request(
    flatcc_builder_t* B, const NCodecDiagUdsTransferDataRequest* m)
{
    ns(TransferDataRequest_start(B));
    ns(TransferDataRequest_block_sequence_counter_add(
        B, m->block_sequence_counter));
    if (m->parameter_record) {
        ns(TransferDataRequest_parameter_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->parameter_record, m->parameter_record_len)));
    }
    return ns(TransferDataRequest_end(B));
}

static uint32_t _emit_transfer_data_response(
    flatcc_builder_t* B, const NCodecDiagUdsTransferDataResponse* m)
{
    ns(TransferDataResponse_start(B));
    ns(TransferDataResponse_block_sequence_counter_add(
        B, m->block_sequence_counter));
    if (m->parameter_record) {
        ns(TransferDataResponse_parameter_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->parameter_record, m->parameter_record_len)));
    }
    return ns(TransferDataResponse_end(B));
}

static uint32_t _emit_request_transfer_exit_request(
    flatcc_builder_t* B, const NCodecDiagUdsRequestTransferExitRequest* m)
{
    ns(RequestTransferExitRequest_start(B));
    if (m->parameter_record) {
        ns(RequestTransferExitRequest_parameter_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->parameter_record, m->parameter_record_len)));
    }
    return ns(RequestTransferExitRequest_end(B));
}

static uint32_t _emit_request_transfer_exit_response(
    flatcc_builder_t* B, const NCodecDiagUdsRequestTransferExitResponse* m)
{
    ns(RequestTransferExitResponse_start(B));
    if (m->parameter_record) {
        ns(RequestTransferExitResponse_parameter_record_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->parameter_record, m->parameter_record_len)));
    }
    return ns(RequestTransferExitResponse_end(B));
}


static void _decode_negative_response(
    ns(NegativeResponse_table_t) t, NCodecDiagUdsNegativeResponse* m)
{
    m->rejected_sid = ns(NegativeResponse_rejected_sid(t));
    m->nrc = (NCodecDiagUdsNegativeResponseCode)ns(NegativeResponse_nrc(t));
}

static void _decode_diagnostic_session_control_request(
    ns(DiagnosticSessionControlRequest_table_t) t,
    NCodecDiagUdsDiagnosticSessionControlRequest* m)
{
    m->diagnostic_session_type = (NCodecDiagUdsDiagnosticSessionType)ns(
        DiagnosticSessionControlRequest_diagnostic_session_type(t));
}

static void _decode_diagnostic_session_control_response(
    ns(DiagnosticSessionControlResponse_table_t) t,
    NCodecDiagUdsDiagnosticSessionControlResponse* m)
{
    m->diagnostic_session_type = (NCodecDiagUdsDiagnosticSessionType)ns(
        DiagnosticSessionControlResponse_diagnostic_session_type(t));
    m->session_parameter_record =
        ns(DiagnosticSessionControlResponse_session_parameter_record(t));
    m->session_parameter_record_len =
        flatbuffers_uint8_vec_len(m->session_parameter_record);
}

static void _decode_ecu_reset_request(
    ns(EcuResetRequest_table_t) t, NCodecDiagUdsEcuResetRequest* m)
{
    m->reset_type = (NCodecDiagUdsResetType)ns(EcuResetRequest_reset_type(t));
}

static void _decode_ecu_reset_response(
    ns(EcuResetResponse_table_t) t, NCodecDiagUdsEcuResetResponse* m)
{
    m->reset_type = (NCodecDiagUdsResetType)ns(EcuResetResponse_reset_type(t));
    m->power_down_time_sec = ns(EcuResetResponse_power_down_time_sec(t));
}

static void _decode_security_access_request(
    ns(SecurityAccessRequest_table_t) t, NCodecDiagUdsSecurityAccessRequest* m)
{
    m->security_level = ns(SecurityAccessRequest_security_level(t));
    m->access_data_record = ns(SecurityAccessRequest_access_data_record(t));
    m->access_data_record_len =
        flatbuffers_uint8_vec_len(m->access_data_record);
}

static void _decode_security_access_response(ns(SecurityAccessResponse_table_t)
                                                 t,
    NCodecDiagUdsSecurityAccessResponse* m)
{
    m->security_level = ns(SecurityAccessResponse_security_level(t));
    m->security_seed_or_key_status =
        ns(SecurityAccessResponse_security_seed_or_key_status(t));
    m->security_seed_or_key_status_len =
        flatbuffers_uint8_vec_len(m->security_seed_or_key_status);
}

static void _decode_communication_control_request(
    ns(CommunicationControlRequest_table_t) t,
    NCodecDiagUdsCommunicationControlRequest* m)
{
    m->control_type = (NCodecDiagUdsCommunicationControlType)ns(
        CommunicationControlRequest_control_type(t));
    m->communication_type =
        ns(CommunicationControlRequest_communication_type(t));
    m->node_identification_number =
        ns(CommunicationControlRequest_node_identification_number(t));
}

static void _decode_communication_control_response(
    ns(CommunicationControlResponse_table_t) t,
    NCodecDiagUdsCommunicationControlResponse* m)
{
    m->control_type = (NCodecDiagUdsCommunicationControlType)ns(
        CommunicationControlResponse_control_type(t));
}

static void _decode_tester_present_request(
    ns(TesterPresentRequest_table_t) t, NCodecDiagUdsTesterPresentRequest* m)
{
    m->sub_function = ns(TesterPresentRequest_sub_function(t));
}

static void _decode_tester_present_response(
    ns(TesterPresentResponse_table_t) t, NCodecDiagUdsTesterPresentResponse* m)
{
    m->sub_function = ns(TesterPresentResponse_sub_function(t));
}

static void _decode_read_data_by_identifier_request(DiagCodecInstance* nc,
    ns(ReadDataByIdentifierRequest_table_t) t,
    NCodecDiagUdsReadDataByIdentifierRequest* m)
{
    {
        flatbuffers_uint16_vec_t v = ns(ReadDataByIdentifierRequest_data_id(t));
        if (v) {
            size_t    len = flatbuffers_uint16_vec_len(v);
            uint16_t* data = diag_alloc(nc, len, sizeof(uint16_t));
            for (size_t i = 0; i < len; i++) {
                data[i] = flatbuffers_uint16_vec_at(v, i);
            }
            m->data_id = data;
            m->data_id_len = len;
        }
    }
}

static void _decode_read_data_by_identifier_record(
    ns(ReadDataByIdentifierRecord_table_t) t,
    NCodecDiagUdsReadDataByIdentifierRecord* m)
{
    m->data_id = ns(ReadDataByIdentifierRecord_data_id(t));
    m->data_record = ns(ReadDataByIdentifierRecord_data_record(t));
    m->data_record_len = flatbuffers_uint8_vec_len(m->data_record);
}

static void _decode_read_data_by_identifier_response(DiagCodecInstance* nc,
    ns(ReadDataByIdentifierResponse_table_t) t,
    NCodecDiagUdsReadDataByIdentifierResponse* m)
{
    {
        ns(ReadDataByIdentifierRecord_vec_t) v =
            ns(ReadDataByIdentifierResponse_records(t));
        if (v) {
            size_t len = ns(ReadDataByIdentifierRecord_vec_len(v));
            NCodecDiagUdsReadDataByIdentifierRecord* data = diag_alloc(
                nc, len, sizeof(NCodecDiagUdsReadDataByIdentifierRecord));
            for (size_t i = 0; i < len; i++) {
                _decode_read_data_by_identifier_record(
                    ns(ReadDataByIdentifierRecord_vec_at(v, i)), &data[i]);
            }
            m->records = data;
            m->records_len = len;
        }
    }
}

static void _decode_write_data_by_identifier_request(
    ns(WriteDataByIdentifierRequest_table_t) t,
    NCodecDiagUdsWriteDataByIdentifierRequest* m)
{
    m->data_id = ns(WriteDataByIdentifierRequest_data_id(t));
    m->data_record = ns(WriteDataByIdentifierRequest_data_record(t));
    m->data_record_len = flatbuffers_uint8_vec_len(m->data_record);
}

static void _decode_write_data_by_identifier_response(
    ns(WriteDataByIdentifierResponse_table_t) t,
    NCodecDiagUdsWriteDataByIdentifierResponse* m)
{
    m->data_id = ns(WriteDataByIdentifierResponse_data_id(t));
}

static void _decode_read_memory_by_address_request(
    ns(ReadMemoryByAddressRequest_table_t) t,
    NCodecDiagUdsReadMemoryByAddressRequest* m)
{
    {
        ns(AddressLengthFormatIdentifier_struct_t) s =
            ns(ReadMemoryByAddressRequest_alfid(t));
        m->alfid.address = (NCodecDiagUdsAddressSizing)ns(
            AddressLengthFormatIdentifier_address(s));
        m->alfid.size = (NCodecDiagUdsLengthSizing)ns(
            AddressLengthFormatIdentifier_size(s));
    }
    m->memory_address = ns(ReadMemoryByAddressRequest_memory_address(t));
    m->memory_size = ns(ReadMemoryByAddressRequest_memory_size(t));
}

static void _decode_read_memory_by_address_response(
    ns(ReadMemoryByAddressResponse_table_t) t,
    NCodecDiagUdsReadMemoryByAddressResponse* m)
{
    m->data_record = ns(ReadMemoryByAddressResponse_data_record(t));
    m->data_record_len = flatbuffers_uint8_vec_len(m->data_record);
}

static void _decode_write_memory_by_address_request(
    ns(WriteMemoryByAddressRequest_table_t) t,
    NCodecDiagUdsWriteMemoryByAddressRequest* m)
{
    {
        ns(AddressLengthFormatIdentifier_struct_t) s =
            ns(WriteMemoryByAddressRequest_alfid(t));
        m->alfid.address = (NCodecDiagUdsAddressSizing)ns(
            AddressLengthFormatIdentifier_address(s));
        m->alfid.size = (NCodecDiagUdsLengthSizing)ns(
            AddressLengthFormatIdentifier_size(s));
    }
    m->memory_address = ns(WriteMemoryByAddressRequest_memory_address(t));
    m->memory_size = ns(WriteMemoryByAddressRequest_memory_size(t));
    m->data_record = ns(WriteMemoryByAddressRequest_data_record(t));
    m->data_record_len = flatbuffers_uint8_vec_len(m->data_record);
}

static void _decode_write_memory_by_address_response(
    ns(WriteMemoryByAddressResponse_table_t) t,
    NCodecDiagUdsWriteMemoryByAddressResponse* m)
{
    {
        ns(AddressLengthFormatIdentifier_struct_t) s =
            ns(WriteMemoryByAddressResponse_alfid(t));
        m->alfid.address = (NCodecDiagUdsAddressSizing)ns(
            AddressLengthFormatIdentifier_address(s));
        m->alfid.size = (NCodecDiagUdsLengthSizing)ns(
            AddressLengthFormatIdentifier_size(s));
    }
    m->memory_address = ns(WriteMemoryByAddressResponse_memory_address(t));
    m->memory_size = ns(WriteMemoryByAddressResponse_memory_size(t));
}

static void _decode_clear_diagnostic_information_request(
    ns(ClearDiagnosticInformationRequest_table_t) t,
    NCodecDiagUdsClearDiagnosticInformationRequest* m)
{
    m->group_of_dtc = ns(ClearDiagnosticInformationRequest_group_of_dtc(t));
}

static void _decode_read_dtc_information_request(
    ns(ReadDtcInformationRequest_table_t) t,
    NCodecDiagUdsReadDtcInformationRequest* m)
{
    m->sub_function = (NCodecDiagUdsDtcSubFunction)ns(
        ReadDtcInformationRequest_sub_function(t));
    m->status_mask = (NCodecDiagUdsDtcStatusFlags)ns(
        ReadDtcInformationRequest_status_mask(t));
    m->mask_num = ns(ReadDtcInformationRequest_mask_num(t));
    m->snapshot_record_number =
        ns(ReadDtcInformationRequest_snapshot_record_number(t));
    m->severity_mask = ns(ReadDtcInformationRequest_severity_mask(t));
}

static void _decode_read_dtc_information_response(
    ns(ReadDtcInformationResponse_table_t) t,
    NCodecDiagUdsReadDtcInformationResponse* m)
{
    m->sub_function = (NCodecDiagUdsDtcSubFunction)ns(
        ReadDtcInformationResponse_sub_function(t));
    m->status_availability_mask = (NCodecDiagUdsDtcStatusFlags)ns(
        ReadDtcInformationResponse_status_availability_mask(t));
    m->dtc_and_status_record =
        ns(ReadDtcInformationResponse_dtc_and_status_record(t));
    m->dtc_and_status_record_len =
        flatbuffers_uint8_vec_len(m->dtc_and_status_record);
}

static void _decode_input_output_control_by_identifier_request(
    ns(InputOutputControlByIdentifierRequest_table_t) t,
    NCodecDiagUdsInputOutputControlByIdentifierRequest* m)
{
    m->data_id = ns(InputOutputControlByIdentifierRequest_data_id(t));
    m->parameter = (NCodecDiagUdsInputOutputControlParameter)ns(
        InputOutputControlByIdentifierRequest_parameter(t));
    m->state = ns(InputOutputControlByIdentifierRequest_state(t));
    m->state_len = flatbuffers_uint8_vec_len(m->state);
    m->enable_mask_record =
        ns(InputOutputControlByIdentifierRequest_enable_mask_record(t));
    m->enable_mask_record_len =
        flatbuffers_uint8_vec_len(m->enable_mask_record);
}

static void _decode_input_output_control_by_identifier_response(
    ns(InputOutputControlByIdentifierResponse_table_t) t,
    NCodecDiagUdsInputOutputControlByIdentifierResponse* m)
{
    m->data_id = ns(InputOutputControlByIdentifierResponse_data_id(t));
    m->parameter = (NCodecDiagUdsInputOutputControlParameter)ns(
        InputOutputControlByIdentifierResponse_parameter(t));
    m->status_record =
        ns(InputOutputControlByIdentifierResponse_status_record(t));
    m->status_record_len = flatbuffers_uint8_vec_len(m->status_record);
}

static void _decode_routine_control_request(
    ns(RoutineControlRequest_table_t) t, NCodecDiagUdsRoutineControlRequest* m)
{
    m->control_type = (NCodecDiagUdsRoutineControlType)ns(
        RoutineControlRequest_control_type(t));
    m->id = ns(RoutineControlRequest_id(t));
    m->control_option_record =
        ns(RoutineControlRequest_control_option_record(t));
    m->control_option_record_len =
        flatbuffers_uint8_vec_len(m->control_option_record);
}

static void _decode_routine_control_response(ns(RoutineControlResponse_table_t)
                                                 t,
    NCodecDiagUdsRoutineControlResponse* m)
{
    m->control_type = (NCodecDiagUdsRoutineControlType)ns(
        RoutineControlResponse_control_type(t));
    m->id = ns(RoutineControlResponse_id(t));
    m->status_record = ns(RoutineControlResponse_status_record(t));
    m->status_record_len = flatbuffers_uint8_vec_len(m->status_record);
}

static void _decode_request_download_request(ns(RequestDownloadRequest_table_t)
                                                 t,
    NCodecDiagUdsRequestDownloadRequest* m)
{
    {
        ns(DataFormatIdentifier_struct_t) s =
            ns(RequestDownloadRequest_dfid(t));
        m->dfid.compression = (NCodecDiagUdsCompressionMethod)ns(
            DataFormatIdentifier_compression(s));
        m->dfid.encryption = (NCodecDiagUdsEncryptionMethod)ns(
            DataFormatIdentifier_encryption(s));
    }
    {
        ns(AddressLengthFormatIdentifier_struct_t) s =
            ns(RequestDownloadRequest_alfid(t));
        m->alfid.address = (NCodecDiagUdsAddressSizing)ns(
            AddressLengthFormatIdentifier_address(s));
        m->alfid.size = (NCodecDiagUdsLengthSizing)ns(
            AddressLengthFormatIdentifier_size(s));
    }
    m->memory_address = ns(RequestDownloadRequest_memory_address(t));
    m->memory_size = ns(RequestDownloadRequest_memory_size(t));
}

static void _decode_request_download_response(
    ns(RequestDownloadResponse_table_t) t,
    NCodecDiagUdsRequestDownloadResponse* m)
{
    {
        ns(LengthFormatIdentifier_struct_t) s =
            ns(RequestDownloadResponse_lfid(t));
        m->lfid.max_block_length_size = (NCodecDiagUdsBlockLengthSizing)ns(
            LengthFormatIdentifier_max_block_length_size(s));
        m->lfid.reserved = (uint8_t)ns(LengthFormatIdentifier_reserved(s));
    }
    m->max_number_of_block_length =
        ns(RequestDownloadResponse_max_number_of_block_length(t));
}

static void _decode_request_upload_request(
    ns(RequestUploadRequest_table_t) t, NCodecDiagUdsRequestUploadRequest* m)
{
    {
        ns(DataFormatIdentifier_struct_t) s = ns(RequestUploadRequest_dfid(t));
        m->dfid.compression = (NCodecDiagUdsCompressionMethod)ns(
            DataFormatIdentifier_compression(s));
        m->dfid.encryption = (NCodecDiagUdsEncryptionMethod)ns(
            DataFormatIdentifier_encryption(s));
    }
    {
        ns(AddressLengthFormatIdentifier_struct_t) s =
            ns(RequestUploadRequest_alfid(t));
        m->alfid.address = (NCodecDiagUdsAddressSizing)ns(
            AddressLengthFormatIdentifier_address(s));
        m->alfid.size = (NCodecDiagUdsLengthSizing)ns(
            AddressLengthFormatIdentifier_size(s));
    }
    m->memory_address = ns(RequestUploadRequest_memory_address(t));
    m->memory_size = ns(RequestUploadRequest_memory_size(t));
}

static void _decode_request_upload_response(
    ns(RequestUploadResponse_table_t) t, NCodecDiagUdsRequestUploadResponse* m)
{
    {
        ns(LengthFormatIdentifier_struct_t) s =
            ns(RequestUploadResponse_lfid(t));
        m->lfid.max_block_length_size = (NCodecDiagUdsBlockLengthSizing)ns(
            LengthFormatIdentifier_max_block_length_size(s));
        m->lfid.reserved = (uint8_t)ns(LengthFormatIdentifier_reserved(s));
    }
    m->max_number_of_block_length =
        ns(RequestUploadResponse_max_number_of_block_length(t));
}

static void _decode_transfer_data_request(
    ns(TransferDataRequest_table_t) t, NCodecDiagUdsTransferDataRequest* m)
{
    m->block_sequence_counter =
        ns(TransferDataRequest_block_sequence_counter(t));
    m->parameter_record = ns(TransferDataRequest_parameter_record(t));
    m->parameter_record_len = flatbuffers_uint8_vec_len(m->parameter_record);
}

static void _decode_transfer_data_response(
    ns(TransferDataResponse_table_t) t, NCodecDiagUdsTransferDataResponse* m)
{
    m->block_sequence_counter =
        ns(TransferDataResponse_block_sequence_counter(t));
    m->parameter_record = ns(TransferDataResponse_parameter_record(t));
    m->parameter_record_len = flatbuffers_uint8_vec_len(m->parameter_record);
}

static void _decode_request_transfer_exit_request(
    ns(RequestTransferExitRequest_table_t) t,
    NCodecDiagUdsRequestTransferExitRequest* m)
{
    m->parameter_record = ns(RequestTransferExitRequest_parameter_record(t));
    m->parameter_record_len = flatbuffers_uint8_vec_len(m->parameter_record);
}

static void _decode_request_transfer_exit_response(
    ns(RequestTransferExitResponse_table_t) t,
    NCodecDiagUdsRequestTransferExitResponse* m)
{
    m->parameter_record = ns(RequestTransferExitResponse_parameter_record(t));
    m->parameter_record_len = flatbuffers_uint8_vec_len(m->parameter_record);
}


static uint32_t _emit_message(
    flatcc_builder_t* B, const NCodecDiagUdsMessage* m)
{
    ns(Message_start_as_root_with_size(B));
    ns(Message_is_request_add(B, m->is_request));
    switch (m->primitive_type) {
    case NCodecDiagUdsServicePrimitiveTypeNegativeResponse: {
        uint32_t ref =
            _emit_negative_response(B, &m->primitive.negative_response);
        ns(Message_primitive_NegativeResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlRequest: {
        uint32_t ref = _emit_diagnostic_session_control_request(
            B, &m->primitive.diagnostic_session_control_request);
        ns(Message_primitive_DiagnosticSessionControlRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlResponse: {
        uint32_t ref = _emit_diagnostic_session_control_response(
            B, &m->primitive.diagnostic_session_control_response);
        ns(Message_primitive_DiagnosticSessionControlResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeEcuResetRequest: {
        uint32_t ref =
            _emit_ecu_reset_request(B, &m->primitive.ecu_reset_request);
        ns(Message_primitive_EcuResetRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeEcuResetResponse: {
        uint32_t ref =
            _emit_ecu_reset_response(B, &m->primitive.ecu_reset_response);
        ns(Message_primitive_EcuResetResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeSecurityAccessRequest: {
        uint32_t ref = _emit_security_access_request(
            B, &m->primitive.security_access_request);
        ns(Message_primitive_SecurityAccessRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeSecurityAccessResponse: {
        uint32_t ref = _emit_security_access_response(
            B, &m->primitive.security_access_response);
        ns(Message_primitive_SecurityAccessResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeCommunicationControlRequest: {
        uint32_t ref = _emit_communication_control_request(
            B, &m->primitive.communication_control_request);
        ns(Message_primitive_CommunicationControlRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeCommunicationControlResponse: {
        uint32_t ref = _emit_communication_control_response(
            B, &m->primitive.communication_control_response);
        ns(Message_primitive_CommunicationControlResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeTesterPresentRequest: {
        uint32_t ref = _emit_tester_present_request(
            B, &m->primitive.tester_present_request);
        ns(Message_primitive_TesterPresentRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeTesterPresentResponse: {
        uint32_t ref = _emit_tester_present_response(
            B, &m->primitive.tester_present_response);
        ns(Message_primitive_TesterPresentResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierRequest: {
        uint32_t ref = _emit_read_data_by_identifier_request(
            B, &m->primitive.read_data_by_identifier_request);
        ns(Message_primitive_ReadDataByIdentifierRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierResponse: {
        uint32_t ref = _emit_read_data_by_identifier_response(
            B, &m->primitive.read_data_by_identifier_response);
        ns(Message_primitive_ReadDataByIdentifierResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierRequest: {
        uint32_t ref = _emit_write_data_by_identifier_request(
            B, &m->primitive.write_data_by_identifier_request);
        ns(Message_primitive_WriteDataByIdentifierRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierResponse: {
        uint32_t ref = _emit_write_data_by_identifier_response(
            B, &m->primitive.write_data_by_identifier_response);
        ns(Message_primitive_WriteDataByIdentifierResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressRequest: {
        uint32_t ref = _emit_read_memory_by_address_request(
            B, &m->primitive.read_memory_by_address_request);
        ns(Message_primitive_ReadMemoryByAddressRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressResponse: {
        uint32_t ref = _emit_read_memory_by_address_response(
            B, &m->primitive.read_memory_by_address_response);
        ns(Message_primitive_ReadMemoryByAddressResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressRequest: {
        uint32_t ref = _emit_write_memory_by_address_request(
            B, &m->primitive.write_memory_by_address_request);
        ns(Message_primitive_WriteMemoryByAddressRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressResponse: {
        uint32_t ref = _emit_write_memory_by_address_response(
            B, &m->primitive.write_memory_by_address_response);
        ns(Message_primitive_WriteMemoryByAddressResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationRequest: {
        uint32_t ref = _emit_clear_diagnostic_information_request(
            B, &m->primitive.clear_diagnostic_information_request);
        ns(Message_primitive_ClearDiagnosticInformationRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationResponse: {
        ns(ClearDiagnosticInformationResponse_start(B));
        ns(Message_primitive_ClearDiagnosticInformationResponse_add(
            B, ns(ClearDiagnosticInformationResponse_end(B))));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeReadDtcInformationRequest: {
        uint32_t ref = _emit_read_dtc_information_request(
            B, &m->primitive.read_dtc_information_request);
        ns(Message_primitive_ReadDtcInformationRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeReadDtcInformationResponse: {
        uint32_t ref = _emit_read_dtc_information_response(
            B, &m->primitive.read_dtc_information_response);
        ns(Message_primitive_ReadDtcInformationResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierRequest: {
        uint32_t ref = _emit_input_output_control_by_identifier_request(
            B, &m->primitive.input_output_control_by_identifier_request);
        ns(Message_primitive_InputOutputControlByIdentifierRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierResponse: {
        uint32_t ref = _emit_input_output_control_by_identifier_response(
            B, &m->primitive.input_output_control_by_identifier_response);
        ns(Message_primitive_InputOutputControlByIdentifierResponse_add(
            B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRoutineControlRequest: {
        uint32_t ref = _emit_routine_control_request(
            B, &m->primitive.routine_control_request);
        ns(Message_primitive_RoutineControlRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRoutineControlResponse: {
        uint32_t ref = _emit_routine_control_response(
            B, &m->primitive.routine_control_response);
        ns(Message_primitive_RoutineControlResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRequestDownloadRequest: {
        uint32_t ref = _emit_request_download_request(
            B, &m->primitive.request_download_request);
        ns(Message_primitive_RequestDownloadRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRequestDownloadResponse: {
        uint32_t ref = _emit_request_download_response(
            B, &m->primitive.request_download_response);
        ns(Message_primitive_RequestDownloadResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRequestUploadRequest: {
        uint32_t ref = _emit_request_upload_request(
            B, &m->primitive.request_upload_request);
        ns(Message_primitive_RequestUploadRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRequestUploadResponse: {
        uint32_t ref = _emit_request_upload_response(
            B, &m->primitive.request_upload_response);
        ns(Message_primitive_RequestUploadResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeTransferDataRequest: {
        uint32_t ref =
            _emit_transfer_data_request(B, &m->primitive.transfer_data_request);
        ns(Message_primitive_TransferDataRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeTransferDataResponse: {
        uint32_t ref = _emit_transfer_data_response(
            B, &m->primitive.transfer_data_response);
        ns(Message_primitive_TransferDataResponse_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRequestTransferExitRequest: {
        uint32_t ref = _emit_request_transfer_exit_request(
            B, &m->primitive.request_transfer_exit_request);
        ns(Message_primitive_RequestTransferExitRequest_add(B, ref));
    } break;
    case NCodecDiagUdsServicePrimitiveTypeRequestTransferExitResponse: {
        uint32_t ref = _emit_request_transfer_exit_response(
            B, &m->primitive.request_transfer_exit_response);
        ns(Message_primitive_RequestTransferExitResponse_add(B, ref));
    } break;
    default:
        break;
    }
    ns(Message_service_id_add(B, m->service_id));
    return ns(Message_end_as_root(B));
}


static void _decode_message(
    DiagCodecInstance* nc, ns(Message_table_t) t, NCodecDiagUdsMessage* m)
{
    m->is_request = ns(Message_is_request(t));
    switch (ns(Message_primitive_type(t))) {
    case ns(ServicePrimitive_NegativeResponse):
        m->primitive_type = NCodecDiagUdsServicePrimitiveTypeNegativeResponse;
        _decode_negative_response(
            (ns(NegativeResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.negative_response);
        break;
    case ns(ServicePrimitive_DiagnosticSessionControlRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlRequest;
        _decode_diagnostic_session_control_request(
            (ns(DiagnosticSessionControlRequest_table_t))ns(
                Message_primitive(t)),
            &m->primitive.diagnostic_session_control_request);
        break;
    case ns(ServicePrimitive_DiagnosticSessionControlResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeDiagnosticSessionControlResponse;
        _decode_diagnostic_session_control_response(
            (ns(DiagnosticSessionControlResponse_table_t))ns(
                Message_primitive(t)),
            &m->primitive.diagnostic_session_control_response);
        break;
    case ns(ServicePrimitive_EcuResetRequest):
        m->primitive_type = NCodecDiagUdsServicePrimitiveTypeEcuResetRequest;
        _decode_ecu_reset_request(
            (ns(EcuResetRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.ecu_reset_request);
        break;
    case ns(ServicePrimitive_EcuResetResponse):
        m->primitive_type = NCodecDiagUdsServicePrimitiveTypeEcuResetResponse;
        _decode_ecu_reset_response(
            (ns(EcuResetResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.ecu_reset_response);
        break;
    case ns(ServicePrimitive_SecurityAccessRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeSecurityAccessRequest;
        _decode_security_access_request(
            (ns(SecurityAccessRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.security_access_request);
        break;
    case ns(ServicePrimitive_SecurityAccessResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeSecurityAccessResponse;
        _decode_security_access_response(
            (ns(SecurityAccessResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.security_access_response);
        break;
    case ns(ServicePrimitive_CommunicationControlRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeCommunicationControlRequest;
        _decode_communication_control_request(
            (ns(CommunicationControlRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.communication_control_request);
        break;
    case ns(ServicePrimitive_CommunicationControlResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeCommunicationControlResponse;
        _decode_communication_control_response(
            (ns(CommunicationControlResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.communication_control_response);
        break;
    case ns(ServicePrimitive_TesterPresentRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeTesterPresentRequest;
        _decode_tester_present_request(
            (ns(TesterPresentRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.tester_present_request);
        break;
    case ns(ServicePrimitive_TesterPresentResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeTesterPresentResponse;
        _decode_tester_present_response(
            (ns(TesterPresentResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.tester_present_response);
        break;
    case ns(ServicePrimitive_ReadDataByIdentifierRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierRequest;
        _decode_read_data_by_identifier_request(nc,
            (ns(ReadDataByIdentifierRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.read_data_by_identifier_request);
        break;
    case ns(ServicePrimitive_ReadDataByIdentifierResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierResponse;
        _decode_read_data_by_identifier_response(nc,
            (ns(ReadDataByIdentifierResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.read_data_by_identifier_response);
        break;
    case ns(ServicePrimitive_WriteDataByIdentifierRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierRequest;
        _decode_write_data_by_identifier_request(
            (ns(WriteDataByIdentifierRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.write_data_by_identifier_request);
        break;
    case ns(ServicePrimitive_WriteDataByIdentifierResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeWriteDataByIdentifierResponse;
        _decode_write_data_by_identifier_response(
            (ns(WriteDataByIdentifierResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.write_data_by_identifier_response);
        break;
    case ns(ServicePrimitive_ReadMemoryByAddressRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressRequest;
        _decode_read_memory_by_address_request(
            (ns(ReadMemoryByAddressRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.read_memory_by_address_request);
        break;
    case ns(ServicePrimitive_ReadMemoryByAddressResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeReadMemoryByAddressResponse;
        _decode_read_memory_by_address_response(
            (ns(ReadMemoryByAddressResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.read_memory_by_address_response);
        break;
    case ns(ServicePrimitive_WriteMemoryByAddressRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressRequest;
        _decode_write_memory_by_address_request(
            (ns(WriteMemoryByAddressRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.write_memory_by_address_request);
        break;
    case ns(ServicePrimitive_WriteMemoryByAddressResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeWriteMemoryByAddressResponse;
        _decode_write_memory_by_address_response(
            (ns(WriteMemoryByAddressResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.write_memory_by_address_response);
        break;
    case ns(ServicePrimitive_ClearDiagnosticInformationRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationRequest;
        _decode_clear_diagnostic_information_request(
            (ns(ClearDiagnosticInformationRequest_table_t))ns(
                Message_primitive(t)),
            &m->primitive.clear_diagnostic_information_request);
        break;
    case ns(ServicePrimitive_ClearDiagnosticInformationResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeClearDiagnosticInformationResponse;
        break;
    case ns(ServicePrimitive_ReadDtcInformationRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeReadDtcInformationRequest;
        _decode_read_dtc_information_request(
            (ns(ReadDtcInformationRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.read_dtc_information_request);
        break;
    case ns(ServicePrimitive_ReadDtcInformationResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeReadDtcInformationResponse;
        _decode_read_dtc_information_response(
            (ns(ReadDtcInformationResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.read_dtc_information_response);
        break;
    case ns(ServicePrimitive_InputOutputControlByIdentifierRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierRequest;
        _decode_input_output_control_by_identifier_request(
            (ns(InputOutputControlByIdentifierRequest_table_t))ns(
                Message_primitive(t)),
            &m->primitive.input_output_control_by_identifier_request);
        break;
    case ns(ServicePrimitive_InputOutputControlByIdentifierResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeInputOutputControlByIdentifierResponse;
        _decode_input_output_control_by_identifier_response(
            (ns(InputOutputControlByIdentifierResponse_table_t))ns(
                Message_primitive(t)),
            &m->primitive.input_output_control_by_identifier_response);
        break;
    case ns(ServicePrimitive_RoutineControlRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRoutineControlRequest;
        _decode_routine_control_request(
            (ns(RoutineControlRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.routine_control_request);
        break;
    case ns(ServicePrimitive_RoutineControlResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRoutineControlResponse;
        _decode_routine_control_response(
            (ns(RoutineControlResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.routine_control_response);
        break;
    case ns(ServicePrimitive_RequestDownloadRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRequestDownloadRequest;
        _decode_request_download_request(
            (ns(RequestDownloadRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.request_download_request);
        break;
    case ns(ServicePrimitive_RequestDownloadResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRequestDownloadResponse;
        _decode_request_download_response(
            (ns(RequestDownloadResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.request_download_response);
        break;
    case ns(ServicePrimitive_RequestUploadRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRequestUploadRequest;
        _decode_request_upload_request(
            (ns(RequestUploadRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.request_upload_request);
        break;
    case ns(ServicePrimitive_RequestUploadResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRequestUploadResponse;
        _decode_request_upload_response(
            (ns(RequestUploadResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.request_upload_response);
        break;
    case ns(ServicePrimitive_TransferDataRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeTransferDataRequest;
        _decode_transfer_data_request(
            (ns(TransferDataRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.transfer_data_request);
        break;
    case ns(ServicePrimitive_TransferDataResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeTransferDataResponse;
        _decode_transfer_data_response(
            (ns(TransferDataResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.transfer_data_response);
        break;
    case ns(ServicePrimitive_RequestTransferExitRequest):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRequestTransferExitRequest;
        _decode_request_transfer_exit_request(
            (ns(RequestTransferExitRequest_table_t))ns(Message_primitive(t)),
            &m->primitive.request_transfer_exit_request);
        break;
    case ns(ServicePrimitive_RequestTransferExitResponse):
        m->primitive_type =
            NCodecDiagUdsServicePrimitiveTypeRequestTransferExitResponse;
        _decode_request_transfer_exit_response(
            (ns(RequestTransferExitResponse_table_t))ns(Message_primitive(t)),
            &m->primitive.request_transfer_exit_response);
        break;
    default:
        break;
    }
    m->service_id = (NCodecDiagUdsServiceId)ns(Message_service_id(t));
}


int32_t uds_write(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*    _nc = (DiagCodecInstance*)nc;
    NCodecDiagUdsMessage* _msg = (NCodecDiagUdsMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    flatcc_builder_t* B = &_nc->fbs_builder;
    flatcc_builder_reset(B);
    _emit_message(B, _msg);
    return diag_stream_write(_nc);
}


int32_t uds_read(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*    _nc = (DiagCodecInstance*)nc;
    NCodecDiagUdsMessage* _msg = (NCodecDiagUdsMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    /* Reset the message, in case caller ignores the return value. */
    *_msg = (NCodecDiagUdsMessage){};

    size_t   length = 0;
    uint8_t* buffer = diag_stream_read(_nc, flatbuffers_identifier, &length);
    if (buffer == NULL) return -ENOMSG;

    _decode_message(_nc, ns(Message_as_root(buffer)), _msg);
    return length;
}
