// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdint.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/diag/codec.h>
#include <dse/ncodec/interface/diag/xil.h>
#include <dse/ncodec/schema/abs/diagnostic/xil_builder.h>


#undef ns
#define ns(x)                                                                  \
    FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Diagnostic_Xil_DiagPort, x)


extern void*    diag_alloc(DiagCodecInstance* nc, size_t count, size_t size);
extern uint8_t* diag_stream_read(
    DiagCodecInstance* nc, const char* identifier, size_t* length);
extern int32_t diag_stream_write(DiagCodecInstance* nc);


/* DiagnosticValue and DiagnosticSeries are mutually recursive. */

static uint32_t _emit_diagnostic_series(
    flatcc_builder_t* B, const NCodecDiagXilDiagnosticSeries* m);
static void _decode_diagnostic_series(DiagCodecInstance* nc,
    ns(DiagnosticSeries_table_t) t, NCodecDiagXilDiagnosticSeries* m);


static uint32_t _emit_diagnostic_property(
    flatcc_builder_t* B, const NCodecDiagXilDiagnosticProperty* m)
{
    if (m->name == NULL) return 0;

    ns(DiagnosticProperty_start(B));
    ns(DiagnosticProperty_name_add(
        B, flatbuffers_string_create_str(B, m->name)));
    if (m->value) {
        ns(DiagnosticProperty_value_add(
            B, flatbuffers_string_create_str(B, m->value)));
    }
    return ns(DiagnosticProperty_end(B));
}

static uint32_t _emit_diagnostic_value(
    flatcc_builder_t* B, const NCodecDiagXilDiagnosticValue* m)
{
    uint32_t elements = 0;
    if (m->elements) {
        ns(DiagnosticValue_vec_start(B));
        for (size_t i = 0; i < m->elements_len; i++) {
            uint32_t ref = _emit_diagnostic_value(B, &m->elements[i]);
            if (ref == 0) return 0;
            ns(DiagnosticValue_vec_push(B, ref));
        }
        elements = ns(DiagnosticValue_vec_end(B));
    }
    uint32_t series = 0;
    if (m->series) {
        ns(DiagnosticSeries_vec_start(B));
        for (size_t i = 0; i < m->series_len; i++) {
            uint32_t ref = _emit_diagnostic_series(B, &m->series[i]);
            if (ref == 0) return 0;
            ns(DiagnosticSeries_vec_push(B, ref));
        }
        series = ns(DiagnosticSeries_vec_end(B));
    }
    uint32_t properties = 0;
    if (m->properties) {
        ns(DiagnosticProperty_vec_start(B));
        for (size_t i = 0; i < m->properties_len; i++) {
            uint32_t ref = _emit_diagnostic_property(B, &m->properties[i]);
            if (ref == 0) return 0;
            ns(DiagnosticProperty_vec_push(B, ref));
        }
        properties = ns(DiagnosticProperty_vec_end(B));
    }

    ns(DiagnosticValue_start(B));
    if (m->name) {
        ns(DiagnosticValue_name_add(
            B, flatbuffers_string_create_str(B, m->name)));
    }
    ns(DiagnosticValue_kind_add(B, m->kind));
    ns(DiagnosticValue_boolean_value_add(B, m->boolean_value));
    ns(DiagnosticValue_signed_value_add(B, m->signed_value));
    ns(DiagnosticValue_unsigned_value_add(B, m->unsigned_value));
    ns(DiagnosticValue_float_value_add(B, m->float_value));
    if (m->string_value) {
        ns(DiagnosticValue_string_value_add(
            B, flatbuffers_string_create_str(B, m->string_value)));
    }
    if (m->bytes_value) {
        ns(DiagnosticValue_bytes_value_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->bytes_value, m->bytes_value_len)));
    }
    if (elements) ns(DiagnosticValue_elements_add(B, elements));
    ns(DiagnosticValue_row_count_add(B, m->row_count));
    ns(DiagnosticValue_column_count_add(B, m->column_count));
    if (m->x_values) {
        ns(DiagnosticValue_x_values_add(
            B, flatbuffers_double_vec_create(B, m->x_values, m->x_values_len)));
    }
    if (m->y_values) {
        ns(DiagnosticValue_y_values_add(
            B, flatbuffers_double_vec_create(B, m->y_values, m->y_values_len)));
    }
    if (series) ns(DiagnosticValue_series_add(B, series));
    if (m->unit) {
        ns(DiagnosticValue_unit_add(
            B, flatbuffers_string_create_str(B, m->unit)));
    }
    if (m->description) {
        ns(DiagnosticValue_description_add(
            B, flatbuffers_string_create_str(B, m->description)));
    }
    if (properties) ns(DiagnosticValue_properties_add(B, properties));
    return ns(DiagnosticValue_end(B));
}

static uint32_t _emit_diagnostic_series(
    flatcc_builder_t* B, const NCodecDiagXilDiagnosticSeries* m)
{
    uint32_t values = 0;
    if (m->values) {
        ns(DiagnosticValue_vec_start(B));
        for (size_t i = 0; i < m->values_len; i++) {
            uint32_t ref = _emit_diagnostic_value(B, &m->values[i]);
            if (ref == 0) return 0;
            ns(DiagnosticValue_vec_push(B, ref));
        }
        values = ns(DiagnosticValue_vec_end(B));
    }

    ns(DiagnosticSeries_start(B));
    if (m->name) {
        ns(DiagnosticSeries_name_add(
            B, flatbuffers_string_create_str(B, m->name)));
    }
    if (values) ns(DiagnosticSeries_values_add(B, values));
    return ns(DiagnosticSeries_end(B));
}

static uint32_t _emit_named_diagnostic_value(
    flatcc_builder_t* B, const NCodecDiagXilNamedDiagnosticValue* m)
{
    if (m->name == NULL) return 0;

    uint32_t value = _emit_diagnostic_value(B, &m->value);
    if (value == 0) return 0;

    ns(NamedDiagnosticValue_start(B));
    ns(NamedDiagnosticValue_name_add(
        B, flatbuffers_string_create_str(B, m->name)));
    ns(NamedDiagnosticValue_value_add(B, value));
    return ns(NamedDiagnosticValue_end(B));
}

static uint32_t _emit_data_value(
    flatcc_builder_t* B, const NCodecDiagXilDataValue* m)
{
    uint32_t physical_value = 0;
    if (m->physical_value) {
        physical_value = _emit_diagnostic_value(B, m->physical_value);
        if (physical_value == 0) return 0;
    }

    ns(DataValue_start(B));
    if (m->path) {
        ns(DataValue_path_add(B, flatbuffers_string_create_str(B, m->path)));
    }
    if (m->raw_value) {
        ns(DataValue_raw_value_add(B,
            flatbuffers_uint8_vec_create(B, m->raw_value, m->raw_value_len)));
    }
    if (physical_value) ns(DataValue_physical_value_add(B, physical_value));
    return ns(DataValue_end(B));
}

static uint32_t _emit_did_selector(
    flatcc_builder_t* B, const NCodecDiagXilDidSelector* m)
{
    ns(DidSelector_start(B));
    ns(DidSelector_kind_add(B, m->kind));
    ns(DidSelector_did_identifier_add(B, m->identifier));
    if (m->name) {
        ns(DidSelector_name_add(B, flatbuffers_string_create_str(B, m->name)));
    }
    return ns(DidSelector_end(B));
}

static uint32_t _emit_did_identity(
    flatcc_builder_t* B, const NCodecDiagXilDidIdentity* m)
{
    ns(DidIdentity_start(B));
    if (m->name) {
        ns(DidIdentity_name_add(B, flatbuffers_string_create_str(B, m->name)));
    }
    ns(DidIdentity_did_identifier_add(B, m->identifier));
    return ns(DidIdentity_end(B));
}

static uint32_t _emit_did_value(
    flatcc_builder_t* B, const NCodecDiagXilDidValue* m)
{
    uint32_t did = _emit_did_selector(B, &m->did);
    uint32_t values = 0;
    if (m->values) {
        ns(DataValue_vec_start(B));
        for (size_t i = 0; i < m->values_len; i++) {
            uint32_t ref = _emit_data_value(B, &m->values[i]);
            if (ref == 0) return 0;
            ns(DataValue_vec_push(B, ref));
        }
        values = ns(DataValue_vec_end(B));
    }
    uint32_t identity = 0;
    if (m->identity) {
        identity = _emit_did_identity(B, m->identity);
    }

    ns(DidValue_start(B));
    ns(DidValue_did_add(B, did));
    if (m->raw_value) {
        ns(DidValue_raw_value_add(B,
            flatbuffers_uint8_vec_create(B, m->raw_value, m->raw_value_len)));
    }
    if (values) ns(DidValue_values_add(B, values));
    if (identity) ns(DidValue_identity_add(B, identity));
    return ns(DidValue_end(B));
}

static uint32_t _emit_diagnostic_trouble_code(
    flatcc_builder_t* B, const NCodecDiagXilDiagnosticTroubleCode* m)
{
    uint32_t environment_data = 0;
    if (m->environment_data) {
        ns(DataValue_vec_start(B));
        for (size_t i = 0; i < m->environment_data_len; i++) {
            uint32_t ref = _emit_data_value(B, &m->environment_data[i]);
            if (ref == 0) return 0;
            ns(DataValue_vec_push(B, ref));
        }
        environment_data = ns(DataValue_vec_end(B));
    }

    ns(DiagnosticTroubleCode_start(B));
    ns(DiagnosticTroubleCode_value_add(B, m->value));
    ns(DiagnosticTroubleCode_status_add(B, m->status));
    if (m->short_name) {
        ns(DiagnosticTroubleCode_short_name_add(
            B, flatbuffers_string_create_str(B, m->short_name)));
    }
    if (m->long_name) {
        ns(DiagnosticTroubleCode_long_name_add(
            B, flatbuffers_string_create_str(B, m->long_name)));
    }
    if (m->description) {
        ns(DiagnosticTroubleCode_description_add(
            B, flatbuffers_string_create_str(B, m->description)));
    }
    if (environment_data)
        ns(DiagnosticTroubleCode_environment_data_add(B, environment_data));
    return ns(DiagnosticTroubleCode_end(B));
}

static uint32_t _emit_execute_job_call(
    flatcc_builder_t* B, const NCodecDiagXilExecuteJobCall* m)
{
    if (m->job_name == NULL) return 0;

    uint32_t parameters = 0;
    if (m->parameters) {
        ns(NamedDiagnosticValue_vec_start(B));
        for (size_t i = 0; i < m->parameters_len; i++) {
            uint32_t ref = _emit_named_diagnostic_value(B, &m->parameters[i]);
            if (ref == 0) return 0;
            ns(NamedDiagnosticValue_vec_push(B, ref));
        }
        parameters = ns(NamedDiagnosticValue_vec_end(B));
    }

    ns(ExecuteJobCall_start(B));
    ns(ExecuteJobCall_job_name_add(
        B, flatbuffers_string_create_str(B, m->job_name)));
    if (parameters) ns(ExecuteJobCall_parameters_add(B, parameters));
    return ns(ExecuteJobCall_end(B));
}

static uint32_t _emit_execute_job_return(
    flatcc_builder_t* B, const NCodecDiagXilExecuteJobReturn* m)
{
    uint32_t values = 0;
    if (m->values) {
        ns(DataValue_vec_start(B));
        for (size_t i = 0; i < m->values_len; i++) {
            uint32_t ref = _emit_data_value(B, &m->values[i]);
            if (ref == 0) return 0;
            ns(DataValue_vec_push(B, ref));
        }
        values = ns(DataValue_vec_end(B));
    }

    ns(ExecuteJobReturn_start(B));
    ns(ExecuteJobReturn_status_add(B, m->status));
    if (values) ns(ExecuteJobReturn_values_add(B, values));
    if (m->info) {
        ns(ExecuteJobReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(ExecuteJobReturn_end(B));
}

static uint32_t _emit_send_hex_service_call(
    flatcc_builder_t* B, const NCodecDiagXilSendHexServiceCall* m)
{
    if (m->pdu == NULL) return 0;

    ns(SendHexServiceCall_start(B));
    ns(SendHexServiceCall_pdu_add(
        B, flatbuffers_uint8_vec_create(B, m->pdu, m->pdu_len)));
    ns(SendHexServiceCall_response_required_add(B, m->response_required));
    return ns(SendHexServiceCall_end(B));
}

static uint32_t _emit_send_hex_service_return(
    flatcc_builder_t* B, const NCodecDiagXilSendHexServiceReturn* m)
{
    ns(SendHexServiceReturn_start(B));
    ns(SendHexServiceReturn_status_add(B, m->status));
    if (m->response_pdu) {
        ns(SendHexServiceReturn_response_pdu_add(
            B, flatbuffers_uint8_vec_create(
                   B, m->response_pdu, m->response_pdu_len)));
    }
    if (m->info) {
        ns(SendHexServiceReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(SendHexServiceReturn_end(B));
}

static uint32_t _emit_read_from_address_call(
    flatcc_builder_t* B, const NCodecDiagXilReadFromAddressCall* m)
{
    ns(ReadFromAddressCall_start(B));
    ns(ReadFromAddressCall_address_add(B, m->address));
    ns(ReadFromAddressCall_number_of_bytes_add(B, m->number_of_bytes));
    return ns(ReadFromAddressCall_end(B));
}

static uint32_t _emit_read_from_address_return(
    flatcc_builder_t* B, const NCodecDiagXilReadFromAddressReturn* m)
{
    ns(ReadFromAddressReturn_start(B));
    ns(ReadFromAddressReturn_status_add(B, m->status));
    if (m->data) {
        ns(ReadFromAddressReturn_data_add(
            B, flatbuffers_uint8_vec_create(B, m->data, m->data_len)));
    }
    if (m->info) {
        ns(ReadFromAddressReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(ReadFromAddressReturn_end(B));
}

static uint32_t _emit_write_to_address_call(
    flatcc_builder_t* B, const NCodecDiagXilWriteToAddressCall* m)
{
    if (m->data == NULL) return 0;

    ns(WriteToAddressCall_start(B));
    ns(WriteToAddressCall_address_add(B, m->address));
    ns(WriteToAddressCall_data_add(
        B, flatbuffers_uint8_vec_create(B, m->data, m->data_len)));
    return ns(WriteToAddressCall_end(B));
}

static uint32_t _emit_write_to_address_return(
    flatcc_builder_t* B, const NCodecDiagXilWriteToAddressReturn* m)
{
    ns(WriteToAddressReturn_start(B));
    ns(WriteToAddressReturn_status_add(B, m->status));
    if (m->info) {
        ns(WriteToAddressReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(WriteToAddressReturn_end(B));
}

static uint32_t _emit_read_did_value_call(
    flatcc_builder_t* B, const NCodecDiagXilReadDIDValueCall* m)
{
    uint32_t did = _emit_did_selector(B, &m->did);

    ns(ReadDIDValueCall_start(B));
    ns(ReadDIDValueCall_did_add(B, did));
    return ns(ReadDIDValueCall_end(B));
}

static uint32_t _emit_read_did_value_return(
    flatcc_builder_t* B, const NCodecDiagXilReadDIDValueReturn* m)
{
    uint32_t value = 0;
    if (m->value) {
        value = _emit_did_value(B, m->value);
        if (value == 0) return 0;
    }

    ns(ReadDIDValueReturn_start(B));
    ns(ReadDIDValueReturn_status_add(B, m->status));
    ns(ReadDIDValueReturn_negative_response_code_add(
        B, m->negative_response_code));
    if (value) ns(ReadDIDValueReturn_value_add(B, value));
    if (m->info) {
        ns(ReadDIDValueReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(ReadDIDValueReturn_end(B));
}

static uint32_t _emit_write_did_value_call(
    flatcc_builder_t* B, const NCodecDiagXilWriteDIDValueCall* m)
{
    uint32_t value = _emit_did_value(B, &m->value);
    if (value == 0) return 0;

    ns(WriteDIDValueCall_start(B));
    ns(WriteDIDValueCall_value_add(B, value));
    return ns(WriteDIDValueCall_end(B));
}

static uint32_t _emit_write_did_value_return(
    flatcc_builder_t* B, const NCodecDiagXilWriteDIDValueReturn* m)
{
    ns(WriteDIDValueReturn_start(B));
    ns(WriteDIDValueReturn_status_add(B, m->status));
    ns(WriteDIDValueReturn_negative_response_code_add(
        B, m->negative_response_code));
    if (m->info) {
        ns(WriteDIDValueReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(WriteDIDValueReturn_end(B));
}

static uint32_t _emit_get_identification_data_return(
    flatcc_builder_t* B, const NCodecDiagXilGetIdentificationDataReturn* m)
{
    uint32_t identifiers = 0;
    if (m->identifiers) {
        ns(DidIdentity_vec_start(B));
        for (size_t i = 0; i < m->identifiers_len; i++) {
            ns(DidIdentity_vec_push(
                B, _emit_did_identity(B, &m->identifiers[i])));
        }
        identifiers = ns(DidIdentity_vec_end(B));
    }

    ns(GetIdentificationDataReturn_start(B));
    ns(GetIdentificationDataReturn_status_add(B, m->status));
    if (identifiers)
        ns(GetIdentificationDataReturn_identifiers_add(B, identifiers));
    if (m->info) {
        ns(GetIdentificationDataReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(GetIdentificationDataReturn_end(B));
}

static uint32_t _emit_get_variant_data_return(
    flatcc_builder_t* B, const NCodecDiagXilGetVariantDataReturn* m)
{
    uint32_t values = 0;
    if (m->values) {
        ns(DidValue_vec_start(B));
        for (size_t i = 0; i < m->values_len; i++) {
            uint32_t ref = _emit_did_value(B, &m->values[i]);
            if (ref == 0) return 0;
            ns(DidValue_vec_push(B, ref));
        }
        values = ns(DidValue_vec_end(B));
    }

    ns(GetVariantDataReturn_start(B));
    ns(GetVariantDataReturn_status_add(B, m->status));
    if (values) ns(GetVariantDataReturn_values_add(B, values));
    if (m->info) {
        ns(GetVariantDataReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(GetVariantDataReturn_end(B));
}

static uint32_t _emit_set_variant_data_call(
    flatcc_builder_t* B, const NCodecDiagXilSetVariantDataCall* m)
{
    if (m->values == NULL) return 0;

    uint32_t values = 0;
    if (m->values) {
        ns(DidValue_vec_start(B));
        for (size_t i = 0; i < m->values_len; i++) {
            uint32_t ref = _emit_did_value(B, &m->values[i]);
            if (ref == 0) return 0;
            ns(DidValue_vec_push(B, ref));
        }
        values = ns(DidValue_vec_end(B));
    }

    ns(SetVariantDataCall_start(B));
    if (values) ns(SetVariantDataCall_values_add(B, values));
    return ns(SetVariantDataCall_end(B));
}

static uint32_t _emit_set_variant_data_return(
    flatcc_builder_t* B, const NCodecDiagXilSetVariantDataReturn* m)
{
    ns(SetVariantDataReturn_start(B));
    ns(SetVariantDataReturn_status_add(B, m->status));
    if (m->info) {
        ns(SetVariantDataReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(SetVariantDataReturn_end(B));
}

static uint32_t _emit_read_fault_memory_call(
    flatcc_builder_t* B, const NCodecDiagXilReadFaultMemoryCall* m)
{
    ns(ReadFaultMemoryCall_start(B));
    ns(ReadFaultMemoryCall_filter_add(B, m->filter));
    return ns(ReadFaultMemoryCall_end(B));
}

static uint32_t _emit_read_fault_memory_return(
    flatcc_builder_t* B, const NCodecDiagXilReadFaultMemoryReturn* m)
{
    uint32_t trouble_codes = 0;
    if (m->trouble_codes) {
        ns(DiagnosticTroubleCode_vec_start(B));
        for (size_t i = 0; i < m->trouble_codes_len; i++) {
            uint32_t ref =
                _emit_diagnostic_trouble_code(B, &m->trouble_codes[i]);
            if (ref == 0) return 0;
            ns(DiagnosticTroubleCode_vec_push(B, ref));
        }
        trouble_codes = ns(DiagnosticTroubleCode_vec_end(B));
    }

    ns(ReadFaultMemoryReturn_start(B));
    ns(ReadFaultMemoryReturn_status_add(B, m->status));
    if (trouble_codes)
        ns(ReadFaultMemoryReturn_trouble_codes_add(B, trouble_codes));
    if (m->info) {
        ns(ReadFaultMemoryReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(ReadFaultMemoryReturn_end(B));
}

static uint32_t _emit_clear_fault_memory_return(
    flatcc_builder_t* B, const NCodecDiagXilClearFaultMemoryReturn* m)
{
    ns(ClearFaultMemoryReturn_start(B));
    ns(ClearFaultMemoryReturn_status_add(B, m->status));
    if (m->info) {
        ns(ClearFaultMemoryReturn_info_add(
            B, flatbuffers_string_create_str(B, m->info)));
    }
    return ns(ClearFaultMemoryReturn_end(B));
}


static void _decode_diagnostic_property(
    ns(DiagnosticProperty_table_t) t, NCodecDiagXilDiagnosticProperty* m)
{
    m->name = ns(DiagnosticProperty_name(t));
    m->value = ns(DiagnosticProperty_value(t));
}

static void _decode_diagnostic_value(DiagCodecInstance* nc,
    ns(DiagnosticValue_table_t) t, NCodecDiagXilDiagnosticValue* m)
{
    m->name = ns(DiagnosticValue_name(t));
    m->kind = (NCodecDiagXilDiagnosticValueKind)ns(DiagnosticValue_kind(t));
    m->boolean_value = ns(DiagnosticValue_boolean_value(t));
    m->signed_value = ns(DiagnosticValue_signed_value(t));
    m->unsigned_value = ns(DiagnosticValue_unsigned_value(t));
    m->float_value = ns(DiagnosticValue_float_value(t));
    m->string_value = ns(DiagnosticValue_string_value(t));
    m->bytes_value = ns(DiagnosticValue_bytes_value(t));
    m->bytes_value_len = flatbuffers_uint8_vec_len(m->bytes_value);
    {
        ns(DiagnosticValue_vec_t) v = ns(DiagnosticValue_elements(t));
        if (v) {
            size_t                        len = ns(DiagnosticValue_vec_len(v));
            NCodecDiagXilDiagnosticValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDiagnosticValue));
            for (size_t i = 0; i < len; i++) {
                _decode_diagnostic_value(
                    nc, ns(DiagnosticValue_vec_at(v, i)), &data[i]);
            }
            m->elements = data;
            m->elements_len = len;
        }
    }
    m->row_count = ns(DiagnosticValue_row_count(t));
    m->column_count = ns(DiagnosticValue_column_count(t));
    {
        flatbuffers_double_vec_t v = ns(DiagnosticValue_x_values(t));
        if (v) {
            size_t  len = flatbuffers_double_vec_len(v);
            double* data = diag_alloc(nc, len, sizeof(double));
            for (size_t i = 0; i < len; i++) {
                data[i] = flatbuffers_double_vec_at(v, i);
            }
            m->x_values = data;
            m->x_values_len = len;
        }
    }
    {
        flatbuffers_double_vec_t v = ns(DiagnosticValue_y_values(t));
        if (v) {
            size_t  len = flatbuffers_double_vec_len(v);
            double* data = diag_alloc(nc, len, sizeof(double));
            for (size_t i = 0; i < len; i++) {
                data[i] = flatbuffers_double_vec_at(v, i);
            }
            m->y_values = data;
            m->y_values_len = len;
        }
    }
    {
        ns(DiagnosticSeries_vec_t) v = ns(DiagnosticValue_series(t));
        if (v) {
            size_t len = ns(DiagnosticSeries_vec_len(v));
            NCodecDiagXilDiagnosticSeries* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDiagnosticSeries));
            for (size_t i = 0; i < len; i++) {
                _decode_diagnostic_series(
                    nc, ns(DiagnosticSeries_vec_at(v, i)), &data[i]);
            }
            m->series = data;
            m->series_len = len;
        }
    }
    m->unit = ns(DiagnosticValue_unit(t));
    m->description = ns(DiagnosticValue_description(t));
    {
        ns(DiagnosticProperty_vec_t) v = ns(DiagnosticValue_properties(t));
        if (v) {
            size_t len = ns(DiagnosticProperty_vec_len(v));
            NCodecDiagXilDiagnosticProperty* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDiagnosticProperty));
            for (size_t i = 0; i < len; i++) {
                _decode_diagnostic_property(
                    ns(DiagnosticProperty_vec_at(v, i)), &data[i]);
            }
            m->properties = data;
            m->properties_len = len;
        }
    }
}

static void _decode_diagnostic_series(DiagCodecInstance* nc,
    ns(DiagnosticSeries_table_t) t, NCodecDiagXilDiagnosticSeries* m)
{
    m->name = ns(DiagnosticSeries_name(t));
    {
        ns(DiagnosticValue_vec_t) v = ns(DiagnosticSeries_values(t));
        if (v) {
            size_t                        len = ns(DiagnosticValue_vec_len(v));
            NCodecDiagXilDiagnosticValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDiagnosticValue));
            for (size_t i = 0; i < len; i++) {
                _decode_diagnostic_value(
                    nc, ns(DiagnosticValue_vec_at(v, i)), &data[i]);
            }
            m->values = data;
            m->values_len = len;
        }
    }
}

static void _decode_named_diagnostic_value(DiagCodecInstance* nc,
    ns(NamedDiagnosticValue_table_t) t, NCodecDiagXilNamedDiagnosticValue* m)
{
    m->name = ns(NamedDiagnosticValue_name(t));
    _decode_diagnostic_value(nc, ns(NamedDiagnosticValue_value(t)), &m->value);
}

static void _decode_data_value(
    DiagCodecInstance* nc, ns(DataValue_table_t) t, NCodecDiagXilDataValue* m)
{
    m->path = ns(DataValue_path(t));
    m->raw_value = ns(DataValue_raw_value(t));
    m->raw_value_len = flatbuffers_uint8_vec_len(m->raw_value);
    {
        ns(DiagnosticValue_table_t) v = ns(DataValue_physical_value(t));
        if (v) {
            NCodecDiagXilDiagnosticValue* data =
                diag_alloc(nc, 1, sizeof(NCodecDiagXilDiagnosticValue));
            _decode_diagnostic_value(nc, v, data);
            m->physical_value = data;
        }
    }
}

static void _decode_did_selector(
    ns(DidSelector_table_t) t, NCodecDiagXilDidSelector* m)
{
    m->kind = (NCodecDiagXilDidSelectorKind)ns(DidSelector_kind(t));
    m->identifier = ns(DidSelector_did_identifier(t));
    m->name = ns(DidSelector_name(t));
}

static void _decode_did_identity(
    ns(DidIdentity_table_t) t, NCodecDiagXilDidIdentity* m)
{
    m->name = ns(DidIdentity_name(t));
    m->identifier = ns(DidIdentity_did_identifier(t));
}

static void _decode_did_value(
    DiagCodecInstance* nc, ns(DidValue_table_t) t, NCodecDiagXilDidValue* m)
{
    _decode_did_selector(ns(DidValue_did(t)), &m->did);
    m->raw_value = ns(DidValue_raw_value(t));
    m->raw_value_len = flatbuffers_uint8_vec_len(m->raw_value);
    {
        ns(DataValue_vec_t) v = ns(DidValue_values(t));
        if (v) {
            size_t                  len = ns(DataValue_vec_len(v));
            NCodecDiagXilDataValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDataValue));
            for (size_t i = 0; i < len; i++) {
                _decode_data_value(nc, ns(DataValue_vec_at(v, i)), &data[i]);
            }
            m->values = data;
            m->values_len = len;
        }
    }
    {
        ns(DidIdentity_table_t) v = ns(DidValue_identity(t));
        if (v) {
            NCodecDiagXilDidIdentity* data =
                diag_alloc(nc, 1, sizeof(NCodecDiagXilDidIdentity));
            _decode_did_identity(v, data);
            m->identity = data;
        }
    }
}

static void _decode_diagnostic_trouble_code(DiagCodecInstance* nc,
    ns(DiagnosticTroubleCode_table_t) t, NCodecDiagXilDiagnosticTroubleCode* m)
{
    m->value = ns(DiagnosticTroubleCode_value(t));
    m->status = ns(DiagnosticTroubleCode_status(t));
    m->short_name = ns(DiagnosticTroubleCode_short_name(t));
    m->long_name = ns(DiagnosticTroubleCode_long_name(t));
    m->description = ns(DiagnosticTroubleCode_description(t));
    {
        ns(DataValue_vec_t) v = ns(DiagnosticTroubleCode_environment_data(t));
        if (v) {
            size_t                  len = ns(DataValue_vec_len(v));
            NCodecDiagXilDataValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDataValue));
            for (size_t i = 0; i < len; i++) {
                _decode_data_value(nc, ns(DataValue_vec_at(v, i)), &data[i]);
            }
            m->environment_data = data;
            m->environment_data_len = len;
        }
    }
}

static void _decode_execute_job_call(DiagCodecInstance* nc,
    ns(ExecuteJobCall_table_t) t, NCodecDiagXilExecuteJobCall* m)
{
    m->job_name = ns(ExecuteJobCall_job_name(t));
    {
        ns(NamedDiagnosticValue_vec_t) v = ns(ExecuteJobCall_parameters(t));
        if (v) {
            size_t len = ns(NamedDiagnosticValue_vec_len(v));
            NCodecDiagXilNamedDiagnosticValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilNamedDiagnosticValue));
            for (size_t i = 0; i < len; i++) {
                _decode_named_diagnostic_value(
                    nc, ns(NamedDiagnosticValue_vec_at(v, i)), &data[i]);
            }
            m->parameters = data;
            m->parameters_len = len;
        }
    }
}

static void _decode_execute_job_return(DiagCodecInstance* nc,
    ns(ExecuteJobReturn_table_t) t, NCodecDiagXilExecuteJobReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(ExecuteJobReturn_status(t));
    {
        ns(DataValue_vec_t) v = ns(ExecuteJobReturn_values(t));
        if (v) {
            size_t                  len = ns(DataValue_vec_len(v));
            NCodecDiagXilDataValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDataValue));
            for (size_t i = 0; i < len; i++) {
                _decode_data_value(nc, ns(DataValue_vec_at(v, i)), &data[i]);
            }
            m->values = data;
            m->values_len = len;
        }
    }
    m->info = ns(ExecuteJobReturn_info(t));
}

static void _decode_send_hex_service_call(
    ns(SendHexServiceCall_table_t) t, NCodecDiagXilSendHexServiceCall* m)
{
    m->pdu = ns(SendHexServiceCall_pdu(t));
    m->pdu_len = flatbuffers_uint8_vec_len(m->pdu);
    m->response_required = ns(SendHexServiceCall_response_required(t));
}

static void _decode_send_hex_service_return(
    ns(SendHexServiceReturn_table_t) t, NCodecDiagXilSendHexServiceReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(SendHexServiceReturn_status(t));
    m->response_pdu = ns(SendHexServiceReturn_response_pdu(t));
    m->response_pdu_len = flatbuffers_uint8_vec_len(m->response_pdu);
    m->info = ns(SendHexServiceReturn_info(t));
}

static void _decode_read_from_address_call(
    ns(ReadFromAddressCall_table_t) t, NCodecDiagXilReadFromAddressCall* m)
{
    m->address = ns(ReadFromAddressCall_address(t));
    m->number_of_bytes = ns(ReadFromAddressCall_number_of_bytes(t));
}

static void _decode_read_from_address_return(
    ns(ReadFromAddressReturn_table_t) t, NCodecDiagXilReadFromAddressReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(ReadFromAddressReturn_status(t));
    m->data = ns(ReadFromAddressReturn_data(t));
    m->data_len = flatbuffers_uint8_vec_len(m->data);
    m->info = ns(ReadFromAddressReturn_info(t));
}

static void _decode_write_to_address_call(
    ns(WriteToAddressCall_table_t) t, NCodecDiagXilWriteToAddressCall* m)
{
    m->address = ns(WriteToAddressCall_address(t));
    m->data = ns(WriteToAddressCall_data(t));
    m->data_len = flatbuffers_uint8_vec_len(m->data);
}

static void _decode_write_to_address_return(
    ns(WriteToAddressReturn_table_t) t, NCodecDiagXilWriteToAddressReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(WriteToAddressReturn_status(t));
    m->info = ns(WriteToAddressReturn_info(t));
}

static void _decode_read_did_value_call(
    ns(ReadDIDValueCall_table_t) t, NCodecDiagXilReadDIDValueCall* m)
{
    _decode_did_selector(ns(ReadDIDValueCall_did(t)), &m->did);
}

static void _decode_read_did_value_return(DiagCodecInstance* nc,
    ns(ReadDIDValueReturn_table_t) t, NCodecDiagXilReadDIDValueReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(ReadDIDValueReturn_status(t));
    m->negative_response_code =
        ns(ReadDIDValueReturn_negative_response_code(t));
    {
        ns(DidValue_table_t) v = ns(ReadDIDValueReturn_value(t));
        if (v) {
            NCodecDiagXilDidValue* data =
                diag_alloc(nc, 1, sizeof(NCodecDiagXilDidValue));
            _decode_did_value(nc, v, data);
            m->value = data;
        }
    }
    m->info = ns(ReadDIDValueReturn_info(t));
}

static void _decode_write_did_value_call(DiagCodecInstance* nc,
    ns(WriteDIDValueCall_table_t) t, NCodecDiagXilWriteDIDValueCall* m)
{
    _decode_did_value(nc, ns(WriteDIDValueCall_value(t)), &m->value);
}

static void _decode_write_did_value_return(
    ns(WriteDIDValueReturn_table_t) t, NCodecDiagXilWriteDIDValueReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(WriteDIDValueReturn_status(t));
    m->negative_response_code =
        ns(WriteDIDValueReturn_negative_response_code(t));
    m->info = ns(WriteDIDValueReturn_info(t));
}

static void _decode_get_identification_data_return(DiagCodecInstance* nc,
    ns(GetIdentificationDataReturn_table_t) t,
    NCodecDiagXilGetIdentificationDataReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(GetIdentificationDataReturn_status(t));
    {
        ns(DidIdentity_vec_t) v =
            ns(GetIdentificationDataReturn_identifiers(t));
        if (v) {
            size_t                    len = ns(DidIdentity_vec_len(v));
            NCodecDiagXilDidIdentity* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDidIdentity));
            for (size_t i = 0; i < len; i++) {
                _decode_did_identity(ns(DidIdentity_vec_at(v, i)), &data[i]);
            }
            m->identifiers = data;
            m->identifiers_len = len;
        }
    }
    m->info = ns(GetIdentificationDataReturn_info(t));
}

static void _decode_get_variant_data_return(DiagCodecInstance* nc,
    ns(GetVariantDataReturn_table_t) t, NCodecDiagXilGetVariantDataReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(GetVariantDataReturn_status(t));
    {
        ns(DidValue_vec_t) v = ns(GetVariantDataReturn_values(t));
        if (v) {
            size_t                 len = ns(DidValue_vec_len(v));
            NCodecDiagXilDidValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDidValue));
            for (size_t i = 0; i < len; i++) {
                _decode_did_value(nc, ns(DidValue_vec_at(v, i)), &data[i]);
            }
            m->values = data;
            m->values_len = len;
        }
    }
    m->info = ns(GetVariantDataReturn_info(t));
}

static void _decode_set_variant_data_call(DiagCodecInstance* nc,
    ns(SetVariantDataCall_table_t) t, NCodecDiagXilSetVariantDataCall* m)
{
    {
        ns(DidValue_vec_t) v = ns(SetVariantDataCall_values(t));
        if (v) {
            size_t                 len = ns(DidValue_vec_len(v));
            NCodecDiagXilDidValue* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDidValue));
            for (size_t i = 0; i < len; i++) {
                _decode_did_value(nc, ns(DidValue_vec_at(v, i)), &data[i]);
            }
            m->values = data;
            m->values_len = len;
        }
    }
}

static void _decode_set_variant_data_return(
    ns(SetVariantDataReturn_table_t) t, NCodecDiagXilSetVariantDataReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(SetVariantDataReturn_status(t));
    m->info = ns(SetVariantDataReturn_info(t));
}

static void _decode_read_fault_memory_call(
    ns(ReadFaultMemoryCall_table_t) t, NCodecDiagXilReadFaultMemoryCall* m)
{
    m->filter = (NCodecDiagXilDtcFilterType)ns(ReadFaultMemoryCall_filter(t));
}

static void _decode_read_fault_memory_return(DiagCodecInstance* nc,
    ns(ReadFaultMemoryReturn_table_t) t, NCodecDiagXilReadFaultMemoryReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(ReadFaultMemoryReturn_status(t));
    {
        ns(DiagnosticTroubleCode_vec_t) v =
            ns(ReadFaultMemoryReturn_trouble_codes(t));
        if (v) {
            size_t len = ns(DiagnosticTroubleCode_vec_len(v));
            NCodecDiagXilDiagnosticTroubleCode* data =
                diag_alloc(nc, len, sizeof(NCodecDiagXilDiagnosticTroubleCode));
            for (size_t i = 0; i < len; i++) {
                _decode_diagnostic_trouble_code(
                    nc, ns(DiagnosticTroubleCode_vec_at(v, i)), &data[i]);
            }
            m->trouble_codes = data;
            m->trouble_codes_len = len;
        }
    }
    m->info = ns(ReadFaultMemoryReturn_info(t));
}

static void _decode_clear_fault_memory_return(ns(ClearFaultMemoryReturn_table_t)
                                                  t,
    NCodecDiagXilClearFaultMemoryReturn* m)
{
    m->status = (NCodecDiagXilStatus)ns(ClearFaultMemoryReturn_status(t));
    m->info = ns(ClearFaultMemoryReturn_info(t));
}


static uint32_t _emit_message(flatcc_builder_t* B,
    const NCodecDiagXilMessage* m, const char* ecu_name, uint32_t node_uid)
{
    if (ecu_name == NULL) return 0;

    ns(XilRpcMessage_start_as_root_with_size(B));
    ns(XilRpcMessage_call_id_add(B, m->call_id));
    switch (m->payload_type) {
    case NCodecDiagXilMethodPayloadTypeExecuteJobCall: {
        uint32_t ref = _emit_execute_job_call(B, &m->payload.execute_job_call);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_ExecuteJobCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeExecuteJobReturn: {
        uint32_t ref =
            _emit_execute_job_return(B, &m->payload.execute_job_return);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_ExecuteJobReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeSendHexServiceCall: {
        uint32_t ref =
            _emit_send_hex_service_call(B, &m->payload.send_hex_service_call);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_SendHexServiceCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeSendHexServiceReturn: {
        uint32_t ref = _emit_send_hex_service_return(
            B, &m->payload.send_hex_service_return);
        ns(XilRpcMessage_payload_SendHexServiceReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeReadDIDValueCall: {
        uint32_t ref =
            _emit_read_did_value_call(B, &m->payload.read_did_value_call);
        ns(XilRpcMessage_payload_ReadDIDValueCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeReadDIDValueReturn: {
        uint32_t ref =
            _emit_read_did_value_return(B, &m->payload.read_did_value_return);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_ReadDIDValueReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeWriteDIDValueCall: {
        uint32_t ref =
            _emit_write_did_value_call(B, &m->payload.write_did_value_call);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_WriteDIDValueCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeWriteDIDValueReturn: {
        uint32_t ref =
            _emit_write_did_value_return(B, &m->payload.write_did_value_return);
        ns(XilRpcMessage_payload_WriteDIDValueReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeReadFaultMemoryCall: {
        uint32_t ref =
            _emit_read_fault_memory_call(B, &m->payload.read_fault_memory_call);
        ns(XilRpcMessage_payload_ReadFaultMemoryCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeReadFaultMemoryReturn: {
        uint32_t ref = _emit_read_fault_memory_return(
            B, &m->payload.read_fault_memory_return);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_ReadFaultMemoryReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeClearFaultMemoryCall: {
        ns(ClearFaultMemoryCall_start(B));
        ns(XilRpcMessage_payload_ClearFaultMemoryCall_add(
            B, ns(ClearFaultMemoryCall_end(B))));
    } break;
    case NCodecDiagXilMethodPayloadTypeClearFaultMemoryReturn: {
        uint32_t ref = _emit_clear_fault_memory_return(
            B, &m->payload.clear_fault_memory_return);
        ns(XilRpcMessage_payload_ClearFaultMemoryReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeReadFromAddressCall: {
        uint32_t ref =
            _emit_read_from_address_call(B, &m->payload.read_from_address_call);
        ns(XilRpcMessage_payload_ReadFromAddressCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeReadFromAddressReturn: {
        uint32_t ref = _emit_read_from_address_return(
            B, &m->payload.read_from_address_return);
        ns(XilRpcMessage_payload_ReadFromAddressReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeWriteToAddressCall: {
        uint32_t ref =
            _emit_write_to_address_call(B, &m->payload.write_to_address_call);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_WriteToAddressCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeWriteToAddressReturn: {
        uint32_t ref = _emit_write_to_address_return(
            B, &m->payload.write_to_address_return);
        ns(XilRpcMessage_payload_WriteToAddressReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeGetIdentificationDataCall: {
        ns(GetIdentificationDataCall_start(B));
        ns(XilRpcMessage_payload_GetIdentificationDataCall_add(
            B, ns(GetIdentificationDataCall_end(B))));
    } break;
    case NCodecDiagXilMethodPayloadTypeGetIdentificationDataReturn: {
        uint32_t ref = _emit_get_identification_data_return(
            B, &m->payload.get_identification_data_return);
        ns(XilRpcMessage_payload_GetIdentificationDataReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeGetVariantDataCall: {
        ns(GetVariantDataCall_start(B));
        ns(XilRpcMessage_payload_GetVariantDataCall_add(
            B, ns(GetVariantDataCall_end(B))));
    } break;
    case NCodecDiagXilMethodPayloadTypeGetVariantDataReturn: {
        uint32_t ref = _emit_get_variant_data_return(
            B, &m->payload.get_variant_data_return);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_GetVariantDataReturn_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeSetVariantDataCall: {
        uint32_t ref =
            _emit_set_variant_data_call(B, &m->payload.set_variant_data_call);
        if (ref == 0) return 0;
        ns(XilRpcMessage_payload_SetVariantDataCall_add(B, ref));
    } break;
    case NCodecDiagXilMethodPayloadTypeSetVariantDataReturn: {
        uint32_t ref = _emit_set_variant_data_return(
            B, &m->payload.set_variant_data_return);
        ns(XilRpcMessage_payload_SetVariantDataReturn_add(B, ref));
    } break;
    default:
        break;
    }
    ns(XilRpcMessage_ecu_name_add(
        B, flatbuffers_string_create_str(B, ecu_name)));
    if (m->logical_link_name) {
        ns(XilRpcMessage_logical_link_name_add(
            B, flatbuffers_string_create_str(B, m->logical_link_name)));
    }
    if (m->ecu_information) {
        ns(XilRpcMessage_ecu_information_add(
            B, flatbuffers_string_create_str(B, m->ecu_information)));
    }
    ns(XilRpcMessage_node_uid_add(B, node_uid));
    ns(XilRpcMessage_simulation_time_add(B, m->simulation_time));
    ns(XilRpcMessage_utc_time_add(B, m->utc_time));
    return ns(XilRpcMessage_end_as_root(B));
}


static void _decode_message(
    DiagCodecInstance* nc, ns(XilRpcMessage_table_t) t, NCodecDiagXilMessage* m)
{
    m->call_id = ns(XilRpcMessage_call_id(t));
    switch (ns(XilRpcMessage_payload_type(t))) {
    case ns(MethodPayload_ExecuteJobCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeExecuteJobCall;
        _decode_execute_job_call(nc,
            (ns(ExecuteJobCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.execute_job_call);
        break;
    case ns(MethodPayload_ExecuteJobReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeExecuteJobReturn;
        _decode_execute_job_return(nc,
            (ns(ExecuteJobReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.execute_job_return);
        break;
    case ns(MethodPayload_SendHexServiceCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeSendHexServiceCall;
        _decode_send_hex_service_call(
            (ns(SendHexServiceCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.send_hex_service_call);
        break;
    case ns(MethodPayload_SendHexServiceReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeSendHexServiceReturn;
        _decode_send_hex_service_return(
            (ns(SendHexServiceReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.send_hex_service_return);
        break;
    case ns(MethodPayload_ReadDIDValueCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeReadDIDValueCall;
        _decode_read_did_value_call(
            (ns(ReadDIDValueCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.read_did_value_call);
        break;
    case ns(MethodPayload_ReadDIDValueReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeReadDIDValueReturn;
        _decode_read_did_value_return(nc,
            (ns(ReadDIDValueReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.read_did_value_return);
        break;
    case ns(MethodPayload_WriteDIDValueCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeWriteDIDValueCall;
        _decode_write_did_value_call(nc,
            (ns(WriteDIDValueCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.write_did_value_call);
        break;
    case ns(MethodPayload_WriteDIDValueReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeWriteDIDValueReturn;
        _decode_write_did_value_return(
            (ns(WriteDIDValueReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.write_did_value_return);
        break;
    case ns(MethodPayload_ReadFaultMemoryCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeReadFaultMemoryCall;
        _decode_read_fault_memory_call(
            (ns(ReadFaultMemoryCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.read_fault_memory_call);
        break;
    case ns(MethodPayload_ReadFaultMemoryReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeReadFaultMemoryReturn;
        _decode_read_fault_memory_return(nc,
            (ns(ReadFaultMemoryReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.read_fault_memory_return);
        break;
    case ns(MethodPayload_ClearFaultMemoryCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeClearFaultMemoryCall;
        break;
    case ns(MethodPayload_ClearFaultMemoryReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeClearFaultMemoryReturn;
        _decode_clear_fault_memory_return(
            (ns(ClearFaultMemoryReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.clear_fault_memory_return);
        break;
    case ns(MethodPayload_ReadFromAddressCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeReadFromAddressCall;
        _decode_read_from_address_call(
            (ns(ReadFromAddressCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.read_from_address_call);
        break;
    case ns(MethodPayload_ReadFromAddressReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeReadFromAddressReturn;
        _decode_read_from_address_return(
            (ns(ReadFromAddressReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.read_from_address_return);
        break;
    case ns(MethodPayload_WriteToAddressCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeWriteToAddressCall;
        _decode_write_to_address_call(
            (ns(WriteToAddressCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.write_to_address_call);
        break;
    case ns(MethodPayload_WriteToAddressReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeWriteToAddressReturn;
        _decode_write_to_address_return(
            (ns(WriteToAddressReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.write_to_address_return);
        break;
    case ns(MethodPayload_GetIdentificationDataCall):
        m->payload_type =
            NCodecDiagXilMethodPayloadTypeGetIdentificationDataCall;
        break;
    case ns(MethodPayload_GetIdentificationDataReturn):
        m->payload_type =
            NCodecDiagXilMethodPayloadTypeGetIdentificationDataReturn;
        _decode_get_identification_data_return(nc,
            (ns(GetIdentificationDataReturn_table_t))ns(
                XilRpcMessage_payload(t)),
            &m->payload.get_identification_data_return);
        break;
    case ns(MethodPayload_GetVariantDataCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeGetVariantDataCall;
        break;
    case ns(MethodPayload_GetVariantDataReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeGetVariantDataReturn;
        _decode_get_variant_data_return(nc,
            (ns(GetVariantDataReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.get_variant_data_return);
        break;
    case ns(MethodPayload_SetVariantDataCall):
        m->payload_type = NCodecDiagXilMethodPayloadTypeSetVariantDataCall;
        _decode_set_variant_data_call(nc,
            (ns(SetVariantDataCall_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.set_variant_data_call);
        break;
    case ns(MethodPayload_SetVariantDataReturn):
        m->payload_type = NCodecDiagXilMethodPayloadTypeSetVariantDataReturn;
        _decode_set_variant_data_return(
            (ns(SetVariantDataReturn_table_t))ns(XilRpcMessage_payload(t)),
            &m->payload.set_variant_data_return);
        break;
    default:
        break;
    }
    m->ecu_name = ns(XilRpcMessage_ecu_name(t));
    m->logical_link_name = ns(XilRpcMessage_logical_link_name(t));
    m->ecu_information = ns(XilRpcMessage_ecu_information(t));
    m->node_uid = ns(XilRpcMessage_node_uid(t));
    m->simulation_time = ns(XilRpcMessage_simulation_time(t));
    m->utc_time = ns(XilRpcMessage_utc_time(t));
}


int32_t xil_write(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*    _nc = (DiagCodecInstance*)nc;
    NCodecDiagXilMessage* _msg = (NCodecDiagXilMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    const char* ecu_name = _msg->ecu_name ? _msg->ecu_name : _nc->name;
    uint32_t    node_uid = _msg->node_uid ? _msg->node_uid : _nc->node_id;

    flatcc_builder_t* B = &_nc->fbs_builder;
    flatcc_builder_reset(B);
    if (_emit_message(B, _msg, ecu_name, node_uid) == 0) {
        flatcc_builder_reset(B);
        log_error(_nc, "XiL message is missing a required field");
        return -EINVAL;
    }
    return diag_stream_write(_nc);
}


int32_t xil_read(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*    _nc = (DiagCodecInstance*)nc;
    NCodecDiagXilMessage* _msg = (NCodecDiagXilMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    /* Reset the message, in case caller ignores the return value. */
    *_msg = (NCodecDiagXilMessage){};

    size_t   length = 0;
    uint8_t* buffer = NULL;
    while ((buffer = diag_stream_read(_nc, flatbuffers_identifier, &length))) {
        ns(XilRpcMessage_table_t) t = ns(XilRpcMessage_as_root(buffer));

        /* Filter: sender==receiver. */
        if ((_nc->node_id) && (_nc->node_id == ns(XilRpcMessage_node_uid(t)))) {
            if (_nc->loopback == false) continue;
        }

        _decode_message(_nc, t, _msg);
        return length;
    }
    return -ENOMSG;
}
