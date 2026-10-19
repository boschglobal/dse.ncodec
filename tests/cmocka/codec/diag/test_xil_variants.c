// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

/* Generated from the public header: one message per union variant
   with every field populated, and deep-compare helpers. */

#include <dse/testing.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <dse/ncodec/interface/diag/xil.h>
#include <tests/cmocka/codec/diag/test_diag.h>


static void assert_diagnostic_property_equal(
    const NCodecDiagXilDiagnosticProperty* a,
    const NCodecDiagXilDiagnosticProperty* b);
static void assert_diagnostic_value_equal(const NCodecDiagXilDiagnosticValue* a,
    const NCodecDiagXilDiagnosticValue* b);
static void assert_diagnostic_series_equal(
    const NCodecDiagXilDiagnosticSeries* a,
    const NCodecDiagXilDiagnosticSeries* b);
static void assert_named_diagnostic_value_equal(
    const NCodecDiagXilNamedDiagnosticValue* a,
    const NCodecDiagXilNamedDiagnosticValue* b);
static void assert_data_value_equal(
    const NCodecDiagXilDataValue* a, const NCodecDiagXilDataValue* b);
static void assert_did_selector_equal(
    const NCodecDiagXilDidSelector* a, const NCodecDiagXilDidSelector* b);
static void assert_did_identity_equal(
    const NCodecDiagXilDidIdentity* a, const NCodecDiagXilDidIdentity* b);
static void assert_did_value_equal(
    const NCodecDiagXilDidValue* a, const NCodecDiagXilDidValue* b);
static void assert_diagnostic_trouble_code_equal(
    const NCodecDiagXilDiagnosticTroubleCode* a,
    const NCodecDiagXilDiagnosticTroubleCode* b);
static void assert_execute_job_call_equal(
    const NCodecDiagXilExecuteJobCall* a, const NCodecDiagXilExecuteJobCall* b);
static void assert_execute_job_return_equal(
    const NCodecDiagXilExecuteJobReturn* a,
    const NCodecDiagXilExecuteJobReturn* b);
static void assert_send_hex_service_call_equal(
    const NCodecDiagXilSendHexServiceCall* a,
    const NCodecDiagXilSendHexServiceCall* b);
static void assert_send_hex_service_return_equal(
    const NCodecDiagXilSendHexServiceReturn* a,
    const NCodecDiagXilSendHexServiceReturn* b);
static void assert_read_from_address_call_equal(
    const NCodecDiagXilReadFromAddressCall* a,
    const NCodecDiagXilReadFromAddressCall* b);
static void assert_read_from_address_return_equal(
    const NCodecDiagXilReadFromAddressReturn* a,
    const NCodecDiagXilReadFromAddressReturn* b);
static void assert_write_to_address_call_equal(
    const NCodecDiagXilWriteToAddressCall* a,
    const NCodecDiagXilWriteToAddressCall* b);
static void assert_write_to_address_return_equal(
    const NCodecDiagXilWriteToAddressReturn* a,
    const NCodecDiagXilWriteToAddressReturn* b);
static void assert_read_did_value_call_equal(
    const NCodecDiagXilReadDIDValueCall* a,
    const NCodecDiagXilReadDIDValueCall* b);
static void assert_read_did_value_return_equal(
    const NCodecDiagXilReadDIDValueReturn* a,
    const NCodecDiagXilReadDIDValueReturn* b);
static void assert_write_did_value_call_equal(
    const NCodecDiagXilWriteDIDValueCall* a,
    const NCodecDiagXilWriteDIDValueCall* b);
static void assert_write_did_value_return_equal(
    const NCodecDiagXilWriteDIDValueReturn* a,
    const NCodecDiagXilWriteDIDValueReturn* b);
static void assert_get_identification_data_return_equal(
    const NCodecDiagXilGetIdentificationDataReturn* a,
    const NCodecDiagXilGetIdentificationDataReturn* b);
static void assert_get_variant_data_return_equal(
    const NCodecDiagXilGetVariantDataReturn* a,
    const NCodecDiagXilGetVariantDataReturn* b);
static void assert_set_variant_data_call_equal(
    const NCodecDiagXilSetVariantDataCall* a,
    const NCodecDiagXilSetVariantDataCall* b);
static void assert_set_variant_data_return_equal(
    const NCodecDiagXilSetVariantDataReturn* a,
    const NCodecDiagXilSetVariantDataReturn* b);
static void assert_read_fault_memory_call_equal(
    const NCodecDiagXilReadFaultMemoryCall* a,
    const NCodecDiagXilReadFaultMemoryCall* b);
static void assert_read_fault_memory_return_equal(
    const NCodecDiagXilReadFaultMemoryReturn* a,
    const NCodecDiagXilReadFaultMemoryReturn* b);
static void assert_clear_fault_memory_return_equal(
    const NCodecDiagXilClearFaultMemoryReturn* a,
    const NCodecDiagXilClearFaultMemoryReturn* b);

static void assert_diagnostic_property_equal(
    const NCodecDiagXilDiagnosticProperty* a,
    const NCodecDiagXilDiagnosticProperty* b)
{
    assert_string_or_null(a->name, b->name);
    assert_string_or_null(a->value, b->value);
}

static void assert_diagnostic_value_equal(const NCodecDiagXilDiagnosticValue* a,
    const NCodecDiagXilDiagnosticValue*                                       b)
{
    assert_string_or_null(a->name, b->name);
    assert_int_equal(a->kind, b->kind);
    assert_int_equal(a->boolean_value, b->boolean_value);
    assert_int_equal(a->signed_value, b->signed_value);
    assert_int_equal(a->unsigned_value, b->unsigned_value);
    assert_true(a->float_value == b->float_value);
    assert_string_or_null(a->string_value, b->string_value);
    assert_vector_or_null(
        a->bytes_value, a->bytes_value_len, b->bytes_value, b->bytes_value_len);
    assert_true((a->elements == NULL) == (b->elements == NULL));
    assert_int_equal(a->elements_len, b->elements_len);
    for (size_t i = 0; i < a->elements_len; i++) {
        assert_diagnostic_value_equal(&a->elements[i], &b->elements[i]);
    }
    assert_int_equal(a->row_count, b->row_count);
    assert_int_equal(a->column_count, b->column_count);
    assert_vector_or_null(
        a->x_values, a->x_values_len, b->x_values, b->x_values_len);
    assert_vector_or_null(
        a->y_values, a->y_values_len, b->y_values, b->y_values_len);
    assert_true((a->series == NULL) == (b->series == NULL));
    assert_int_equal(a->series_len, b->series_len);
    for (size_t i = 0; i < a->series_len; i++) {
        assert_diagnostic_series_equal(&a->series[i], &b->series[i]);
    }
    assert_string_or_null(a->unit, b->unit);
    assert_string_or_null(a->description, b->description);
    assert_true((a->properties == NULL) == (b->properties == NULL));
    assert_int_equal(a->properties_len, b->properties_len);
    for (size_t i = 0; i < a->properties_len; i++) {
        assert_diagnostic_property_equal(&a->properties[i], &b->properties[i]);
    }
}

static void assert_diagnostic_series_equal(
    const NCodecDiagXilDiagnosticSeries* a,
    const NCodecDiagXilDiagnosticSeries* b)
{
    assert_string_or_null(a->name, b->name);
    assert_true((a->values == NULL) == (b->values == NULL));
    assert_int_equal(a->values_len, b->values_len);
    for (size_t i = 0; i < a->values_len; i++) {
        assert_diagnostic_value_equal(&a->values[i], &b->values[i]);
    }
}

static void assert_named_diagnostic_value_equal(
    const NCodecDiagXilNamedDiagnosticValue* a,
    const NCodecDiagXilNamedDiagnosticValue* b)
{
    assert_string_or_null(a->name, b->name);
    assert_diagnostic_value_equal(&a->value, &b->value);
}

static void assert_data_value_equal(
    const NCodecDiagXilDataValue* a, const NCodecDiagXilDataValue* b)
{
    assert_string_or_null(a->path, b->path);
    assert_vector_or_null(
        a->raw_value, a->raw_value_len, b->raw_value, b->raw_value_len);
    if (a->physical_value == NULL) {
        assert_null(b->physical_value);
    } else {
        assert_non_null(b->physical_value);
        assert_diagnostic_value_equal(a->physical_value, b->physical_value);
    }
}

static void assert_did_selector_equal(
    const NCodecDiagXilDidSelector* a, const NCodecDiagXilDidSelector* b)
{
    assert_int_equal(a->kind, b->kind);
    assert_int_equal(a->identifier, b->identifier);
    assert_string_or_null(a->name, b->name);
}

static void assert_did_identity_equal(
    const NCodecDiagXilDidIdentity* a, const NCodecDiagXilDidIdentity* b)
{
    assert_string_or_null(a->name, b->name);
    assert_int_equal(a->identifier, b->identifier);
}

static void assert_did_value_equal(
    const NCodecDiagXilDidValue* a, const NCodecDiagXilDidValue* b)
{
    assert_did_selector_equal(&a->did, &b->did);
    assert_vector_or_null(
        a->raw_value, a->raw_value_len, b->raw_value, b->raw_value_len);
    assert_true((a->values == NULL) == (b->values == NULL));
    assert_int_equal(a->values_len, b->values_len);
    for (size_t i = 0; i < a->values_len; i++) {
        assert_data_value_equal(&a->values[i], &b->values[i]);
    }
    if (a->identity == NULL) {
        assert_null(b->identity);
    } else {
        assert_non_null(b->identity);
        assert_did_identity_equal(a->identity, b->identity);
    }
}

static void assert_diagnostic_trouble_code_equal(
    const NCodecDiagXilDiagnosticTroubleCode* a,
    const NCodecDiagXilDiagnosticTroubleCode* b)
{
    assert_int_equal(a->value, b->value);
    assert_int_equal(a->status, b->status);
    assert_string_or_null(a->short_name, b->short_name);
    assert_string_or_null(a->long_name, b->long_name);
    assert_string_or_null(a->description, b->description);
    assert_true((a->environment_data == NULL) == (b->environment_data == NULL));
    assert_int_equal(a->environment_data_len, b->environment_data_len);
    for (size_t i = 0; i < a->environment_data_len; i++) {
        assert_data_value_equal(
            &a->environment_data[i], &b->environment_data[i]);
    }
}

static void assert_execute_job_call_equal(
    const NCodecDiagXilExecuteJobCall* a, const NCodecDiagXilExecuteJobCall* b)
{
    assert_string_or_null(a->job_name, b->job_name);
    assert_true((a->parameters == NULL) == (b->parameters == NULL));
    assert_int_equal(a->parameters_len, b->parameters_len);
    for (size_t i = 0; i < a->parameters_len; i++) {
        assert_named_diagnostic_value_equal(
            &a->parameters[i], &b->parameters[i]);
    }
}

static void assert_execute_job_return_equal(
    const NCodecDiagXilExecuteJobReturn* a,
    const NCodecDiagXilExecuteJobReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_true((a->values == NULL) == (b->values == NULL));
    assert_int_equal(a->values_len, b->values_len);
    for (size_t i = 0; i < a->values_len; i++) {
        assert_data_value_equal(&a->values[i], &b->values[i]);
    }
    assert_string_or_null(a->info, b->info);
}

static void assert_send_hex_service_call_equal(
    const NCodecDiagXilSendHexServiceCall* a,
    const NCodecDiagXilSendHexServiceCall* b)
{
    assert_vector_or_null(a->pdu, a->pdu_len, b->pdu, b->pdu_len);
    assert_int_equal(a->response_required, b->response_required);
}

static void assert_send_hex_service_return_equal(
    const NCodecDiagXilSendHexServiceReturn* a,
    const NCodecDiagXilSendHexServiceReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_vector_or_null(a->response_pdu, a->response_pdu_len, b->response_pdu,
        b->response_pdu_len);
    assert_string_or_null(a->info, b->info);
}

static void assert_read_from_address_call_equal(
    const NCodecDiagXilReadFromAddressCall* a,
    const NCodecDiagXilReadFromAddressCall* b)
{
    assert_int_equal(a->address, b->address);
    assert_int_equal(a->number_of_bytes, b->number_of_bytes);
}

static void assert_read_from_address_return_equal(
    const NCodecDiagXilReadFromAddressReturn* a,
    const NCodecDiagXilReadFromAddressReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_vector_or_null(a->data, a->data_len, b->data, b->data_len);
    assert_string_or_null(a->info, b->info);
}

static void assert_write_to_address_call_equal(
    const NCodecDiagXilWriteToAddressCall* a,
    const NCodecDiagXilWriteToAddressCall* b)
{
    assert_int_equal(a->address, b->address);
    assert_vector_or_null(a->data, a->data_len, b->data, b->data_len);
}

static void assert_write_to_address_return_equal(
    const NCodecDiagXilWriteToAddressReturn* a,
    const NCodecDiagXilWriteToAddressReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_string_or_null(a->info, b->info);
}

static void assert_read_did_value_call_equal(
    const NCodecDiagXilReadDIDValueCall* a,
    const NCodecDiagXilReadDIDValueCall* b)
{
    assert_did_selector_equal(&a->did, &b->did);
}

static void assert_read_did_value_return_equal(
    const NCodecDiagXilReadDIDValueReturn* a,
    const NCodecDiagXilReadDIDValueReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_int_equal(a->negative_response_code, b->negative_response_code);
    if (a->value == NULL) {
        assert_null(b->value);
    } else {
        assert_non_null(b->value);
        assert_did_value_equal(a->value, b->value);
    }
    assert_string_or_null(a->info, b->info);
}

static void assert_write_did_value_call_equal(
    const NCodecDiagXilWriteDIDValueCall* a,
    const NCodecDiagXilWriteDIDValueCall* b)
{
    assert_did_value_equal(&a->value, &b->value);
}

static void assert_write_did_value_return_equal(
    const NCodecDiagXilWriteDIDValueReturn* a,
    const NCodecDiagXilWriteDIDValueReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_int_equal(a->negative_response_code, b->negative_response_code);
    assert_string_or_null(a->info, b->info);
}

static void assert_get_identification_data_return_equal(
    const NCodecDiagXilGetIdentificationDataReturn* a,
    const NCodecDiagXilGetIdentificationDataReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_true((a->identifiers == NULL) == (b->identifiers == NULL));
    assert_int_equal(a->identifiers_len, b->identifiers_len);
    for (size_t i = 0; i < a->identifiers_len; i++) {
        assert_did_identity_equal(&a->identifiers[i], &b->identifiers[i]);
    }
    assert_string_or_null(a->info, b->info);
}

static void assert_get_variant_data_return_equal(
    const NCodecDiagXilGetVariantDataReturn* a,
    const NCodecDiagXilGetVariantDataReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_true((a->values == NULL) == (b->values == NULL));
    assert_int_equal(a->values_len, b->values_len);
    for (size_t i = 0; i < a->values_len; i++) {
        assert_did_value_equal(&a->values[i], &b->values[i]);
    }
    assert_string_or_null(a->info, b->info);
}

static void assert_set_variant_data_call_equal(
    const NCodecDiagXilSetVariantDataCall* a,
    const NCodecDiagXilSetVariantDataCall* b)
{
    assert_true((a->values == NULL) == (b->values == NULL));
    assert_int_equal(a->values_len, b->values_len);
    for (size_t i = 0; i < a->values_len; i++) {
        assert_did_value_equal(&a->values[i], &b->values[i]);
    }
}

static void assert_set_variant_data_return_equal(
    const NCodecDiagXilSetVariantDataReturn* a,
    const NCodecDiagXilSetVariantDataReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_string_or_null(a->info, b->info);
}

static void assert_read_fault_memory_call_equal(
    const NCodecDiagXilReadFaultMemoryCall* a,
    const NCodecDiagXilReadFaultMemoryCall* b)
{
    assert_int_equal(a->filter, b->filter);
}

static void assert_read_fault_memory_return_equal(
    const NCodecDiagXilReadFaultMemoryReturn* a,
    const NCodecDiagXilReadFaultMemoryReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_true((a->trouble_codes == NULL) == (b->trouble_codes == NULL));
    assert_int_equal(a->trouble_codes_len, b->trouble_codes_len);
    for (size_t i = 0; i < a->trouble_codes_len; i++) {
        assert_diagnostic_trouble_code_equal(
            &a->trouble_codes[i], &b->trouble_codes[i]);
    }
    assert_string_or_null(a->info, b->info);
}

static void assert_clear_fault_memory_return_equal(
    const NCodecDiagXilClearFaultMemoryReturn* a,
    const NCodecDiagXilClearFaultMemoryReturn* b)
{
    assert_int_equal(a->status, b->status);
    assert_string_or_null(a->info, b->info);
}


static const uint8_t                       _vec_1[] = { 5, 6, 7 };
static const uint8_t                       _vec_2[] = { 10, 11, 12 };
static const double                        _vec_3[] = { 13.5, 14.5, 15.5 };
static const double                        _vec_4[] = { 14.5, 15.5, 16.5 };
static const NCodecDiagXilDiagnosticSeries _vec_5[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_6[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_7[] = { 19, 20, 21 };
static const double                        _vec_8[] = { 22.5, 23.5, 24.5 };
static const double                        _vec_9[] = { 23.5, 24.5, 25.5 };
static const NCodecDiagXilDiagnosticSeries _vec_10[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_11[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_12[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1007),
        .unsigned_value = UINT64_C(4294967304),
        .float_value = 9.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_2,
        .bytes_value_len = ARRAY_SIZE(_vec_2),
        .row_count = 65547,
        .column_count = 65548,
        .x_values = _vec_3,
        .x_values_len = ARRAY_SIZE(_vec_3),
        .y_values = _vec_4,
        .y_values_len = ARRAY_SIZE(_vec_4),
        .series = _vec_5,
        .series_len = ARRAY_SIZE(_vec_5),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_6,
        .properties_len = ARRAY_SIZE(_vec_6),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1016),
        .unsigned_value = UINT64_C(4294967313),
        .float_value = 18.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_7,
        .bytes_value_len = ARRAY_SIZE(_vec_7),
        .row_count = 65556,
        .column_count = 65557,
        .x_values = _vec_8,
        .x_values_len = ARRAY_SIZE(_vec_8),
        .y_values = _vec_9,
        .y_values_len = ARRAY_SIZE(_vec_9),
        .series = _vec_10,
        .series_len = ARRAY_SIZE(_vec_10),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_11,
        .properties_len = ARRAY_SIZE(_vec_11),
    },
};
static const double                        _vec_13[] = { 26.5, 27.5, 28.5 };
static const double                        _vec_14[] = { 27.5, 28.5, 29.5 };
static const uint8_t                       _vec_15[] = { 32, 33, 34 };
static const double                        _vec_16[] = { 35.5, 36.5, 37.5 };
static const double                        _vec_17[] = { 36.5, 37.5, 38.5 };
static const NCodecDiagXilDiagnosticSeries _vec_18[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_19[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_20[] = { 41, 42, 43 };
static const double                        _vec_21[] = { 44.5, 45.5, 46.5 };
static const double                        _vec_22[] = { 45.5, 46.5, 47.5 };
static const NCodecDiagXilDiagnosticSeries _vec_23[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_24[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_25[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1029),
        .unsigned_value = UINT64_C(4294967326),
        .float_value = 31.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_15,
        .bytes_value_len = ARRAY_SIZE(_vec_15),
        .row_count = 65569,
        .column_count = 65570,
        .x_values = _vec_16,
        .x_values_len = ARRAY_SIZE(_vec_16),
        .y_values = _vec_17,
        .y_values_len = ARRAY_SIZE(_vec_17),
        .series = _vec_18,
        .series_len = ARRAY_SIZE(_vec_18),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_19,
        .properties_len = ARRAY_SIZE(_vec_19),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1038),
        .unsigned_value = UINT64_C(4294967335),
        .float_value = 40.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_20,
        .bytes_value_len = ARRAY_SIZE(_vec_20),
        .row_count = 65578,
        .column_count = 65579,
        .x_values = _vec_21,
        .x_values_len = ARRAY_SIZE(_vec_21),
        .y_values = _vec_22,
        .y_values_len = ARRAY_SIZE(_vec_22),
        .series = _vec_23,
        .series_len = ARRAY_SIZE(_vec_23),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_24,
        .properties_len = ARRAY_SIZE(_vec_24),
    },
};
static const uint8_t                       _vec_26[] = { 50, 51, 52 };
static const double                        _vec_27[] = { 53.5, 54.5, 55.5 };
static const double                        _vec_28[] = { 54.5, 55.5, 56.5 };
static const NCodecDiagXilDiagnosticSeries _vec_29[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_30[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_31[] = { 59, 60, 61 };
static const double                        _vec_32[] = { 62.5, 63.5, 64.5 };
static const double                        _vec_33[] = { 63.5, 64.5, 65.5 };
static const NCodecDiagXilDiagnosticSeries _vec_34[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_35[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_36[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1047),
        .unsigned_value = UINT64_C(4294967344),
        .float_value = 49.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_26,
        .bytes_value_len = ARRAY_SIZE(_vec_26),
        .row_count = 65587,
        .column_count = 65588,
        .x_values = _vec_27,
        .x_values_len = ARRAY_SIZE(_vec_27),
        .y_values = _vec_28,
        .y_values_len = ARRAY_SIZE(_vec_28),
        .series = _vec_29,
        .series_len = ARRAY_SIZE(_vec_29),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_30,
        .properties_len = ARRAY_SIZE(_vec_30),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1056),
        .unsigned_value = UINT64_C(4294967353),
        .float_value = 58.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_31,
        .bytes_value_len = ARRAY_SIZE(_vec_31),
        .row_count = 65596,
        .column_count = 65597,
        .x_values = _vec_32,
        .x_values_len = ARRAY_SIZE(_vec_32),
        .y_values = _vec_33,
        .y_values_len = ARRAY_SIZE(_vec_33),
        .series = _vec_34,
        .series_len = ARRAY_SIZE(_vec_34),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_35,
        .properties_len = ARRAY_SIZE(_vec_35),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_37[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_25,
        .values_len = ARRAY_SIZE(_vec_25),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_36,
        .values_len = ARRAY_SIZE(_vec_36),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_38[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_39[] = { 68, 69, 70 };
static const uint8_t                       _vec_40[] = { 73, 74, 75 };
static const double                        _vec_41[] = { 76.5, 77.5, 78.5 };
static const double                        _vec_42[] = { 77.5, 78.5, 79.5 };
static const NCodecDiagXilDiagnosticSeries _vec_43[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_44[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_45[] = { 82, 83, 84 };
static const double                        _vec_46[] = { 85.5, 86.5, 87.5 };
static const double                        _vec_47[] = { 86.5, 87.5, 88.5 };
static const NCodecDiagXilDiagnosticSeries _vec_48[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_49[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_50[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1070),
        .unsigned_value = UINT64_C(4294967367),
        .float_value = 72.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_40,
        .bytes_value_len = ARRAY_SIZE(_vec_40),
        .row_count = 65610,
        .column_count = 65611,
        .x_values = _vec_41,
        .x_values_len = ARRAY_SIZE(_vec_41),
        .y_values = _vec_42,
        .y_values_len = ARRAY_SIZE(_vec_42),
        .series = _vec_43,
        .series_len = ARRAY_SIZE(_vec_43),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_44,
        .properties_len = ARRAY_SIZE(_vec_44),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1079),
        .unsigned_value = UINT64_C(4294967376),
        .float_value = 81.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_45,
        .bytes_value_len = ARRAY_SIZE(_vec_45),
        .row_count = 65619,
        .column_count = 65620,
        .x_values = _vec_46,
        .x_values_len = ARRAY_SIZE(_vec_46),
        .y_values = _vec_47,
        .y_values_len = ARRAY_SIZE(_vec_47),
        .series = _vec_48,
        .series_len = ARRAY_SIZE(_vec_48),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_49,
        .properties_len = ARRAY_SIZE(_vec_49),
    },
};
static const double                        _vec_51[] = { 89.5, 90.5, 91.5 };
static const double                        _vec_52[] = { 90.5, 91.5, 92.5 };
static const uint8_t                       _vec_53[] = { 95, 96, 97 };
static const double                        _vec_54[] = { 98.5, 99.5, 100.5 };
static const double                        _vec_55[] = { 99.5, 100.5, 101.5 };
static const NCodecDiagXilDiagnosticSeries _vec_56[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_57[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_58[] = { 104, 105, 106 };
static const double                        _vec_59[] = { 107.5, 108.5, 109.5 };
static const double                        _vec_60[] = { 108.5, 109.5, 110.5 };
static const NCodecDiagXilDiagnosticSeries _vec_61[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_62[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_63[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1092),
        .unsigned_value = UINT64_C(4294967389),
        .float_value = 94.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_53,
        .bytes_value_len = ARRAY_SIZE(_vec_53),
        .row_count = 65632,
        .column_count = 65633,
        .x_values = _vec_54,
        .x_values_len = ARRAY_SIZE(_vec_54),
        .y_values = _vec_55,
        .y_values_len = ARRAY_SIZE(_vec_55),
        .series = _vec_56,
        .series_len = ARRAY_SIZE(_vec_56),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_57,
        .properties_len = ARRAY_SIZE(_vec_57),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1101),
        .unsigned_value = UINT64_C(4294967398),
        .float_value = 103.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_58,
        .bytes_value_len = ARRAY_SIZE(_vec_58),
        .row_count = 65641,
        .column_count = 65642,
        .x_values = _vec_59,
        .x_values_len = ARRAY_SIZE(_vec_59),
        .y_values = _vec_60,
        .y_values_len = ARRAY_SIZE(_vec_60),
        .series = _vec_61,
        .series_len = ARRAY_SIZE(_vec_61),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_62,
        .properties_len = ARRAY_SIZE(_vec_62),
    },
};
static const uint8_t                       _vec_64[] = { 113, 114, 115 };
static const double                        _vec_65[] = { 116.5, 117.5, 118.5 };
static const double                        _vec_66[] = { 117.5, 118.5, 119.5 };
static const NCodecDiagXilDiagnosticSeries _vec_67[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_68[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_69[] = { 122, 123, 124 };
static const double                        _vec_70[] = { 125.5, 126.5, 127.5 };
static const double                        _vec_71[] = { 126.5, 127.5, 128.5 };
static const NCodecDiagXilDiagnosticSeries _vec_72[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_73[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_74[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1110),
        .unsigned_value = UINT64_C(4294967407),
        .float_value = 112.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_64,
        .bytes_value_len = ARRAY_SIZE(_vec_64),
        .row_count = 65650,
        .column_count = 65651,
        .x_values = _vec_65,
        .x_values_len = ARRAY_SIZE(_vec_65),
        .y_values = _vec_66,
        .y_values_len = ARRAY_SIZE(_vec_66),
        .series = _vec_67,
        .series_len = ARRAY_SIZE(_vec_67),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_68,
        .properties_len = ARRAY_SIZE(_vec_68),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1119),
        .unsigned_value = UINT64_C(4294967416),
        .float_value = 121.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_69,
        .bytes_value_len = ARRAY_SIZE(_vec_69),
        .row_count = 65659,
        .column_count = 65660,
        .x_values = _vec_70,
        .x_values_len = ARRAY_SIZE(_vec_70),
        .y_values = _vec_71,
        .y_values_len = ARRAY_SIZE(_vec_71),
        .series = _vec_72,
        .series_len = ARRAY_SIZE(_vec_72),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_73,
        .properties_len = ARRAY_SIZE(_vec_73),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_75[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_63,
        .values_len = ARRAY_SIZE(_vec_63),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_74,
        .values_len = ARRAY_SIZE(_vec_74),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_76[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilNamedDiagnosticValue _vec_77[] = {
    {
        .name = "NamedDiagnosticValue.name",
        .value = {
            .name = "DiagnosticValue.name",
            .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
            .boolean_value = true,
            .signed_value = INT64_C(-1002),
            .unsigned_value = UINT64_C(4294967299),
            .float_value = 4.25,
            .string_value = "DiagnosticValue.string_value",
            .bytes_value = _vec_1,
            .bytes_value_len = ARRAY_SIZE(_vec_1),
            .elements = _vec_12,
            .elements_len = ARRAY_SIZE(_vec_12),
            .row_count = 65560,
            .column_count = 65561,
            .x_values = _vec_13,
            .x_values_len = ARRAY_SIZE(_vec_13),
            .y_values = _vec_14,
            .y_values_len = ARRAY_SIZE(_vec_14),
            .series = _vec_37,
            .series_len = ARRAY_SIZE(_vec_37),
            .unit = "DiagnosticValue.unit",
            .description = "DiagnosticValue.description",
            .properties = _vec_38,
            .properties_len = ARRAY_SIZE(_vec_38),
        },
    },
    {
        .name = "NamedDiagnosticValue.name",
        .value = {
            .name = "DiagnosticValue.name",
            .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
            .boolean_value = true,
            .signed_value = INT64_C(-1065),
            .unsigned_value = UINT64_C(4294967362),
            .float_value = 67.25,
            .string_value = "DiagnosticValue.string_value",
            .bytes_value = _vec_39,
            .bytes_value_len = ARRAY_SIZE(_vec_39),
            .elements = _vec_50,
            .elements_len = ARRAY_SIZE(_vec_50),
            .row_count = 65623,
            .column_count = 65624,
            .x_values = _vec_51,
            .x_values_len = ARRAY_SIZE(_vec_51),
            .y_values = _vec_52,
            .y_values_len = ARRAY_SIZE(_vec_52),
            .series = _vec_75,
            .series_len = ARRAY_SIZE(_vec_75),
            .unit = "DiagnosticValue.unit",
            .description = "DiagnosticValue.description",
            .properties = _vec_76,
            .properties_len = ARRAY_SIZE(_vec_76),
        },
    },
};
static const uint8_t                       _vec_78[] = { 127, 128, 129 };
static const uint8_t                       _vec_80[] = { 132, 133, 134 };
static const uint8_t                       _vec_81[] = { 137, 138, 139 };
static const double                        _vec_82[] = { 140.5, 141.5, 142.5 };
static const double                        _vec_83[] = { 141.5, 142.5, 143.5 };
static const NCodecDiagXilDiagnosticSeries _vec_84[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_85[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_86[] = { 146, 147, 148 };
static const double                        _vec_87[] = { 149.5, 150.5, 151.5 };
static const double                        _vec_88[] = { 150.5, 151.5, 152.5 };
static const NCodecDiagXilDiagnosticSeries _vec_89[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_90[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_91[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1134),
        .unsigned_value = UINT64_C(4294967431),
        .float_value = 136.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_81,
        .bytes_value_len = ARRAY_SIZE(_vec_81),
        .row_count = 65674,
        .column_count = 65675,
        .x_values = _vec_82,
        .x_values_len = ARRAY_SIZE(_vec_82),
        .y_values = _vec_83,
        .y_values_len = ARRAY_SIZE(_vec_83),
        .series = _vec_84,
        .series_len = ARRAY_SIZE(_vec_84),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_85,
        .properties_len = ARRAY_SIZE(_vec_85),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1143),
        .unsigned_value = UINT64_C(4294967440),
        .float_value = 145.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_86,
        .bytes_value_len = ARRAY_SIZE(_vec_86),
        .row_count = 65683,
        .column_count = 65684,
        .x_values = _vec_87,
        .x_values_len = ARRAY_SIZE(_vec_87),
        .y_values = _vec_88,
        .y_values_len = ARRAY_SIZE(_vec_88),
        .series = _vec_89,
        .series_len = ARRAY_SIZE(_vec_89),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_90,
        .properties_len = ARRAY_SIZE(_vec_90),
    },
};
static const double                        _vec_92[] = { 153.5, 154.5, 155.5 };
static const double                        _vec_93[] = { 154.5, 155.5, 156.5 };
static const uint8_t                       _vec_94[] = { 159, 160, 161 };
static const double                        _vec_95[] = { 162.5, 163.5, 164.5 };
static const double                        _vec_96[] = { 163.5, 164.5, 165.5 };
static const NCodecDiagXilDiagnosticSeries _vec_97[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_98[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_99[] = { 168, 169, 170 };
static const double                        _vec_100[] = { 171.5, 172.5, 173.5 };
static const double                        _vec_101[] = { 172.5, 173.5, 174.5 };
static const NCodecDiagXilDiagnosticSeries _vec_102[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_103[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_104[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1156),
        .unsigned_value = UINT64_C(4294967453),
        .float_value = 158.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_94,
        .bytes_value_len = ARRAY_SIZE(_vec_94),
        .row_count = 65696,
        .column_count = 65697,
        .x_values = _vec_95,
        .x_values_len = ARRAY_SIZE(_vec_95),
        .y_values = _vec_96,
        .y_values_len = ARRAY_SIZE(_vec_96),
        .series = _vec_97,
        .series_len = ARRAY_SIZE(_vec_97),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_98,
        .properties_len = ARRAY_SIZE(_vec_98),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1165),
        .unsigned_value = UINT64_C(4294967462),
        .float_value = 167.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_99,
        .bytes_value_len = ARRAY_SIZE(_vec_99),
        .row_count = 65705,
        .column_count = 65706,
        .x_values = _vec_100,
        .x_values_len = ARRAY_SIZE(_vec_100),
        .y_values = _vec_101,
        .y_values_len = ARRAY_SIZE(_vec_101),
        .series = _vec_102,
        .series_len = ARRAY_SIZE(_vec_102),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_103,
        .properties_len = ARRAY_SIZE(_vec_103),
    },
};
static const uint8_t                       _vec_105[] = { 177, 178, 179 };
static const double                        _vec_106[] = { 180.5, 181.5, 182.5 };
static const double                        _vec_107[] = { 181.5, 182.5, 183.5 };
static const NCodecDiagXilDiagnosticSeries _vec_108[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_109[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_110[] = { 186, 187, 188 };
static const double                        _vec_111[] = { 189.5, 190.5, 191.5 };
static const double                        _vec_112[] = { 190.5, 191.5, 192.5 };
static const NCodecDiagXilDiagnosticSeries _vec_113[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_114[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_115[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1174),
        .unsigned_value = UINT64_C(4294967471),
        .float_value = 176.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_105,
        .bytes_value_len = ARRAY_SIZE(_vec_105),
        .row_count = 65714,
        .column_count = 65715,
        .x_values = _vec_106,
        .x_values_len = ARRAY_SIZE(_vec_106),
        .y_values = _vec_107,
        .y_values_len = ARRAY_SIZE(_vec_107),
        .series = _vec_108,
        .series_len = ARRAY_SIZE(_vec_108),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_109,
        .properties_len = ARRAY_SIZE(_vec_109),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1183),
        .unsigned_value = UINT64_C(4294967480),
        .float_value = 185.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_110,
        .bytes_value_len = ARRAY_SIZE(_vec_110),
        .row_count = 65723,
        .column_count = 65724,
        .x_values = _vec_111,
        .x_values_len = ARRAY_SIZE(_vec_111),
        .y_values = _vec_112,
        .y_values_len = ARRAY_SIZE(_vec_112),
        .series = _vec_113,
        .series_len = ARRAY_SIZE(_vec_113),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_114,
        .properties_len = ARRAY_SIZE(_vec_114),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_116[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_104,
        .values_len = ARRAY_SIZE(_vec_104),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_115,
        .values_len = ARRAY_SIZE(_vec_115),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_117[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_79 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1129),
    .unsigned_value = UINT64_C(4294967426),
    .float_value = 131.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_80,
    .bytes_value_len = ARRAY_SIZE(_vec_80),
    .elements = _vec_91,
    .elements_len = ARRAY_SIZE(_vec_91),
    .row_count = 65687,
    .column_count = 65688,
    .x_values = _vec_92,
    .x_values_len = ARRAY_SIZE(_vec_92),
    .y_values = _vec_93,
    .y_values_len = ARRAY_SIZE(_vec_93),
    .series = _vec_116,
    .series_len = ARRAY_SIZE(_vec_116),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_117,
    .properties_len = ARRAY_SIZE(_vec_117),
};
static const uint8_t                       _vec_118[] = { 191, 192, 193 };
static const uint8_t                       _vec_120[] = { 196, 197, 198 };
static const uint8_t                       _vec_121[] = { 201, 202, 203 };
static const double                        _vec_122[] = { 204.5, 205.5, 206.5 };
static const double                        _vec_123[] = { 205.5, 206.5, 207.5 };
static const NCodecDiagXilDiagnosticSeries _vec_124[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_125[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_126[] = { 210, 211, 212 };
static const double                        _vec_127[] = { 213.5, 214.5, 215.5 };
static const double                        _vec_128[] = { 214.5, 215.5, 216.5 };
static const NCodecDiagXilDiagnosticSeries _vec_129[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_130[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_131[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1198),
        .unsigned_value = UINT64_C(4294967495),
        .float_value = 200.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_121,
        .bytes_value_len = ARRAY_SIZE(_vec_121),
        .row_count = 65738,
        .column_count = 65739,
        .x_values = _vec_122,
        .x_values_len = ARRAY_SIZE(_vec_122),
        .y_values = _vec_123,
        .y_values_len = ARRAY_SIZE(_vec_123),
        .series = _vec_124,
        .series_len = ARRAY_SIZE(_vec_124),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_125,
        .properties_len = ARRAY_SIZE(_vec_125),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1207),
        .unsigned_value = UINT64_C(4294967504),
        .float_value = 209.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_126,
        .bytes_value_len = ARRAY_SIZE(_vec_126),
        .row_count = 65747,
        .column_count = 65748,
        .x_values = _vec_127,
        .x_values_len = ARRAY_SIZE(_vec_127),
        .y_values = _vec_128,
        .y_values_len = ARRAY_SIZE(_vec_128),
        .series = _vec_129,
        .series_len = ARRAY_SIZE(_vec_129),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_130,
        .properties_len = ARRAY_SIZE(_vec_130),
    },
};
static const double                        _vec_132[] = { 217.5, 218.5, 219.5 };
static const double                        _vec_133[] = { 218.5, 219.5, 220.5 };
static const uint8_t                       _vec_134[] = { 223, 224, 225 };
static const double                        _vec_135[] = { 226.5, 227.5, 228.5 };
static const double                        _vec_136[] = { 227.5, 228.5, 229.5 };
static const NCodecDiagXilDiagnosticSeries _vec_137[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_138[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_139[] = { 232, 233, 234 };
static const double                        _vec_140[] = { 235.5, 236.5, 237.5 };
static const double                        _vec_141[] = { 236.5, 237.5, 238.5 };
static const NCodecDiagXilDiagnosticSeries _vec_142[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_143[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_144[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1220),
        .unsigned_value = UINT64_C(4294967517),
        .float_value = 222.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_134,
        .bytes_value_len = ARRAY_SIZE(_vec_134),
        .row_count = 65760,
        .column_count = 65761,
        .x_values = _vec_135,
        .x_values_len = ARRAY_SIZE(_vec_135),
        .y_values = _vec_136,
        .y_values_len = ARRAY_SIZE(_vec_136),
        .series = _vec_137,
        .series_len = ARRAY_SIZE(_vec_137),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_138,
        .properties_len = ARRAY_SIZE(_vec_138),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1229),
        .unsigned_value = UINT64_C(4294967526),
        .float_value = 231.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_139,
        .bytes_value_len = ARRAY_SIZE(_vec_139),
        .row_count = 65769,
        .column_count = 65770,
        .x_values = _vec_140,
        .x_values_len = ARRAY_SIZE(_vec_140),
        .y_values = _vec_141,
        .y_values_len = ARRAY_SIZE(_vec_141),
        .series = _vec_142,
        .series_len = ARRAY_SIZE(_vec_142),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_143,
        .properties_len = ARRAY_SIZE(_vec_143),
    },
};
static const uint8_t                       _vec_145[] = { 241, 242, 243 };
static const double                        _vec_146[] = { 244.5, 245.5, 246.5 };
static const double                        _vec_147[] = { 245.5, 246.5, 247.5 };
static const NCodecDiagXilDiagnosticSeries _vec_148[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_149[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_150[] = { 250, 251, 252 };
static const double                        _vec_151[] = { 253.5, 254.5, 255.5 };
static const double                        _vec_152[] = { 254.5, 255.5, 256.5 };
static const NCodecDiagXilDiagnosticSeries _vec_153[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_154[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_155[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1238),
        .unsigned_value = UINT64_C(4294967535),
        .float_value = 240.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_145,
        .bytes_value_len = ARRAY_SIZE(_vec_145),
        .row_count = 65778,
        .column_count = 65779,
        .x_values = _vec_146,
        .x_values_len = ARRAY_SIZE(_vec_146),
        .y_values = _vec_147,
        .y_values_len = ARRAY_SIZE(_vec_147),
        .series = _vec_148,
        .series_len = ARRAY_SIZE(_vec_148),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_149,
        .properties_len = ARRAY_SIZE(_vec_149),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1247),
        .unsigned_value = UINT64_C(4294967544),
        .float_value = 249.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_150,
        .bytes_value_len = ARRAY_SIZE(_vec_150),
        .row_count = 65787,
        .column_count = 65788,
        .x_values = _vec_151,
        .x_values_len = ARRAY_SIZE(_vec_151),
        .y_values = _vec_152,
        .y_values_len = ARRAY_SIZE(_vec_152),
        .series = _vec_153,
        .series_len = ARRAY_SIZE(_vec_153),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_154,
        .properties_len = ARRAY_SIZE(_vec_154),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_156[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_144,
        .values_len = ARRAY_SIZE(_vec_144),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_155,
        .values_len = ARRAY_SIZE(_vec_155),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_157[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_119 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1193),
    .unsigned_value = UINT64_C(4294967490),
    .float_value = 195.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_120,
    .bytes_value_len = ARRAY_SIZE(_vec_120),
    .elements = _vec_131,
    .elements_len = ARRAY_SIZE(_vec_131),
    .row_count = 65751,
    .column_count = 65752,
    .x_values = _vec_132,
    .x_values_len = ARRAY_SIZE(_vec_132),
    .y_values = _vec_133,
    .y_values_len = ARRAY_SIZE(_vec_133),
    .series = _vec_156,
    .series_len = ARRAY_SIZE(_vec_156),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_157,
    .properties_len = ARRAY_SIZE(_vec_157),
};
static const NCodecDiagXilDataValue _vec_158[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_78,
        .raw_value_len = ARRAY_SIZE(_vec_78),
        .physical_value = &_obj_79,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_118,
        .raw_value_len = ARRAY_SIZE(_vec_118),
        .physical_value = &_obj_119,
    },
};
static const uint8_t                       _vec_159[] = { 255, 0, 1 };
static const uint8_t                       _vec_160[] = { 1, 2, 3 };
static const uint8_t                       _vec_162[] = { 5, 6, 7 };
static const uint8_t                       _vec_163[] = { 6, 7, 8 };
static const uint8_t                       _vec_165[] = { 11, 12, 13 };
static const uint8_t                       _vec_166[] = { 16, 17, 18 };
static const double                        _vec_167[] = { 275.5, 276.5, 277.5 };
static const double                        _vec_168[] = { 276.5, 277.5, 278.5 };
static const NCodecDiagXilDiagnosticSeries _vec_169[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_170[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_171[] = { 25, 26, 27 };
static const double                        _vec_172[] = { 284.5, 285.5, 286.5 };
static const double                        _vec_173[] = { 285.5, 286.5, 287.5 };
static const NCodecDiagXilDiagnosticSeries _vec_174[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_175[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_176[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1269),
        .unsigned_value = UINT64_C(4294967566),
        .float_value = 271.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_166,
        .bytes_value_len = ARRAY_SIZE(_vec_166),
        .row_count = 65809,
        .column_count = 65810,
        .x_values = _vec_167,
        .x_values_len = ARRAY_SIZE(_vec_167),
        .y_values = _vec_168,
        .y_values_len = ARRAY_SIZE(_vec_168),
        .series = _vec_169,
        .series_len = ARRAY_SIZE(_vec_169),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_170,
        .properties_len = ARRAY_SIZE(_vec_170),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1278),
        .unsigned_value = UINT64_C(4294967575),
        .float_value = 280.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_171,
        .bytes_value_len = ARRAY_SIZE(_vec_171),
        .row_count = 65818,
        .column_count = 65819,
        .x_values = _vec_172,
        .x_values_len = ARRAY_SIZE(_vec_172),
        .y_values = _vec_173,
        .y_values_len = ARRAY_SIZE(_vec_173),
        .series = _vec_174,
        .series_len = ARRAY_SIZE(_vec_174),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_175,
        .properties_len = ARRAY_SIZE(_vec_175),
    },
};
static const double                        _vec_177[] = { 288.5, 289.5, 290.5 };
static const double                        _vec_178[] = { 289.5, 290.5, 291.5 };
static const uint8_t                       _vec_179[] = { 38, 39, 40 };
static const double                        _vec_180[] = { 297.5, 298.5, 299.5 };
static const double                        _vec_181[] = { 298.5, 299.5, 300.5 };
static const NCodecDiagXilDiagnosticSeries _vec_182[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_183[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_184[] = { 47, 48, 49 };
static const double                        _vec_185[] = { 306.5, 307.5, 308.5 };
static const double                        _vec_186[] = { 307.5, 308.5, 309.5 };
static const NCodecDiagXilDiagnosticSeries _vec_187[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_188[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_189[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1291),
        .unsigned_value = UINT64_C(4294967588),
        .float_value = 293.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_179,
        .bytes_value_len = ARRAY_SIZE(_vec_179),
        .row_count = 65831,
        .column_count = 65832,
        .x_values = _vec_180,
        .x_values_len = ARRAY_SIZE(_vec_180),
        .y_values = _vec_181,
        .y_values_len = ARRAY_SIZE(_vec_181),
        .series = _vec_182,
        .series_len = ARRAY_SIZE(_vec_182),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_183,
        .properties_len = ARRAY_SIZE(_vec_183),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1300),
        .unsigned_value = UINT64_C(4294967597),
        .float_value = 302.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_184,
        .bytes_value_len = ARRAY_SIZE(_vec_184),
        .row_count = 65840,
        .column_count = 65841,
        .x_values = _vec_185,
        .x_values_len = ARRAY_SIZE(_vec_185),
        .y_values = _vec_186,
        .y_values_len = ARRAY_SIZE(_vec_186),
        .series = _vec_187,
        .series_len = ARRAY_SIZE(_vec_187),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_188,
        .properties_len = ARRAY_SIZE(_vec_188),
    },
};
static const uint8_t                       _vec_190[] = { 56, 57, 58 };
static const double                        _vec_191[] = { 315.5, 316.5, 317.5 };
static const double                        _vec_192[] = { 316.5, 317.5, 318.5 };
static const NCodecDiagXilDiagnosticSeries _vec_193[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_194[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_195[] = { 65, 66, 67 };
static const double                        _vec_196[] = { 324.5, 325.5, 326.5 };
static const double                        _vec_197[] = { 325.5, 326.5, 327.5 };
static const NCodecDiagXilDiagnosticSeries _vec_198[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_199[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_200[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1309),
        .unsigned_value = UINT64_C(4294967606),
        .float_value = 311.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_190,
        .bytes_value_len = ARRAY_SIZE(_vec_190),
        .row_count = 65849,
        .column_count = 65850,
        .x_values = _vec_191,
        .x_values_len = ARRAY_SIZE(_vec_191),
        .y_values = _vec_192,
        .y_values_len = ARRAY_SIZE(_vec_192),
        .series = _vec_193,
        .series_len = ARRAY_SIZE(_vec_193),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_194,
        .properties_len = ARRAY_SIZE(_vec_194),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1318),
        .unsigned_value = UINT64_C(4294967615),
        .float_value = 320.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_195,
        .bytes_value_len = ARRAY_SIZE(_vec_195),
        .row_count = 65858,
        .column_count = 65859,
        .x_values = _vec_196,
        .x_values_len = ARRAY_SIZE(_vec_196),
        .y_values = _vec_197,
        .y_values_len = ARRAY_SIZE(_vec_197),
        .series = _vec_198,
        .series_len = ARRAY_SIZE(_vec_198),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_199,
        .properties_len = ARRAY_SIZE(_vec_199),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_201[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_189,
        .values_len = ARRAY_SIZE(_vec_189),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_200,
        .values_len = ARRAY_SIZE(_vec_200),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_202[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_164 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1264),
    .unsigned_value = UINT64_C(4294967561),
    .float_value = 266.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_165,
    .bytes_value_len = ARRAY_SIZE(_vec_165),
    .elements = _vec_176,
    .elements_len = ARRAY_SIZE(_vec_176),
    .row_count = 65822,
    .column_count = 65823,
    .x_values = _vec_177,
    .x_values_len = ARRAY_SIZE(_vec_177),
    .y_values = _vec_178,
    .y_values_len = ARRAY_SIZE(_vec_178),
    .series = _vec_201,
    .series_len = ARRAY_SIZE(_vec_201),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_202,
    .properties_len = ARRAY_SIZE(_vec_202),
};
static const uint8_t                       _vec_203[] = { 70, 71, 72 };
static const uint8_t                       _vec_205[] = { 75, 76, 77 };
static const uint8_t                       _vec_206[] = { 80, 81, 82 };
static const double                        _vec_207[] = { 339.5, 340.5, 341.5 };
static const double                        _vec_208[] = { 340.5, 341.5, 342.5 };
static const NCodecDiagXilDiagnosticSeries _vec_209[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_210[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_211[] = { 89, 90, 91 };
static const double                        _vec_212[] = { 348.5, 349.5, 350.5 };
static const double                        _vec_213[] = { 349.5, 350.5, 351.5 };
static const NCodecDiagXilDiagnosticSeries _vec_214[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_215[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_216[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1333),
        .unsigned_value = UINT64_C(4294967630),
        .float_value = 335.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_206,
        .bytes_value_len = ARRAY_SIZE(_vec_206),
        .row_count = 65873,
        .column_count = 65874,
        .x_values = _vec_207,
        .x_values_len = ARRAY_SIZE(_vec_207),
        .y_values = _vec_208,
        .y_values_len = ARRAY_SIZE(_vec_208),
        .series = _vec_209,
        .series_len = ARRAY_SIZE(_vec_209),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_210,
        .properties_len = ARRAY_SIZE(_vec_210),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1342),
        .unsigned_value = UINT64_C(4294967639),
        .float_value = 344.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_211,
        .bytes_value_len = ARRAY_SIZE(_vec_211),
        .row_count = 65882,
        .column_count = 65883,
        .x_values = _vec_212,
        .x_values_len = ARRAY_SIZE(_vec_212),
        .y_values = _vec_213,
        .y_values_len = ARRAY_SIZE(_vec_213),
        .series = _vec_214,
        .series_len = ARRAY_SIZE(_vec_214),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_215,
        .properties_len = ARRAY_SIZE(_vec_215),
    },
};
static const double                        _vec_217[] = { 352.5, 353.5, 354.5 };
static const double                        _vec_218[] = { 353.5, 354.5, 355.5 };
static const uint8_t                       _vec_219[] = { 102, 103, 104 };
static const double                        _vec_220[] = { 361.5, 362.5, 363.5 };
static const double                        _vec_221[] = { 362.5, 363.5, 364.5 };
static const NCodecDiagXilDiagnosticSeries _vec_222[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_223[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_224[] = { 111, 112, 113 };
static const double                        _vec_225[] = { 370.5, 371.5, 372.5 };
static const double                        _vec_226[] = { 371.5, 372.5, 373.5 };
static const NCodecDiagXilDiagnosticSeries _vec_227[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_228[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_229[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1355),
        .unsigned_value = UINT64_C(4294967652),
        .float_value = 357.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_219,
        .bytes_value_len = ARRAY_SIZE(_vec_219),
        .row_count = 65895,
        .column_count = 65896,
        .x_values = _vec_220,
        .x_values_len = ARRAY_SIZE(_vec_220),
        .y_values = _vec_221,
        .y_values_len = ARRAY_SIZE(_vec_221),
        .series = _vec_222,
        .series_len = ARRAY_SIZE(_vec_222),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_223,
        .properties_len = ARRAY_SIZE(_vec_223),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1364),
        .unsigned_value = UINT64_C(4294967661),
        .float_value = 366.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_224,
        .bytes_value_len = ARRAY_SIZE(_vec_224),
        .row_count = 65904,
        .column_count = 65905,
        .x_values = _vec_225,
        .x_values_len = ARRAY_SIZE(_vec_225),
        .y_values = _vec_226,
        .y_values_len = ARRAY_SIZE(_vec_226),
        .series = _vec_227,
        .series_len = ARRAY_SIZE(_vec_227),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_228,
        .properties_len = ARRAY_SIZE(_vec_228),
    },
};
static const uint8_t                       _vec_230[] = { 120, 121, 122 };
static const double                        _vec_231[] = { 379.5, 380.5, 381.5 };
static const double                        _vec_232[] = { 380.5, 381.5, 382.5 };
static const NCodecDiagXilDiagnosticSeries _vec_233[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_234[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_235[] = { 129, 130, 131 };
static const double                        _vec_236[] = { 388.5, 389.5, 390.5 };
static const double                        _vec_237[] = { 389.5, 390.5, 391.5 };
static const NCodecDiagXilDiagnosticSeries _vec_238[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_239[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_240[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1373),
        .unsigned_value = UINT64_C(4294967670),
        .float_value = 375.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_230,
        .bytes_value_len = ARRAY_SIZE(_vec_230),
        .row_count = 65913,
        .column_count = 65914,
        .x_values = _vec_231,
        .x_values_len = ARRAY_SIZE(_vec_231),
        .y_values = _vec_232,
        .y_values_len = ARRAY_SIZE(_vec_232),
        .series = _vec_233,
        .series_len = ARRAY_SIZE(_vec_233),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_234,
        .properties_len = ARRAY_SIZE(_vec_234),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1382),
        .unsigned_value = UINT64_C(4294967679),
        .float_value = 384.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_235,
        .bytes_value_len = ARRAY_SIZE(_vec_235),
        .row_count = 65922,
        .column_count = 65923,
        .x_values = _vec_236,
        .x_values_len = ARRAY_SIZE(_vec_236),
        .y_values = _vec_237,
        .y_values_len = ARRAY_SIZE(_vec_237),
        .series = _vec_238,
        .series_len = ARRAY_SIZE(_vec_238),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_239,
        .properties_len = ARRAY_SIZE(_vec_239),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_241[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_229,
        .values_len = ARRAY_SIZE(_vec_229),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_240,
        .values_len = ARRAY_SIZE(_vec_240),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_242[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_204 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1328),
    .unsigned_value = UINT64_C(4294967625),
    .float_value = 330.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_205,
    .bytes_value_len = ARRAY_SIZE(_vec_205),
    .elements = _vec_216,
    .elements_len = ARRAY_SIZE(_vec_216),
    .row_count = 65886,
    .column_count = 65887,
    .x_values = _vec_217,
    .x_values_len = ARRAY_SIZE(_vec_217),
    .y_values = _vec_218,
    .y_values_len = ARRAY_SIZE(_vec_218),
    .series = _vec_241,
    .series_len = ARRAY_SIZE(_vec_241),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_242,
    .properties_len = ARRAY_SIZE(_vec_242),
};
static const NCodecDiagXilDataValue _vec_243[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_163,
        .raw_value_len = ARRAY_SIZE(_vec_163),
        .physical_value = &_obj_164,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_203,
        .raw_value_len = ARRAY_SIZE(_vec_203),
        .physical_value = &_obj_204,
    },
};
static const NCodecDiagXilDidIdentity _obj_244 = {
    .name = "DidIdentity.name",
    .identifier = 646,
};
static const NCodecDiagXilDidValue _obj_161 = {
    .did = {
        .kind = NCodecDiagXilDidSelectorKindName,
        .identifier = 516,
        .name = "DidSelector.name",
    },
    .raw_value = _vec_162,
    .raw_value_len = ARRAY_SIZE(_vec_162),
    .values = _vec_243,
    .values_len = ARRAY_SIZE(_vec_243),
    .identity = &_obj_244,
};
static const uint8_t                       _vec_245[] = { 136, 137, 138 };
static const uint8_t                       _vec_246[] = { 137, 138, 139 };
static const uint8_t                       _vec_248[] = { 142, 143, 144 };
static const uint8_t                       _vec_249[] = { 147, 148, 149 };
static const double                        _vec_250[] = { 406.5, 407.5, 408.5 };
static const double                        _vec_251[] = { 407.5, 408.5, 409.5 };
static const NCodecDiagXilDiagnosticSeries _vec_252[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_253[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_254[] = { 156, 157, 158 };
static const double                        _vec_255[] = { 415.5, 416.5, 417.5 };
static const double                        _vec_256[] = { 416.5, 417.5, 418.5 };
static const NCodecDiagXilDiagnosticSeries _vec_257[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_258[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_259[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1400),
        .unsigned_value = UINT64_C(4294967697),
        .float_value = 402.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_249,
        .bytes_value_len = ARRAY_SIZE(_vec_249),
        .row_count = 65940,
        .column_count = 65941,
        .x_values = _vec_250,
        .x_values_len = ARRAY_SIZE(_vec_250),
        .y_values = _vec_251,
        .y_values_len = ARRAY_SIZE(_vec_251),
        .series = _vec_252,
        .series_len = ARRAY_SIZE(_vec_252),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_253,
        .properties_len = ARRAY_SIZE(_vec_253),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1409),
        .unsigned_value = UINT64_C(4294967706),
        .float_value = 411.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_254,
        .bytes_value_len = ARRAY_SIZE(_vec_254),
        .row_count = 65949,
        .column_count = 65950,
        .x_values = _vec_255,
        .x_values_len = ARRAY_SIZE(_vec_255),
        .y_values = _vec_256,
        .y_values_len = ARRAY_SIZE(_vec_256),
        .series = _vec_257,
        .series_len = ARRAY_SIZE(_vec_257),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_258,
        .properties_len = ARRAY_SIZE(_vec_258),
    },
};
static const double                        _vec_260[] = { 419.5, 420.5, 421.5 };
static const double                        _vec_261[] = { 420.5, 421.5, 422.5 };
static const uint8_t                       _vec_262[] = { 169, 170, 171 };
static const double                        _vec_263[] = { 428.5, 429.5, 430.5 };
static const double                        _vec_264[] = { 429.5, 430.5, 431.5 };
static const NCodecDiagXilDiagnosticSeries _vec_265[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_266[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_267[] = { 178, 179, 180 };
static const double                        _vec_268[] = { 437.5, 438.5, 439.5 };
static const double                        _vec_269[] = { 438.5, 439.5, 440.5 };
static const NCodecDiagXilDiagnosticSeries _vec_270[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_271[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_272[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1422),
        .unsigned_value = UINT64_C(4294967719),
        .float_value = 424.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_262,
        .bytes_value_len = ARRAY_SIZE(_vec_262),
        .row_count = 65962,
        .column_count = 65963,
        .x_values = _vec_263,
        .x_values_len = ARRAY_SIZE(_vec_263),
        .y_values = _vec_264,
        .y_values_len = ARRAY_SIZE(_vec_264),
        .series = _vec_265,
        .series_len = ARRAY_SIZE(_vec_265),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_266,
        .properties_len = ARRAY_SIZE(_vec_266),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1431),
        .unsigned_value = UINT64_C(4294967728),
        .float_value = 433.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_267,
        .bytes_value_len = ARRAY_SIZE(_vec_267),
        .row_count = 65971,
        .column_count = 65972,
        .x_values = _vec_268,
        .x_values_len = ARRAY_SIZE(_vec_268),
        .y_values = _vec_269,
        .y_values_len = ARRAY_SIZE(_vec_269),
        .series = _vec_270,
        .series_len = ARRAY_SIZE(_vec_270),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_271,
        .properties_len = ARRAY_SIZE(_vec_271),
    },
};
static const uint8_t                       _vec_273[] = { 187, 188, 189 };
static const double                        _vec_274[] = { 446.5, 447.5, 448.5 };
static const double                        _vec_275[] = { 447.5, 448.5, 449.5 };
static const NCodecDiagXilDiagnosticSeries _vec_276[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_277[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_278[] = { 196, 197, 198 };
static const double                        _vec_279[] = { 455.5, 456.5, 457.5 };
static const double                        _vec_280[] = { 456.5, 457.5, 458.5 };
static const NCodecDiagXilDiagnosticSeries _vec_281[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_282[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_283[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1440),
        .unsigned_value = UINT64_C(4294967737),
        .float_value = 442.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_273,
        .bytes_value_len = ARRAY_SIZE(_vec_273),
        .row_count = 65980,
        .column_count = 65981,
        .x_values = _vec_274,
        .x_values_len = ARRAY_SIZE(_vec_274),
        .y_values = _vec_275,
        .y_values_len = ARRAY_SIZE(_vec_275),
        .series = _vec_276,
        .series_len = ARRAY_SIZE(_vec_276),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_277,
        .properties_len = ARRAY_SIZE(_vec_277),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1449),
        .unsigned_value = UINT64_C(4294967746),
        .float_value = 451.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_278,
        .bytes_value_len = ARRAY_SIZE(_vec_278),
        .row_count = 65989,
        .column_count = 65990,
        .x_values = _vec_279,
        .x_values_len = ARRAY_SIZE(_vec_279),
        .y_values = _vec_280,
        .y_values_len = ARRAY_SIZE(_vec_280),
        .series = _vec_281,
        .series_len = ARRAY_SIZE(_vec_281),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_282,
        .properties_len = ARRAY_SIZE(_vec_282),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_284[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_272,
        .values_len = ARRAY_SIZE(_vec_272),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_283,
        .values_len = ARRAY_SIZE(_vec_283),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_285[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_247 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1395),
    .unsigned_value = UINT64_C(4294967692),
    .float_value = 397.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_248,
    .bytes_value_len = ARRAY_SIZE(_vec_248),
    .elements = _vec_259,
    .elements_len = ARRAY_SIZE(_vec_259),
    .row_count = 65953,
    .column_count = 65954,
    .x_values = _vec_260,
    .x_values_len = ARRAY_SIZE(_vec_260),
    .y_values = _vec_261,
    .y_values_len = ARRAY_SIZE(_vec_261),
    .series = _vec_284,
    .series_len = ARRAY_SIZE(_vec_284),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_285,
    .properties_len = ARRAY_SIZE(_vec_285),
};
static const uint8_t                       _vec_286[] = { 201, 202, 203 };
static const uint8_t                       _vec_288[] = { 206, 207, 208 };
static const uint8_t                       _vec_289[] = { 211, 212, 213 };
static const double                        _vec_290[] = { 470.5, 471.5, 472.5 };
static const double                        _vec_291[] = { 471.5, 472.5, 473.5 };
static const NCodecDiagXilDiagnosticSeries _vec_292[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_293[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_294[] = { 220, 221, 222 };
static const double                        _vec_295[] = { 479.5, 480.5, 481.5 };
static const double                        _vec_296[] = { 480.5, 481.5, 482.5 };
static const NCodecDiagXilDiagnosticSeries _vec_297[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_298[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_299[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1464),
        .unsigned_value = UINT64_C(4294967761),
        .float_value = 466.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_289,
        .bytes_value_len = ARRAY_SIZE(_vec_289),
        .row_count = 66004,
        .column_count = 66005,
        .x_values = _vec_290,
        .x_values_len = ARRAY_SIZE(_vec_290),
        .y_values = _vec_291,
        .y_values_len = ARRAY_SIZE(_vec_291),
        .series = _vec_292,
        .series_len = ARRAY_SIZE(_vec_292),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_293,
        .properties_len = ARRAY_SIZE(_vec_293),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1473),
        .unsigned_value = UINT64_C(4294967770),
        .float_value = 475.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_294,
        .bytes_value_len = ARRAY_SIZE(_vec_294),
        .row_count = 66013,
        .column_count = 66014,
        .x_values = _vec_295,
        .x_values_len = ARRAY_SIZE(_vec_295),
        .y_values = _vec_296,
        .y_values_len = ARRAY_SIZE(_vec_296),
        .series = _vec_297,
        .series_len = ARRAY_SIZE(_vec_297),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_298,
        .properties_len = ARRAY_SIZE(_vec_298),
    },
};
static const double                        _vec_300[] = { 483.5, 484.5, 485.5 };
static const double                        _vec_301[] = { 484.5, 485.5, 486.5 };
static const uint8_t                       _vec_302[] = { 233, 234, 235 };
static const double                        _vec_303[] = { 492.5, 493.5, 494.5 };
static const double                        _vec_304[] = { 493.5, 494.5, 495.5 };
static const NCodecDiagXilDiagnosticSeries _vec_305[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_306[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_307[] = { 242, 243, 244 };
static const double                        _vec_308[] = { 501.5, 502.5, 503.5 };
static const double                        _vec_309[] = { 502.5, 503.5, 504.5 };
static const NCodecDiagXilDiagnosticSeries _vec_310[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_311[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_312[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1486),
        .unsigned_value = UINT64_C(4294967783),
        .float_value = 488.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_302,
        .bytes_value_len = ARRAY_SIZE(_vec_302),
        .row_count = 66026,
        .column_count = 66027,
        .x_values = _vec_303,
        .x_values_len = ARRAY_SIZE(_vec_303),
        .y_values = _vec_304,
        .y_values_len = ARRAY_SIZE(_vec_304),
        .series = _vec_305,
        .series_len = ARRAY_SIZE(_vec_305),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_306,
        .properties_len = ARRAY_SIZE(_vec_306),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1495),
        .unsigned_value = UINT64_C(4294967792),
        .float_value = 497.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_307,
        .bytes_value_len = ARRAY_SIZE(_vec_307),
        .row_count = 66035,
        .column_count = 66036,
        .x_values = _vec_308,
        .x_values_len = ARRAY_SIZE(_vec_308),
        .y_values = _vec_309,
        .y_values_len = ARRAY_SIZE(_vec_309),
        .series = _vec_310,
        .series_len = ARRAY_SIZE(_vec_310),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_311,
        .properties_len = ARRAY_SIZE(_vec_311),
    },
};
static const uint8_t                       _vec_313[] = { 251, 252, 253 };
static const double                        _vec_314[] = { 510.5, 511.5, 512.5 };
static const double                        _vec_315[] = { 511.5, 512.5, 513.5 };
static const NCodecDiagXilDiagnosticSeries _vec_316[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_317[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_318[] = { 4, 5, 6 };
static const double                        _vec_319[] = { 519.5, 520.5, 521.5 };
static const double                        _vec_320[] = { 520.5, 521.5, 522.5 };
static const NCodecDiagXilDiagnosticSeries _vec_321[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_322[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_323[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1504),
        .unsigned_value = UINT64_C(4294967801),
        .float_value = 506.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_313,
        .bytes_value_len = ARRAY_SIZE(_vec_313),
        .row_count = 66044,
        .column_count = 66045,
        .x_values = _vec_314,
        .x_values_len = ARRAY_SIZE(_vec_314),
        .y_values = _vec_315,
        .y_values_len = ARRAY_SIZE(_vec_315),
        .series = _vec_316,
        .series_len = ARRAY_SIZE(_vec_316),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_317,
        .properties_len = ARRAY_SIZE(_vec_317),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1513),
        .unsigned_value = UINT64_C(4294967810),
        .float_value = 515.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_318,
        .bytes_value_len = ARRAY_SIZE(_vec_318),
        .row_count = 66053,
        .column_count = 66054,
        .x_values = _vec_319,
        .x_values_len = ARRAY_SIZE(_vec_319),
        .y_values = _vec_320,
        .y_values_len = ARRAY_SIZE(_vec_320),
        .series = _vec_321,
        .series_len = ARRAY_SIZE(_vec_321),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_322,
        .properties_len = ARRAY_SIZE(_vec_322),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_324[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_312,
        .values_len = ARRAY_SIZE(_vec_312),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_323,
        .values_len = ARRAY_SIZE(_vec_323),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_325[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_287 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1459),
    .unsigned_value = UINT64_C(4294967756),
    .float_value = 461.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_288,
    .bytes_value_len = ARRAY_SIZE(_vec_288),
    .elements = _vec_299,
    .elements_len = ARRAY_SIZE(_vec_299),
    .row_count = 66017,
    .column_count = 66018,
    .x_values = _vec_300,
    .x_values_len = ARRAY_SIZE(_vec_300),
    .y_values = _vec_301,
    .y_values_len = ARRAY_SIZE(_vec_301),
    .series = _vec_324,
    .series_len = ARRAY_SIZE(_vec_324),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_325,
    .properties_len = ARRAY_SIZE(_vec_325),
};
static const NCodecDiagXilDataValue _vec_326[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_246,
        .raw_value_len = ARRAY_SIZE(_vec_246),
        .physical_value = &_obj_247,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_286,
        .raw_value_len = ARRAY_SIZE(_vec_286),
        .physical_value = &_obj_287,
    },
};
static const NCodecDiagXilDidIdentity _obj_327 = {
    .name = "DidIdentity.name",
    .identifier = 777,
};
static const uint8_t                       _vec_328[] = { 13, 14, 15 };
static const uint8_t                       _vec_330[] = { 18, 19, 20 };
static const uint8_t                       _vec_331[] = { 23, 24, 25 };
static const double                        _vec_332[] = { 538.5, 539.5, 540.5 };
static const double                        _vec_333[] = { 539.5, 540.5, 541.5 };
static const NCodecDiagXilDiagnosticSeries _vec_334[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_335[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_336[] = { 32, 33, 34 };
static const double                        _vec_337[] = { 547.5, 548.5, 549.5 };
static const double                        _vec_338[] = { 548.5, 549.5, 550.5 };
static const NCodecDiagXilDiagnosticSeries _vec_339[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_340[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_341[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1532),
        .unsigned_value = UINT64_C(4294967829),
        .float_value = 534.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_331,
        .bytes_value_len = ARRAY_SIZE(_vec_331),
        .row_count = 66072,
        .column_count = 66073,
        .x_values = _vec_332,
        .x_values_len = ARRAY_SIZE(_vec_332),
        .y_values = _vec_333,
        .y_values_len = ARRAY_SIZE(_vec_333),
        .series = _vec_334,
        .series_len = ARRAY_SIZE(_vec_334),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_335,
        .properties_len = ARRAY_SIZE(_vec_335),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1541),
        .unsigned_value = UINT64_C(4294967838),
        .float_value = 543.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_336,
        .bytes_value_len = ARRAY_SIZE(_vec_336),
        .row_count = 66081,
        .column_count = 66082,
        .x_values = _vec_337,
        .x_values_len = ARRAY_SIZE(_vec_337),
        .y_values = _vec_338,
        .y_values_len = ARRAY_SIZE(_vec_338),
        .series = _vec_339,
        .series_len = ARRAY_SIZE(_vec_339),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_340,
        .properties_len = ARRAY_SIZE(_vec_340),
    },
};
static const double                        _vec_342[] = { 551.5, 552.5, 553.5 };
static const double                        _vec_343[] = { 552.5, 553.5, 554.5 };
static const uint8_t                       _vec_344[] = { 45, 46, 47 };
static const double                        _vec_345[] = { 560.5, 561.5, 562.5 };
static const double                        _vec_346[] = { 561.5, 562.5, 563.5 };
static const NCodecDiagXilDiagnosticSeries _vec_347[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_348[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_349[] = { 54, 55, 56 };
static const double                        _vec_350[] = { 569.5, 570.5, 571.5 };
static const double                        _vec_351[] = { 570.5, 571.5, 572.5 };
static const NCodecDiagXilDiagnosticSeries _vec_352[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_353[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_354[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1554),
        .unsigned_value = UINT64_C(4294967851),
        .float_value = 556.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_344,
        .bytes_value_len = ARRAY_SIZE(_vec_344),
        .row_count = 66094,
        .column_count = 66095,
        .x_values = _vec_345,
        .x_values_len = ARRAY_SIZE(_vec_345),
        .y_values = _vec_346,
        .y_values_len = ARRAY_SIZE(_vec_346),
        .series = _vec_347,
        .series_len = ARRAY_SIZE(_vec_347),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_348,
        .properties_len = ARRAY_SIZE(_vec_348),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1563),
        .unsigned_value = UINT64_C(4294967860),
        .float_value = 565.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_349,
        .bytes_value_len = ARRAY_SIZE(_vec_349),
        .row_count = 66103,
        .column_count = 66104,
        .x_values = _vec_350,
        .x_values_len = ARRAY_SIZE(_vec_350),
        .y_values = _vec_351,
        .y_values_len = ARRAY_SIZE(_vec_351),
        .series = _vec_352,
        .series_len = ARRAY_SIZE(_vec_352),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_353,
        .properties_len = ARRAY_SIZE(_vec_353),
    },
};
static const uint8_t                       _vec_355[] = { 63, 64, 65 };
static const double                        _vec_356[] = { 578.5, 579.5, 580.5 };
static const double                        _vec_357[] = { 579.5, 580.5, 581.5 };
static const NCodecDiagXilDiagnosticSeries _vec_358[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_359[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_360[] = { 72, 73, 74 };
static const double                        _vec_361[] = { 587.5, 588.5, 589.5 };
static const double                        _vec_362[] = { 588.5, 589.5, 590.5 };
static const NCodecDiagXilDiagnosticSeries _vec_363[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_364[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_365[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1572),
        .unsigned_value = UINT64_C(4294967869),
        .float_value = 574.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_355,
        .bytes_value_len = ARRAY_SIZE(_vec_355),
        .row_count = 66112,
        .column_count = 66113,
        .x_values = _vec_356,
        .x_values_len = ARRAY_SIZE(_vec_356),
        .y_values = _vec_357,
        .y_values_len = ARRAY_SIZE(_vec_357),
        .series = _vec_358,
        .series_len = ARRAY_SIZE(_vec_358),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_359,
        .properties_len = ARRAY_SIZE(_vec_359),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1581),
        .unsigned_value = UINT64_C(4294967878),
        .float_value = 583.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_360,
        .bytes_value_len = ARRAY_SIZE(_vec_360),
        .row_count = 66121,
        .column_count = 66122,
        .x_values = _vec_361,
        .x_values_len = ARRAY_SIZE(_vec_361),
        .y_values = _vec_362,
        .y_values_len = ARRAY_SIZE(_vec_362),
        .series = _vec_363,
        .series_len = ARRAY_SIZE(_vec_363),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_364,
        .properties_len = ARRAY_SIZE(_vec_364),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_366[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_354,
        .values_len = ARRAY_SIZE(_vec_354),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_365,
        .values_len = ARRAY_SIZE(_vec_365),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_367[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_329 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1527),
    .unsigned_value = UINT64_C(4294967824),
    .float_value = 529.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_330,
    .bytes_value_len = ARRAY_SIZE(_vec_330),
    .elements = _vec_341,
    .elements_len = ARRAY_SIZE(_vec_341),
    .row_count = 66085,
    .column_count = 66086,
    .x_values = _vec_342,
    .x_values_len = ARRAY_SIZE(_vec_342),
    .y_values = _vec_343,
    .y_values_len = ARRAY_SIZE(_vec_343),
    .series = _vec_366,
    .series_len = ARRAY_SIZE(_vec_366),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_367,
    .properties_len = ARRAY_SIZE(_vec_367),
};
static const uint8_t                       _vec_368[] = { 77, 78, 79 };
static const uint8_t                       _vec_370[] = { 82, 83, 84 };
static const uint8_t                       _vec_371[] = { 87, 88, 89 };
static const double                        _vec_372[] = { 602.5, 603.5, 604.5 };
static const double                        _vec_373[] = { 603.5, 604.5, 605.5 };
static const NCodecDiagXilDiagnosticSeries _vec_374[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_375[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_376[] = { 96, 97, 98 };
static const double                        _vec_377[] = { 611.5, 612.5, 613.5 };
static const double                        _vec_378[] = { 612.5, 613.5, 614.5 };
static const NCodecDiagXilDiagnosticSeries _vec_379[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_380[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_381[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1596),
        .unsigned_value = UINT64_C(4294967893),
        .float_value = 598.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_371,
        .bytes_value_len = ARRAY_SIZE(_vec_371),
        .row_count = 66136,
        .column_count = 66137,
        .x_values = _vec_372,
        .x_values_len = ARRAY_SIZE(_vec_372),
        .y_values = _vec_373,
        .y_values_len = ARRAY_SIZE(_vec_373),
        .series = _vec_374,
        .series_len = ARRAY_SIZE(_vec_374),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_375,
        .properties_len = ARRAY_SIZE(_vec_375),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1605),
        .unsigned_value = UINT64_C(4294967902),
        .float_value = 607.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_376,
        .bytes_value_len = ARRAY_SIZE(_vec_376),
        .row_count = 66145,
        .column_count = 66146,
        .x_values = _vec_377,
        .x_values_len = ARRAY_SIZE(_vec_377),
        .y_values = _vec_378,
        .y_values_len = ARRAY_SIZE(_vec_378),
        .series = _vec_379,
        .series_len = ARRAY_SIZE(_vec_379),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_380,
        .properties_len = ARRAY_SIZE(_vec_380),
    },
};
static const double                        _vec_382[] = { 615.5, 616.5, 617.5 };
static const double                        _vec_383[] = { 616.5, 617.5, 618.5 };
static const uint8_t                       _vec_384[] = { 109, 110, 111 };
static const double                        _vec_385[] = { 624.5, 625.5, 626.5 };
static const double                        _vec_386[] = { 625.5, 626.5, 627.5 };
static const NCodecDiagXilDiagnosticSeries _vec_387[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_388[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_389[] = { 118, 119, 120 };
static const double                        _vec_390[] = { 633.5, 634.5, 635.5 };
static const double                        _vec_391[] = { 634.5, 635.5, 636.5 };
static const NCodecDiagXilDiagnosticSeries _vec_392[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_393[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_394[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1618),
        .unsigned_value = UINT64_C(4294967915),
        .float_value = 620.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_384,
        .bytes_value_len = ARRAY_SIZE(_vec_384),
        .row_count = 66158,
        .column_count = 66159,
        .x_values = _vec_385,
        .x_values_len = ARRAY_SIZE(_vec_385),
        .y_values = _vec_386,
        .y_values_len = ARRAY_SIZE(_vec_386),
        .series = _vec_387,
        .series_len = ARRAY_SIZE(_vec_387),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_388,
        .properties_len = ARRAY_SIZE(_vec_388),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1627),
        .unsigned_value = UINT64_C(4294967924),
        .float_value = 629.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_389,
        .bytes_value_len = ARRAY_SIZE(_vec_389),
        .row_count = 66167,
        .column_count = 66168,
        .x_values = _vec_390,
        .x_values_len = ARRAY_SIZE(_vec_390),
        .y_values = _vec_391,
        .y_values_len = ARRAY_SIZE(_vec_391),
        .series = _vec_392,
        .series_len = ARRAY_SIZE(_vec_392),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_393,
        .properties_len = ARRAY_SIZE(_vec_393),
    },
};
static const uint8_t                       _vec_395[] = { 127, 128, 129 };
static const double                        _vec_396[] = { 642.5, 643.5, 644.5 };
static const double                        _vec_397[] = { 643.5, 644.5, 645.5 };
static const NCodecDiagXilDiagnosticSeries _vec_398[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_399[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_400[] = { 136, 137, 138 };
static const double                        _vec_401[] = { 651.5, 652.5, 653.5 };
static const double                        _vec_402[] = { 652.5, 653.5, 654.5 };
static const NCodecDiagXilDiagnosticSeries _vec_403[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_404[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_405[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1636),
        .unsigned_value = UINT64_C(4294967933),
        .float_value = 638.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_395,
        .bytes_value_len = ARRAY_SIZE(_vec_395),
        .row_count = 66176,
        .column_count = 66177,
        .x_values = _vec_396,
        .x_values_len = ARRAY_SIZE(_vec_396),
        .y_values = _vec_397,
        .y_values_len = ARRAY_SIZE(_vec_397),
        .series = _vec_398,
        .series_len = ARRAY_SIZE(_vec_398),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_399,
        .properties_len = ARRAY_SIZE(_vec_399),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1645),
        .unsigned_value = UINT64_C(4294967942),
        .float_value = 647.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_400,
        .bytes_value_len = ARRAY_SIZE(_vec_400),
        .row_count = 66185,
        .column_count = 66186,
        .x_values = _vec_401,
        .x_values_len = ARRAY_SIZE(_vec_401),
        .y_values = _vec_402,
        .y_values_len = ARRAY_SIZE(_vec_402),
        .series = _vec_403,
        .series_len = ARRAY_SIZE(_vec_403),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_404,
        .properties_len = ARRAY_SIZE(_vec_404),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_406[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_394,
        .values_len = ARRAY_SIZE(_vec_394),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_405,
        .values_len = ARRAY_SIZE(_vec_405),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_407[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_369 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1591),
    .unsigned_value = UINT64_C(4294967888),
    .float_value = 593.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_370,
    .bytes_value_len = ARRAY_SIZE(_vec_370),
    .elements = _vec_381,
    .elements_len = ARRAY_SIZE(_vec_381),
    .row_count = 66149,
    .column_count = 66150,
    .x_values = _vec_382,
    .x_values_len = ARRAY_SIZE(_vec_382),
    .y_values = _vec_383,
    .y_values_len = ARRAY_SIZE(_vec_383),
    .series = _vec_406,
    .series_len = ARRAY_SIZE(_vec_406),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_407,
    .properties_len = ARRAY_SIZE(_vec_407),
};
static const NCodecDiagXilDataValue _vec_408[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_328,
        .raw_value_len = ARRAY_SIZE(_vec_328),
        .physical_value = &_obj_329,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_368,
        .raw_value_len = ARRAY_SIZE(_vec_368),
        .physical_value = &_obj_369,
    },
};
static const uint8_t                       _vec_409[] = { 143, 144, 145 };
static const uint8_t                       _vec_411[] = { 148, 149, 150 };
static const uint8_t                       _vec_412[] = { 153, 154, 155 };
static const double                        _vec_413[] = { 668.5, 669.5, 670.5 };
static const double                        _vec_414[] = { 669.5, 670.5, 671.5 };
static const NCodecDiagXilDiagnosticSeries _vec_415[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_416[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_417[] = { 162, 163, 164 };
static const double                        _vec_418[] = { 677.5, 678.5, 679.5 };
static const double                        _vec_419[] = { 678.5, 679.5, 680.5 };
static const NCodecDiagXilDiagnosticSeries _vec_420[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_421[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_422[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1662),
        .unsigned_value = UINT64_C(4294967959),
        .float_value = 664.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_412,
        .bytes_value_len = ARRAY_SIZE(_vec_412),
        .row_count = 66202,
        .column_count = 66203,
        .x_values = _vec_413,
        .x_values_len = ARRAY_SIZE(_vec_413),
        .y_values = _vec_414,
        .y_values_len = ARRAY_SIZE(_vec_414),
        .series = _vec_415,
        .series_len = ARRAY_SIZE(_vec_415),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_416,
        .properties_len = ARRAY_SIZE(_vec_416),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1671),
        .unsigned_value = UINT64_C(4294967968),
        .float_value = 673.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_417,
        .bytes_value_len = ARRAY_SIZE(_vec_417),
        .row_count = 66211,
        .column_count = 66212,
        .x_values = _vec_418,
        .x_values_len = ARRAY_SIZE(_vec_418),
        .y_values = _vec_419,
        .y_values_len = ARRAY_SIZE(_vec_419),
        .series = _vec_420,
        .series_len = ARRAY_SIZE(_vec_420),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_421,
        .properties_len = ARRAY_SIZE(_vec_421),
    },
};
static const double                        _vec_423[] = { 681.5, 682.5, 683.5 };
static const double                        _vec_424[] = { 682.5, 683.5, 684.5 };
static const uint8_t                       _vec_425[] = { 175, 176, 177 };
static const double                        _vec_426[] = { 690.5, 691.5, 692.5 };
static const double                        _vec_427[] = { 691.5, 692.5, 693.5 };
static const NCodecDiagXilDiagnosticSeries _vec_428[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_429[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_430[] = { 184, 185, 186 };
static const double                        _vec_431[] = { 699.5, 700.5, 701.5 };
static const double                        _vec_432[] = { 700.5, 701.5, 702.5 };
static const NCodecDiagXilDiagnosticSeries _vec_433[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_434[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_435[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1684),
        .unsigned_value = UINT64_C(4294967981),
        .float_value = 686.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_425,
        .bytes_value_len = ARRAY_SIZE(_vec_425),
        .row_count = 66224,
        .column_count = 66225,
        .x_values = _vec_426,
        .x_values_len = ARRAY_SIZE(_vec_426),
        .y_values = _vec_427,
        .y_values_len = ARRAY_SIZE(_vec_427),
        .series = _vec_428,
        .series_len = ARRAY_SIZE(_vec_428),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_429,
        .properties_len = ARRAY_SIZE(_vec_429),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1693),
        .unsigned_value = UINT64_C(4294967990),
        .float_value = 695.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_430,
        .bytes_value_len = ARRAY_SIZE(_vec_430),
        .row_count = 66233,
        .column_count = 66234,
        .x_values = _vec_431,
        .x_values_len = ARRAY_SIZE(_vec_431),
        .y_values = _vec_432,
        .y_values_len = ARRAY_SIZE(_vec_432),
        .series = _vec_433,
        .series_len = ARRAY_SIZE(_vec_433),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_434,
        .properties_len = ARRAY_SIZE(_vec_434),
    },
};
static const uint8_t                       _vec_436[] = { 193, 194, 195 };
static const double                        _vec_437[] = { 708.5, 709.5, 710.5 };
static const double                        _vec_438[] = { 709.5, 710.5, 711.5 };
static const NCodecDiagXilDiagnosticSeries _vec_439[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_440[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_441[] = { 202, 203, 204 };
static const double                        _vec_442[] = { 717.5, 718.5, 719.5 };
static const double                        _vec_443[] = { 718.5, 719.5, 720.5 };
static const NCodecDiagXilDiagnosticSeries _vec_444[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_445[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_446[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1702),
        .unsigned_value = UINT64_C(4294967999),
        .float_value = 704.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_436,
        .bytes_value_len = ARRAY_SIZE(_vec_436),
        .row_count = 66242,
        .column_count = 66243,
        .x_values = _vec_437,
        .x_values_len = ARRAY_SIZE(_vec_437),
        .y_values = _vec_438,
        .y_values_len = ARRAY_SIZE(_vec_438),
        .series = _vec_439,
        .series_len = ARRAY_SIZE(_vec_439),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_440,
        .properties_len = ARRAY_SIZE(_vec_440),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1711),
        .unsigned_value = UINT64_C(4294968008),
        .float_value = 713.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_441,
        .bytes_value_len = ARRAY_SIZE(_vec_441),
        .row_count = 66251,
        .column_count = 66252,
        .x_values = _vec_442,
        .x_values_len = ARRAY_SIZE(_vec_442),
        .y_values = _vec_443,
        .y_values_len = ARRAY_SIZE(_vec_443),
        .series = _vec_444,
        .series_len = ARRAY_SIZE(_vec_444),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_445,
        .properties_len = ARRAY_SIZE(_vec_445),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_447[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_435,
        .values_len = ARRAY_SIZE(_vec_435),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_446,
        .values_len = ARRAY_SIZE(_vec_446),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_448[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_410 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1657),
    .unsigned_value = UINT64_C(4294967954),
    .float_value = 659.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_411,
    .bytes_value_len = ARRAY_SIZE(_vec_411),
    .elements = _vec_422,
    .elements_len = ARRAY_SIZE(_vec_422),
    .row_count = 66215,
    .column_count = 66216,
    .x_values = _vec_423,
    .x_values_len = ARRAY_SIZE(_vec_423),
    .y_values = _vec_424,
    .y_values_len = ARRAY_SIZE(_vec_424),
    .series = _vec_447,
    .series_len = ARRAY_SIZE(_vec_447),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_448,
    .properties_len = ARRAY_SIZE(_vec_448),
};
static const uint8_t                       _vec_449[] = { 207, 208, 209 };
static const uint8_t                       _vec_451[] = { 212, 213, 214 };
static const uint8_t                       _vec_452[] = { 217, 218, 219 };
static const double                        _vec_453[] = { 732.5, 733.5, 734.5 };
static const double                        _vec_454[] = { 733.5, 734.5, 735.5 };
static const NCodecDiagXilDiagnosticSeries _vec_455[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_456[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_457[] = { 226, 227, 228 };
static const double                        _vec_458[] = { 741.5, 742.5, 743.5 };
static const double                        _vec_459[] = { 742.5, 743.5, 744.5 };
static const NCodecDiagXilDiagnosticSeries _vec_460[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_461[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_462[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1726),
        .unsigned_value = UINT64_C(4294968023),
        .float_value = 728.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_452,
        .bytes_value_len = ARRAY_SIZE(_vec_452),
        .row_count = 66266,
        .column_count = 66267,
        .x_values = _vec_453,
        .x_values_len = ARRAY_SIZE(_vec_453),
        .y_values = _vec_454,
        .y_values_len = ARRAY_SIZE(_vec_454),
        .series = _vec_455,
        .series_len = ARRAY_SIZE(_vec_455),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_456,
        .properties_len = ARRAY_SIZE(_vec_456),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1735),
        .unsigned_value = UINT64_C(4294968032),
        .float_value = 737.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_457,
        .bytes_value_len = ARRAY_SIZE(_vec_457),
        .row_count = 66275,
        .column_count = 66276,
        .x_values = _vec_458,
        .x_values_len = ARRAY_SIZE(_vec_458),
        .y_values = _vec_459,
        .y_values_len = ARRAY_SIZE(_vec_459),
        .series = _vec_460,
        .series_len = ARRAY_SIZE(_vec_460),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_461,
        .properties_len = ARRAY_SIZE(_vec_461),
    },
};
static const double                        _vec_463[] = { 745.5, 746.5, 747.5 };
static const double                        _vec_464[] = { 746.5, 747.5, 748.5 };
static const uint8_t                       _vec_465[] = { 239, 240, 241 };
static const double                        _vec_466[] = { 754.5, 755.5, 756.5 };
static const double                        _vec_467[] = { 755.5, 756.5, 757.5 };
static const NCodecDiagXilDiagnosticSeries _vec_468[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_469[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_470[] = { 248, 249, 250 };
static const double                        _vec_471[] = { 763.5, 764.5, 765.5 };
static const double                        _vec_472[] = { 764.5, 765.5, 766.5 };
static const NCodecDiagXilDiagnosticSeries _vec_473[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_474[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_475[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1748),
        .unsigned_value = UINT64_C(4294968045),
        .float_value = 750.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_465,
        .bytes_value_len = ARRAY_SIZE(_vec_465),
        .row_count = 66288,
        .column_count = 66289,
        .x_values = _vec_466,
        .x_values_len = ARRAY_SIZE(_vec_466),
        .y_values = _vec_467,
        .y_values_len = ARRAY_SIZE(_vec_467),
        .series = _vec_468,
        .series_len = ARRAY_SIZE(_vec_468),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_469,
        .properties_len = ARRAY_SIZE(_vec_469),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1757),
        .unsigned_value = UINT64_C(4294968054),
        .float_value = 759.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_470,
        .bytes_value_len = ARRAY_SIZE(_vec_470),
        .row_count = 66297,
        .column_count = 66298,
        .x_values = _vec_471,
        .x_values_len = ARRAY_SIZE(_vec_471),
        .y_values = _vec_472,
        .y_values_len = ARRAY_SIZE(_vec_472),
        .series = _vec_473,
        .series_len = ARRAY_SIZE(_vec_473),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_474,
        .properties_len = ARRAY_SIZE(_vec_474),
    },
};
static const uint8_t                       _vec_476[] = { 1, 2, 3 };
static const double                        _vec_477[] = { 772.5, 773.5, 774.5 };
static const double                        _vec_478[] = { 773.5, 774.5, 775.5 };
static const NCodecDiagXilDiagnosticSeries _vec_479[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_480[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_481[] = { 10, 11, 12 };
static const double                        _vec_482[] = { 781.5, 782.5, 783.5 };
static const double                        _vec_483[] = { 782.5, 783.5, 784.5 };
static const NCodecDiagXilDiagnosticSeries _vec_484[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_485[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_486[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1766),
        .unsigned_value = UINT64_C(4294968063),
        .float_value = 768.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_476,
        .bytes_value_len = ARRAY_SIZE(_vec_476),
        .row_count = 66306,
        .column_count = 66307,
        .x_values = _vec_477,
        .x_values_len = ARRAY_SIZE(_vec_477),
        .y_values = _vec_478,
        .y_values_len = ARRAY_SIZE(_vec_478),
        .series = _vec_479,
        .series_len = ARRAY_SIZE(_vec_479),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_480,
        .properties_len = ARRAY_SIZE(_vec_480),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1775),
        .unsigned_value = UINT64_C(4294968072),
        .float_value = 777.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_481,
        .bytes_value_len = ARRAY_SIZE(_vec_481),
        .row_count = 66315,
        .column_count = 66316,
        .x_values = _vec_482,
        .x_values_len = ARRAY_SIZE(_vec_482),
        .y_values = _vec_483,
        .y_values_len = ARRAY_SIZE(_vec_483),
        .series = _vec_484,
        .series_len = ARRAY_SIZE(_vec_484),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_485,
        .properties_len = ARRAY_SIZE(_vec_485),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_487[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_475,
        .values_len = ARRAY_SIZE(_vec_475),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_486,
        .values_len = ARRAY_SIZE(_vec_486),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_488[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_450 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1721),
    .unsigned_value = UINT64_C(4294968018),
    .float_value = 723.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_451,
    .bytes_value_len = ARRAY_SIZE(_vec_451),
    .elements = _vec_462,
    .elements_len = ARRAY_SIZE(_vec_462),
    .row_count = 66279,
    .column_count = 66280,
    .x_values = _vec_463,
    .x_values_len = ARRAY_SIZE(_vec_463),
    .y_values = _vec_464,
    .y_values_len = ARRAY_SIZE(_vec_464),
    .series = _vec_487,
    .series_len = ARRAY_SIZE(_vec_487),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_488,
    .properties_len = ARRAY_SIZE(_vec_488),
};
static const NCodecDiagXilDataValue _vec_489[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_409,
        .raw_value_len = ARRAY_SIZE(_vec_409),
        .physical_value = &_obj_410,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_449,
        .raw_value_len = ARRAY_SIZE(_vec_449),
        .physical_value = &_obj_450,
    },
};
static const NCodecDiagXilDiagnosticTroubleCode _vec_490[] = {
    {
        .value = UINT64_C(4294967819),
        .status = 125,
        .short_name = "DiagnosticTroubleCode.short_name",
        .long_name = "DiagnosticTroubleCode.long_name",
        .description = "DiagnosticTroubleCode.description",
        .environment_data = _vec_408,
        .environment_data_len = ARRAY_SIZE(_vec_408),
    },
    {
        .value = UINT64_C(4294967949),
        .status = 55,
        .short_name = "DiagnosticTroubleCode.short_name",
        .long_name = "DiagnosticTroubleCode.long_name",
        .description = "DiagnosticTroubleCode.description",
        .environment_data = _vec_489,
        .environment_data_len = ARRAY_SIZE(_vec_489),
    },
};
static const uint8_t                  _vec_491[] = { 17, 18, 19 };
static const uint8_t                  _vec_492[] = { 19, 20, 21 };
static const NCodecDiagXilDidIdentity _vec_493[] = {
    {
        .name = "DidIdentity.name",
        .identifier = 1044,
    },
    {
        .name = "DidIdentity.name",
        .identifier = 1045,
    },
};
static const uint8_t                       _vec_494[] = { 23, 24, 25 };
static const uint8_t                       _vec_495[] = { 24, 25, 26 };
static const uint8_t                       _vec_497[] = { 29, 30, 31 };
static const uint8_t                       _vec_498[] = { 34, 35, 36 };
static const double                        _vec_499[] = { 805.5, 806.5, 807.5 };
static const double                        _vec_500[] = { 806.5, 807.5, 808.5 };
static const NCodecDiagXilDiagnosticSeries _vec_501[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_502[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_503[] = { 43, 44, 45 };
static const double                        _vec_504[] = { 814.5, 815.5, 816.5 };
static const double                        _vec_505[] = { 815.5, 816.5, 817.5 };
static const NCodecDiagXilDiagnosticSeries _vec_506[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_507[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_508[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1799),
        .unsigned_value = UINT64_C(4294968096),
        .float_value = 801.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_498,
        .bytes_value_len = ARRAY_SIZE(_vec_498),
        .row_count = 66339,
        .column_count = 66340,
        .x_values = _vec_499,
        .x_values_len = ARRAY_SIZE(_vec_499),
        .y_values = _vec_500,
        .y_values_len = ARRAY_SIZE(_vec_500),
        .series = _vec_501,
        .series_len = ARRAY_SIZE(_vec_501),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_502,
        .properties_len = ARRAY_SIZE(_vec_502),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1808),
        .unsigned_value = UINT64_C(4294968105),
        .float_value = 810.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_503,
        .bytes_value_len = ARRAY_SIZE(_vec_503),
        .row_count = 66348,
        .column_count = 66349,
        .x_values = _vec_504,
        .x_values_len = ARRAY_SIZE(_vec_504),
        .y_values = _vec_505,
        .y_values_len = ARRAY_SIZE(_vec_505),
        .series = _vec_506,
        .series_len = ARRAY_SIZE(_vec_506),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_507,
        .properties_len = ARRAY_SIZE(_vec_507),
    },
};
static const double                        _vec_509[] = { 818.5, 819.5, 820.5 };
static const double                        _vec_510[] = { 819.5, 820.5, 821.5 };
static const uint8_t                       _vec_511[] = { 56, 57, 58 };
static const double                        _vec_512[] = { 827.5, 828.5, 829.5 };
static const double                        _vec_513[] = { 828.5, 829.5, 830.5 };
static const NCodecDiagXilDiagnosticSeries _vec_514[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_515[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_516[] = { 65, 66, 67 };
static const double                        _vec_517[] = { 836.5, 837.5, 838.5 };
static const double                        _vec_518[] = { 837.5, 838.5, 839.5 };
static const NCodecDiagXilDiagnosticSeries _vec_519[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_520[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_521[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1821),
        .unsigned_value = UINT64_C(4294968118),
        .float_value = 823.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_511,
        .bytes_value_len = ARRAY_SIZE(_vec_511),
        .row_count = 66361,
        .column_count = 66362,
        .x_values = _vec_512,
        .x_values_len = ARRAY_SIZE(_vec_512),
        .y_values = _vec_513,
        .y_values_len = ARRAY_SIZE(_vec_513),
        .series = _vec_514,
        .series_len = ARRAY_SIZE(_vec_514),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_515,
        .properties_len = ARRAY_SIZE(_vec_515),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1830),
        .unsigned_value = UINT64_C(4294968127),
        .float_value = 832.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_516,
        .bytes_value_len = ARRAY_SIZE(_vec_516),
        .row_count = 66370,
        .column_count = 66371,
        .x_values = _vec_517,
        .x_values_len = ARRAY_SIZE(_vec_517),
        .y_values = _vec_518,
        .y_values_len = ARRAY_SIZE(_vec_518),
        .series = _vec_519,
        .series_len = ARRAY_SIZE(_vec_519),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_520,
        .properties_len = ARRAY_SIZE(_vec_520),
    },
};
static const uint8_t                       _vec_522[] = { 74, 75, 76 };
static const double                        _vec_523[] = { 845.5, 846.5, 847.5 };
static const double                        _vec_524[] = { 846.5, 847.5, 848.5 };
static const NCodecDiagXilDiagnosticSeries _vec_525[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_526[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_527[] = { 83, 84, 85 };
static const double                        _vec_528[] = { 854.5, 855.5, 856.5 };
static const double                        _vec_529[] = { 855.5, 856.5, 857.5 };
static const NCodecDiagXilDiagnosticSeries _vec_530[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_531[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_532[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1839),
        .unsigned_value = UINT64_C(4294968136),
        .float_value = 841.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_522,
        .bytes_value_len = ARRAY_SIZE(_vec_522),
        .row_count = 66379,
        .column_count = 66380,
        .x_values = _vec_523,
        .x_values_len = ARRAY_SIZE(_vec_523),
        .y_values = _vec_524,
        .y_values_len = ARRAY_SIZE(_vec_524),
        .series = _vec_525,
        .series_len = ARRAY_SIZE(_vec_525),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_526,
        .properties_len = ARRAY_SIZE(_vec_526),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1848),
        .unsigned_value = UINT64_C(4294968145),
        .float_value = 850.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_527,
        .bytes_value_len = ARRAY_SIZE(_vec_527),
        .row_count = 66388,
        .column_count = 66389,
        .x_values = _vec_528,
        .x_values_len = ARRAY_SIZE(_vec_528),
        .y_values = _vec_529,
        .y_values_len = ARRAY_SIZE(_vec_529),
        .series = _vec_530,
        .series_len = ARRAY_SIZE(_vec_530),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_531,
        .properties_len = ARRAY_SIZE(_vec_531),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_533[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_521,
        .values_len = ARRAY_SIZE(_vec_521),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_532,
        .values_len = ARRAY_SIZE(_vec_532),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_534[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_496 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1794),
    .unsigned_value = UINT64_C(4294968091),
    .float_value = 796.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_497,
    .bytes_value_len = ARRAY_SIZE(_vec_497),
    .elements = _vec_508,
    .elements_len = ARRAY_SIZE(_vec_508),
    .row_count = 66352,
    .column_count = 66353,
    .x_values = _vec_509,
    .x_values_len = ARRAY_SIZE(_vec_509),
    .y_values = _vec_510,
    .y_values_len = ARRAY_SIZE(_vec_510),
    .series = _vec_533,
    .series_len = ARRAY_SIZE(_vec_533),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_534,
    .properties_len = ARRAY_SIZE(_vec_534),
};
static const uint8_t                       _vec_535[] = { 88, 89, 90 };
static const uint8_t                       _vec_537[] = { 93, 94, 95 };
static const uint8_t                       _vec_538[] = { 98, 99, 100 };
static const double                        _vec_539[] = { 869.5, 870.5, 871.5 };
static const double                        _vec_540[] = { 870.5, 871.5, 872.5 };
static const NCodecDiagXilDiagnosticSeries _vec_541[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_542[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_543[] = { 107, 108, 109 };
static const double                        _vec_544[] = { 878.5, 879.5, 880.5 };
static const double                        _vec_545[] = { 879.5, 880.5, 881.5 };
static const NCodecDiagXilDiagnosticSeries _vec_546[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_547[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_548[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1863),
        .unsigned_value = UINT64_C(4294968160),
        .float_value = 865.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_538,
        .bytes_value_len = ARRAY_SIZE(_vec_538),
        .row_count = 66403,
        .column_count = 66404,
        .x_values = _vec_539,
        .x_values_len = ARRAY_SIZE(_vec_539),
        .y_values = _vec_540,
        .y_values_len = ARRAY_SIZE(_vec_540),
        .series = _vec_541,
        .series_len = ARRAY_SIZE(_vec_541),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_542,
        .properties_len = ARRAY_SIZE(_vec_542),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1872),
        .unsigned_value = UINT64_C(4294968169),
        .float_value = 874.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_543,
        .bytes_value_len = ARRAY_SIZE(_vec_543),
        .row_count = 66412,
        .column_count = 66413,
        .x_values = _vec_544,
        .x_values_len = ARRAY_SIZE(_vec_544),
        .y_values = _vec_545,
        .y_values_len = ARRAY_SIZE(_vec_545),
        .series = _vec_546,
        .series_len = ARRAY_SIZE(_vec_546),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_547,
        .properties_len = ARRAY_SIZE(_vec_547),
    },
};
static const double                        _vec_549[] = { 882.5, 883.5, 884.5 };
static const double                        _vec_550[] = { 883.5, 884.5, 885.5 };
static const uint8_t                       _vec_551[] = { 120, 121, 122 };
static const double                        _vec_552[] = { 891.5, 892.5, 893.5 };
static const double                        _vec_553[] = { 892.5, 893.5, 894.5 };
static const NCodecDiagXilDiagnosticSeries _vec_554[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_555[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_556[] = { 129, 130, 131 };
static const double                        _vec_557[] = { 900.5, 901.5, 902.5 };
static const double                        _vec_558[] = { 901.5, 902.5, 903.5 };
static const NCodecDiagXilDiagnosticSeries _vec_559[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_560[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_561[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1885),
        .unsigned_value = UINT64_C(4294968182),
        .float_value = 887.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_551,
        .bytes_value_len = ARRAY_SIZE(_vec_551),
        .row_count = 66425,
        .column_count = 66426,
        .x_values = _vec_552,
        .x_values_len = ARRAY_SIZE(_vec_552),
        .y_values = _vec_553,
        .y_values_len = ARRAY_SIZE(_vec_553),
        .series = _vec_554,
        .series_len = ARRAY_SIZE(_vec_554),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_555,
        .properties_len = ARRAY_SIZE(_vec_555),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1894),
        .unsigned_value = UINT64_C(4294968191),
        .float_value = 896.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_556,
        .bytes_value_len = ARRAY_SIZE(_vec_556),
        .row_count = 66434,
        .column_count = 66435,
        .x_values = _vec_557,
        .x_values_len = ARRAY_SIZE(_vec_557),
        .y_values = _vec_558,
        .y_values_len = ARRAY_SIZE(_vec_558),
        .series = _vec_559,
        .series_len = ARRAY_SIZE(_vec_559),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_560,
        .properties_len = ARRAY_SIZE(_vec_560),
    },
};
static const uint8_t                       _vec_562[] = { 138, 139, 140 };
static const double                        _vec_563[] = { 909.5, 910.5, 911.5 };
static const double                        _vec_564[] = { 910.5, 911.5, 912.5 };
static const NCodecDiagXilDiagnosticSeries _vec_565[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_566[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_567[] = { 147, 148, 149 };
static const double                        _vec_568[] = { 918.5, 919.5, 920.5 };
static const double                        _vec_569[] = { 919.5, 920.5, 921.5 };
static const NCodecDiagXilDiagnosticSeries _vec_570[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_571[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_572[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1903),
        .unsigned_value = UINT64_C(4294968200),
        .float_value = 905.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_562,
        .bytes_value_len = ARRAY_SIZE(_vec_562),
        .row_count = 66443,
        .column_count = 66444,
        .x_values = _vec_563,
        .x_values_len = ARRAY_SIZE(_vec_563),
        .y_values = _vec_564,
        .y_values_len = ARRAY_SIZE(_vec_564),
        .series = _vec_565,
        .series_len = ARRAY_SIZE(_vec_565),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_566,
        .properties_len = ARRAY_SIZE(_vec_566),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1912),
        .unsigned_value = UINT64_C(4294968209),
        .float_value = 914.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_567,
        .bytes_value_len = ARRAY_SIZE(_vec_567),
        .row_count = 66452,
        .column_count = 66453,
        .x_values = _vec_568,
        .x_values_len = ARRAY_SIZE(_vec_568),
        .y_values = _vec_569,
        .y_values_len = ARRAY_SIZE(_vec_569),
        .series = _vec_570,
        .series_len = ARRAY_SIZE(_vec_570),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_571,
        .properties_len = ARRAY_SIZE(_vec_571),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_573[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_561,
        .values_len = ARRAY_SIZE(_vec_561),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_572,
        .values_len = ARRAY_SIZE(_vec_572),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_574[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_536 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1858),
    .unsigned_value = UINT64_C(4294968155),
    .float_value = 860.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_537,
    .bytes_value_len = ARRAY_SIZE(_vec_537),
    .elements = _vec_548,
    .elements_len = ARRAY_SIZE(_vec_548),
    .row_count = 66416,
    .column_count = 66417,
    .x_values = _vec_549,
    .x_values_len = ARRAY_SIZE(_vec_549),
    .y_values = _vec_550,
    .y_values_len = ARRAY_SIZE(_vec_550),
    .series = _vec_573,
    .series_len = ARRAY_SIZE(_vec_573),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_574,
    .properties_len = ARRAY_SIZE(_vec_574),
};
static const NCodecDiagXilDataValue _vec_575[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_495,
        .raw_value_len = ARRAY_SIZE(_vec_495),
        .physical_value = &_obj_496,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_535,
        .raw_value_len = ARRAY_SIZE(_vec_535),
        .physical_value = &_obj_536,
    },
};
static const NCodecDiagXilDidIdentity _obj_576 = {
    .name = "DidIdentity.name",
    .identifier = 1176,
};
static const uint8_t                       _vec_577[] = { 154, 155, 156 };
static const uint8_t                       _vec_578[] = { 155, 156, 157 };
static const uint8_t                       _vec_580[] = { 160, 161, 162 };
static const uint8_t                       _vec_581[] = { 165, 166, 167 };
static const double                        _vec_582[] = { 936.5, 937.5, 938.5 };
static const double                        _vec_583[] = { 937.5, 938.5, 939.5 };
static const NCodecDiagXilDiagnosticSeries _vec_584[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_585[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_586[] = { 174, 175, 176 };
static const double                        _vec_587[] = { 945.5, 946.5, 947.5 };
static const double                        _vec_588[] = { 946.5, 947.5, 948.5 };
static const NCodecDiagXilDiagnosticSeries _vec_589[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_590[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_591[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1930),
        .unsigned_value = UINT64_C(4294968227),
        .float_value = 932.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_581,
        .bytes_value_len = ARRAY_SIZE(_vec_581),
        .row_count = 66470,
        .column_count = 66471,
        .x_values = _vec_582,
        .x_values_len = ARRAY_SIZE(_vec_582),
        .y_values = _vec_583,
        .y_values_len = ARRAY_SIZE(_vec_583),
        .series = _vec_584,
        .series_len = ARRAY_SIZE(_vec_584),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_585,
        .properties_len = ARRAY_SIZE(_vec_585),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1939),
        .unsigned_value = UINT64_C(4294968236),
        .float_value = 941.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_586,
        .bytes_value_len = ARRAY_SIZE(_vec_586),
        .row_count = 66479,
        .column_count = 66480,
        .x_values = _vec_587,
        .x_values_len = ARRAY_SIZE(_vec_587),
        .y_values = _vec_588,
        .y_values_len = ARRAY_SIZE(_vec_588),
        .series = _vec_589,
        .series_len = ARRAY_SIZE(_vec_589),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_590,
        .properties_len = ARRAY_SIZE(_vec_590),
    },
};
static const double                        _vec_592[] = { 949.5, 950.5, 951.5 };
static const double                        _vec_593[] = { 950.5, 951.5, 952.5 };
static const uint8_t                       _vec_594[] = { 187, 188, 189 };
static const double                        _vec_595[] = { 958.5, 959.5, 960.5 };
static const double                        _vec_596[] = { 959.5, 960.5, 961.5 };
static const NCodecDiagXilDiagnosticSeries _vec_597[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_598[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_599[] = { 196, 197, 198 };
static const double                        _vec_600[] = { 967.5, 968.5, 969.5 };
static const double                        _vec_601[] = { 968.5, 969.5, 970.5 };
static const NCodecDiagXilDiagnosticSeries _vec_602[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_603[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_604[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1952),
        .unsigned_value = UINT64_C(4294968249),
        .float_value = 954.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_594,
        .bytes_value_len = ARRAY_SIZE(_vec_594),
        .row_count = 66492,
        .column_count = 66493,
        .x_values = _vec_595,
        .x_values_len = ARRAY_SIZE(_vec_595),
        .y_values = _vec_596,
        .y_values_len = ARRAY_SIZE(_vec_596),
        .series = _vec_597,
        .series_len = ARRAY_SIZE(_vec_597),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_598,
        .properties_len = ARRAY_SIZE(_vec_598),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1961),
        .unsigned_value = UINT64_C(4294968258),
        .float_value = 963.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_599,
        .bytes_value_len = ARRAY_SIZE(_vec_599),
        .row_count = 66501,
        .column_count = 66502,
        .x_values = _vec_600,
        .x_values_len = ARRAY_SIZE(_vec_600),
        .y_values = _vec_601,
        .y_values_len = ARRAY_SIZE(_vec_601),
        .series = _vec_602,
        .series_len = ARRAY_SIZE(_vec_602),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_603,
        .properties_len = ARRAY_SIZE(_vec_603),
    },
};
static const uint8_t                       _vec_605[] = { 205, 206, 207 };
static const double                        _vec_606[] = { 976.5, 977.5, 978.5 };
static const double                        _vec_607[] = { 977.5, 978.5, 979.5 };
static const NCodecDiagXilDiagnosticSeries _vec_608[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_609[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t                       _vec_610[] = { 214, 215, 216 };
static const double                        _vec_611[] = { 985.5, 986.5, 987.5 };
static const double                        _vec_612[] = { 986.5, 987.5, 988.5 };
static const NCodecDiagXilDiagnosticSeries _vec_613[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_614[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_615[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1970),
        .unsigned_value = UINT64_C(4294968267),
        .float_value = 972.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_605,
        .bytes_value_len = ARRAY_SIZE(_vec_605),
        .row_count = 66510,
        .column_count = 66511,
        .x_values = _vec_606,
        .x_values_len = ARRAY_SIZE(_vec_606),
        .y_values = _vec_607,
        .y_values_len = ARRAY_SIZE(_vec_607),
        .series = _vec_608,
        .series_len = ARRAY_SIZE(_vec_608),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_609,
        .properties_len = ARRAY_SIZE(_vec_609),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1979),
        .unsigned_value = UINT64_C(4294968276),
        .float_value = 981.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_610,
        .bytes_value_len = ARRAY_SIZE(_vec_610),
        .row_count = 66519,
        .column_count = 66520,
        .x_values = _vec_611,
        .x_values_len = ARRAY_SIZE(_vec_611),
        .y_values = _vec_612,
        .y_values_len = ARRAY_SIZE(_vec_612),
        .series = _vec_613,
        .series_len = ARRAY_SIZE(_vec_613),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_614,
        .properties_len = ARRAY_SIZE(_vec_614),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_616[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_604,
        .values_len = ARRAY_SIZE(_vec_604),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_615,
        .values_len = ARRAY_SIZE(_vec_615),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_617[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_579 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1925),
    .unsigned_value = UINT64_C(4294968222),
    .float_value = 927.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_580,
    .bytes_value_len = ARRAY_SIZE(_vec_580),
    .elements = _vec_591,
    .elements_len = ARRAY_SIZE(_vec_591),
    .row_count = 66483,
    .column_count = 66484,
    .x_values = _vec_592,
    .x_values_len = ARRAY_SIZE(_vec_592),
    .y_values = _vec_593,
    .y_values_len = ARRAY_SIZE(_vec_593),
    .series = _vec_616,
    .series_len = ARRAY_SIZE(_vec_616),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_617,
    .properties_len = ARRAY_SIZE(_vec_617),
};
static const uint8_t _vec_618[] = { 219, 220, 221 };
static const uint8_t _vec_620[] = { 224, 225, 226 };
static const uint8_t _vec_621[] = { 229, 230, 231 };
static const double  _vec_622[] = { 1000.5, 1001.5, 1002.5 };
static const double  _vec_623[] = { 1001.5, 1002.5, 1003.5 };
static const NCodecDiagXilDiagnosticSeries _vec_624[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_625[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_626[] = { 238, 239, 240 };
static const double  _vec_627[] = { 1009.5, 1010.5, 1011.5 };
static const double  _vec_628[] = { 1010.5, 1011.5, 1012.5 };
static const NCodecDiagXilDiagnosticSeries _vec_629[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_630[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_631[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-1994),
        .unsigned_value = UINT64_C(4294968291),
        .float_value = 996.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_621,
        .bytes_value_len = ARRAY_SIZE(_vec_621),
        .row_count = 66534,
        .column_count = 66535,
        .x_values = _vec_622,
        .x_values_len = ARRAY_SIZE(_vec_622),
        .y_values = _vec_623,
        .y_values_len = ARRAY_SIZE(_vec_623),
        .series = _vec_624,
        .series_len = ARRAY_SIZE(_vec_624),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_625,
        .properties_len = ARRAY_SIZE(_vec_625),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2003),
        .unsigned_value = UINT64_C(4294968300),
        .float_value = 1005.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_626,
        .bytes_value_len = ARRAY_SIZE(_vec_626),
        .row_count = 66543,
        .column_count = 66544,
        .x_values = _vec_627,
        .x_values_len = ARRAY_SIZE(_vec_627),
        .y_values = _vec_628,
        .y_values_len = ARRAY_SIZE(_vec_628),
        .series = _vec_629,
        .series_len = ARRAY_SIZE(_vec_629),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_630,
        .properties_len = ARRAY_SIZE(_vec_630),
    },
};
static const double  _vec_632[] = { 1013.5, 1014.5, 1015.5 };
static const double  _vec_633[] = { 1014.5, 1015.5, 1016.5 };
static const uint8_t _vec_634[] = { 251, 252, 253 };
static const double  _vec_635[] = { 1022.5, 1023.5, 1024.5 };
static const double  _vec_636[] = { 1023.5, 1024.5, 1025.5 };
static const NCodecDiagXilDiagnosticSeries _vec_637[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_638[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_639[] = { 4, 5, 6 };
static const double  _vec_640[] = { 1031.5, 1032.5, 1033.5 };
static const double  _vec_641[] = { 1032.5, 1033.5, 1034.5 };
static const NCodecDiagXilDiagnosticSeries _vec_642[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_643[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_644[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2016),
        .unsigned_value = UINT64_C(4294968313),
        .float_value = 1018.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_634,
        .bytes_value_len = ARRAY_SIZE(_vec_634),
        .row_count = 66556,
        .column_count = 66557,
        .x_values = _vec_635,
        .x_values_len = ARRAY_SIZE(_vec_635),
        .y_values = _vec_636,
        .y_values_len = ARRAY_SIZE(_vec_636),
        .series = _vec_637,
        .series_len = ARRAY_SIZE(_vec_637),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_638,
        .properties_len = ARRAY_SIZE(_vec_638),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2025),
        .unsigned_value = UINT64_C(4294968322),
        .float_value = 1027.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_639,
        .bytes_value_len = ARRAY_SIZE(_vec_639),
        .row_count = 66565,
        .column_count = 66566,
        .x_values = _vec_640,
        .x_values_len = ARRAY_SIZE(_vec_640),
        .y_values = _vec_641,
        .y_values_len = ARRAY_SIZE(_vec_641),
        .series = _vec_642,
        .series_len = ARRAY_SIZE(_vec_642),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_643,
        .properties_len = ARRAY_SIZE(_vec_643),
    },
};
static const uint8_t _vec_645[] = { 13, 14, 15 };
static const double  _vec_646[] = { 1040.5, 1041.5, 1042.5 };
static const double  _vec_647[] = { 1041.5, 1042.5, 1043.5 };
static const NCodecDiagXilDiagnosticSeries _vec_648[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_649[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_650[] = { 22, 23, 24 };
static const double  _vec_651[] = { 1049.5, 1050.5, 1051.5 };
static const double  _vec_652[] = { 1050.5, 1051.5, 1052.5 };
static const NCodecDiagXilDiagnosticSeries _vec_653[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_654[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_655[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2034),
        .unsigned_value = UINT64_C(4294968331),
        .float_value = 1036.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_645,
        .bytes_value_len = ARRAY_SIZE(_vec_645),
        .row_count = 66574,
        .column_count = 66575,
        .x_values = _vec_646,
        .x_values_len = ARRAY_SIZE(_vec_646),
        .y_values = _vec_647,
        .y_values_len = ARRAY_SIZE(_vec_647),
        .series = _vec_648,
        .series_len = ARRAY_SIZE(_vec_648),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_649,
        .properties_len = ARRAY_SIZE(_vec_649),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2043),
        .unsigned_value = UINT64_C(4294968340),
        .float_value = 1045.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_650,
        .bytes_value_len = ARRAY_SIZE(_vec_650),
        .row_count = 66583,
        .column_count = 66584,
        .x_values = _vec_651,
        .x_values_len = ARRAY_SIZE(_vec_651),
        .y_values = _vec_652,
        .y_values_len = ARRAY_SIZE(_vec_652),
        .series = _vec_653,
        .series_len = ARRAY_SIZE(_vec_653),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_654,
        .properties_len = ARRAY_SIZE(_vec_654),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_656[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_644,
        .values_len = ARRAY_SIZE(_vec_644),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_655,
        .values_len = ARRAY_SIZE(_vec_655),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_657[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_619 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-1989),
    .unsigned_value = UINT64_C(4294968286),
    .float_value = 991.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_620,
    .bytes_value_len = ARRAY_SIZE(_vec_620),
    .elements = _vec_631,
    .elements_len = ARRAY_SIZE(_vec_631),
    .row_count = 66547,
    .column_count = 66548,
    .x_values = _vec_632,
    .x_values_len = ARRAY_SIZE(_vec_632),
    .y_values = _vec_633,
    .y_values_len = ARRAY_SIZE(_vec_633),
    .series = _vec_656,
    .series_len = ARRAY_SIZE(_vec_656),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_657,
    .properties_len = ARRAY_SIZE(_vec_657),
};
static const NCodecDiagXilDataValue _vec_658[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_578,
        .raw_value_len = ARRAY_SIZE(_vec_578),
        .physical_value = &_obj_579,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_618,
        .raw_value_len = ARRAY_SIZE(_vec_618),
        .physical_value = &_obj_619,
    },
};
static const NCodecDiagXilDidIdentity _obj_659 = {
    .name = "DidIdentity.name",
    .identifier = 1307,
};
static const NCodecDiagXilDidValue _vec_660[] = {
    {
        .did = {
            .kind = NCodecDiagXilDidSelectorKindName,
            .identifier = 1046,
            .name = "DidSelector.name",
        },
        .raw_value = _vec_494,
        .raw_value_len = ARRAY_SIZE(_vec_494),
        .values = _vec_575,
        .values_len = ARRAY_SIZE(_vec_575),
        .identity = &_obj_576,
    },
    {
        .did = {
            .kind = NCodecDiagXilDidSelectorKindName,
            .identifier = 1177,
            .name = "DidSelector.name",
        },
        .raw_value = _vec_577,
        .raw_value_len = ARRAY_SIZE(_vec_577),
        .values = _vec_658,
        .values_len = ARRAY_SIZE(_vec_658),
        .identity = &_obj_659,
    },
};
static const uint8_t _vec_661[] = { 29, 30, 31 };
static const uint8_t _vec_662[] = { 30, 31, 32 };
static const uint8_t _vec_664[] = { 35, 36, 37 };
static const uint8_t _vec_665[] = { 40, 41, 42 };
static const double  _vec_666[] = { 1067.5, 1068.5, 1069.5 };
static const double  _vec_667[] = { 1068.5, 1069.5, 1070.5 };
static const NCodecDiagXilDiagnosticSeries _vec_668[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_669[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_670[] = { 49, 50, 51 };
static const double  _vec_671[] = { 1076.5, 1077.5, 1078.5 };
static const double  _vec_672[] = { 1077.5, 1078.5, 1079.5 };
static const NCodecDiagXilDiagnosticSeries _vec_673[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_674[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_675[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2061),
        .unsigned_value = UINT64_C(4294968358),
        .float_value = 1063.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_665,
        .bytes_value_len = ARRAY_SIZE(_vec_665),
        .row_count = 66601,
        .column_count = 66602,
        .x_values = _vec_666,
        .x_values_len = ARRAY_SIZE(_vec_666),
        .y_values = _vec_667,
        .y_values_len = ARRAY_SIZE(_vec_667),
        .series = _vec_668,
        .series_len = ARRAY_SIZE(_vec_668),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_669,
        .properties_len = ARRAY_SIZE(_vec_669),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2070),
        .unsigned_value = UINT64_C(4294968367),
        .float_value = 1072.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_670,
        .bytes_value_len = ARRAY_SIZE(_vec_670),
        .row_count = 66610,
        .column_count = 66611,
        .x_values = _vec_671,
        .x_values_len = ARRAY_SIZE(_vec_671),
        .y_values = _vec_672,
        .y_values_len = ARRAY_SIZE(_vec_672),
        .series = _vec_673,
        .series_len = ARRAY_SIZE(_vec_673),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_674,
        .properties_len = ARRAY_SIZE(_vec_674),
    },
};
static const double  _vec_676[] = { 1080.5, 1081.5, 1082.5 };
static const double  _vec_677[] = { 1081.5, 1082.5, 1083.5 };
static const uint8_t _vec_678[] = { 62, 63, 64 };
static const double  _vec_679[] = { 1089.5, 1090.5, 1091.5 };
static const double  _vec_680[] = { 1090.5, 1091.5, 1092.5 };
static const NCodecDiagXilDiagnosticSeries _vec_681[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_682[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_683[] = { 71, 72, 73 };
static const double  _vec_684[] = { 1098.5, 1099.5, 1100.5 };
static const double  _vec_685[] = { 1099.5, 1100.5, 1101.5 };
static const NCodecDiagXilDiagnosticSeries _vec_686[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_687[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_688[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2083),
        .unsigned_value = UINT64_C(4294968380),
        .float_value = 1085.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_678,
        .bytes_value_len = ARRAY_SIZE(_vec_678),
        .row_count = 66623,
        .column_count = 66624,
        .x_values = _vec_679,
        .x_values_len = ARRAY_SIZE(_vec_679),
        .y_values = _vec_680,
        .y_values_len = ARRAY_SIZE(_vec_680),
        .series = _vec_681,
        .series_len = ARRAY_SIZE(_vec_681),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_682,
        .properties_len = ARRAY_SIZE(_vec_682),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2092),
        .unsigned_value = UINT64_C(4294968389),
        .float_value = 1094.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_683,
        .bytes_value_len = ARRAY_SIZE(_vec_683),
        .row_count = 66632,
        .column_count = 66633,
        .x_values = _vec_684,
        .x_values_len = ARRAY_SIZE(_vec_684),
        .y_values = _vec_685,
        .y_values_len = ARRAY_SIZE(_vec_685),
        .series = _vec_686,
        .series_len = ARRAY_SIZE(_vec_686),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_687,
        .properties_len = ARRAY_SIZE(_vec_687),
    },
};
static const uint8_t _vec_689[] = { 80, 81, 82 };
static const double  _vec_690[] = { 1107.5, 1108.5, 1109.5 };
static const double  _vec_691[] = { 1108.5, 1109.5, 1110.5 };
static const NCodecDiagXilDiagnosticSeries _vec_692[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_693[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_694[] = { 89, 90, 91 };
static const double  _vec_695[] = { 1116.5, 1117.5, 1118.5 };
static const double  _vec_696[] = { 1117.5, 1118.5, 1119.5 };
static const NCodecDiagXilDiagnosticSeries _vec_697[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_698[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_699[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2101),
        .unsigned_value = UINT64_C(4294968398),
        .float_value = 1103.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_689,
        .bytes_value_len = ARRAY_SIZE(_vec_689),
        .row_count = 66641,
        .column_count = 66642,
        .x_values = _vec_690,
        .x_values_len = ARRAY_SIZE(_vec_690),
        .y_values = _vec_691,
        .y_values_len = ARRAY_SIZE(_vec_691),
        .series = _vec_692,
        .series_len = ARRAY_SIZE(_vec_692),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_693,
        .properties_len = ARRAY_SIZE(_vec_693),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2110),
        .unsigned_value = UINT64_C(4294968407),
        .float_value = 1112.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_694,
        .bytes_value_len = ARRAY_SIZE(_vec_694),
        .row_count = 66650,
        .column_count = 66651,
        .x_values = _vec_695,
        .x_values_len = ARRAY_SIZE(_vec_695),
        .y_values = _vec_696,
        .y_values_len = ARRAY_SIZE(_vec_696),
        .series = _vec_697,
        .series_len = ARRAY_SIZE(_vec_697),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_698,
        .properties_len = ARRAY_SIZE(_vec_698),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_700[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_688,
        .values_len = ARRAY_SIZE(_vec_688),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_699,
        .values_len = ARRAY_SIZE(_vec_699),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_701[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_663 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-2056),
    .unsigned_value = UINT64_C(4294968353),
    .float_value = 1058.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_664,
    .bytes_value_len = ARRAY_SIZE(_vec_664),
    .elements = _vec_675,
    .elements_len = ARRAY_SIZE(_vec_675),
    .row_count = 66614,
    .column_count = 66615,
    .x_values = _vec_676,
    .x_values_len = ARRAY_SIZE(_vec_676),
    .y_values = _vec_677,
    .y_values_len = ARRAY_SIZE(_vec_677),
    .series = _vec_700,
    .series_len = ARRAY_SIZE(_vec_700),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_701,
    .properties_len = ARRAY_SIZE(_vec_701),
};
static const uint8_t _vec_702[] = { 94, 95, 96 };
static const uint8_t _vec_704[] = { 99, 100, 101 };
static const uint8_t _vec_705[] = { 104, 105, 106 };
static const double  _vec_706[] = { 1131.5, 1132.5, 1133.5 };
static const double  _vec_707[] = { 1132.5, 1133.5, 1134.5 };
static const NCodecDiagXilDiagnosticSeries _vec_708[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_709[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_710[] = { 113, 114, 115 };
static const double  _vec_711[] = { 1140.5, 1141.5, 1142.5 };
static const double  _vec_712[] = { 1141.5, 1142.5, 1143.5 };
static const NCodecDiagXilDiagnosticSeries _vec_713[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_714[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_715[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2125),
        .unsigned_value = UINT64_C(4294968422),
        .float_value = 1127.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_705,
        .bytes_value_len = ARRAY_SIZE(_vec_705),
        .row_count = 66665,
        .column_count = 66666,
        .x_values = _vec_706,
        .x_values_len = ARRAY_SIZE(_vec_706),
        .y_values = _vec_707,
        .y_values_len = ARRAY_SIZE(_vec_707),
        .series = _vec_708,
        .series_len = ARRAY_SIZE(_vec_708),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_709,
        .properties_len = ARRAY_SIZE(_vec_709),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2134),
        .unsigned_value = UINT64_C(4294968431),
        .float_value = 1136.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_710,
        .bytes_value_len = ARRAY_SIZE(_vec_710),
        .row_count = 66674,
        .column_count = 66675,
        .x_values = _vec_711,
        .x_values_len = ARRAY_SIZE(_vec_711),
        .y_values = _vec_712,
        .y_values_len = ARRAY_SIZE(_vec_712),
        .series = _vec_713,
        .series_len = ARRAY_SIZE(_vec_713),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_714,
        .properties_len = ARRAY_SIZE(_vec_714),
    },
};
static const double  _vec_716[] = { 1144.5, 1145.5, 1146.5 };
static const double  _vec_717[] = { 1145.5, 1146.5, 1147.5 };
static const uint8_t _vec_718[] = { 126, 127, 128 };
static const double  _vec_719[] = { 1153.5, 1154.5, 1155.5 };
static const double  _vec_720[] = { 1154.5, 1155.5, 1156.5 };
static const NCodecDiagXilDiagnosticSeries _vec_721[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_722[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_723[] = { 135, 136, 137 };
static const double  _vec_724[] = { 1162.5, 1163.5, 1164.5 };
static const double  _vec_725[] = { 1163.5, 1164.5, 1165.5 };
static const NCodecDiagXilDiagnosticSeries _vec_726[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_727[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_728[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2147),
        .unsigned_value = UINT64_C(4294968444),
        .float_value = 1149.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_718,
        .bytes_value_len = ARRAY_SIZE(_vec_718),
        .row_count = 66687,
        .column_count = 66688,
        .x_values = _vec_719,
        .x_values_len = ARRAY_SIZE(_vec_719),
        .y_values = _vec_720,
        .y_values_len = ARRAY_SIZE(_vec_720),
        .series = _vec_721,
        .series_len = ARRAY_SIZE(_vec_721),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_722,
        .properties_len = ARRAY_SIZE(_vec_722),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2156),
        .unsigned_value = UINT64_C(4294968453),
        .float_value = 1158.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_723,
        .bytes_value_len = ARRAY_SIZE(_vec_723),
        .row_count = 66696,
        .column_count = 66697,
        .x_values = _vec_724,
        .x_values_len = ARRAY_SIZE(_vec_724),
        .y_values = _vec_725,
        .y_values_len = ARRAY_SIZE(_vec_725),
        .series = _vec_726,
        .series_len = ARRAY_SIZE(_vec_726),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_727,
        .properties_len = ARRAY_SIZE(_vec_727),
    },
};
static const uint8_t _vec_729[] = { 144, 145, 146 };
static const double  _vec_730[] = { 1171.5, 1172.5, 1173.5 };
static const double  _vec_731[] = { 1172.5, 1173.5, 1174.5 };
static const NCodecDiagXilDiagnosticSeries _vec_732[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_733[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_734[] = { 153, 154, 155 };
static const double  _vec_735[] = { 1180.5, 1181.5, 1182.5 };
static const double  _vec_736[] = { 1181.5, 1182.5, 1183.5 };
static const NCodecDiagXilDiagnosticSeries _vec_737[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_738[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_739[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2165),
        .unsigned_value = UINT64_C(4294968462),
        .float_value = 1167.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_729,
        .bytes_value_len = ARRAY_SIZE(_vec_729),
        .row_count = 66705,
        .column_count = 66706,
        .x_values = _vec_730,
        .x_values_len = ARRAY_SIZE(_vec_730),
        .y_values = _vec_731,
        .y_values_len = ARRAY_SIZE(_vec_731),
        .series = _vec_732,
        .series_len = ARRAY_SIZE(_vec_732),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_733,
        .properties_len = ARRAY_SIZE(_vec_733),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2174),
        .unsigned_value = UINT64_C(4294968471),
        .float_value = 1176.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_734,
        .bytes_value_len = ARRAY_SIZE(_vec_734),
        .row_count = 66714,
        .column_count = 66715,
        .x_values = _vec_735,
        .x_values_len = ARRAY_SIZE(_vec_735),
        .y_values = _vec_736,
        .y_values_len = ARRAY_SIZE(_vec_736),
        .series = _vec_737,
        .series_len = ARRAY_SIZE(_vec_737),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_738,
        .properties_len = ARRAY_SIZE(_vec_738),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_740[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_728,
        .values_len = ARRAY_SIZE(_vec_728),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_739,
        .values_len = ARRAY_SIZE(_vec_739),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_741[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_703 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-2120),
    .unsigned_value = UINT64_C(4294968417),
    .float_value = 1122.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_704,
    .bytes_value_len = ARRAY_SIZE(_vec_704),
    .elements = _vec_715,
    .elements_len = ARRAY_SIZE(_vec_715),
    .row_count = 66678,
    .column_count = 66679,
    .x_values = _vec_716,
    .x_values_len = ARRAY_SIZE(_vec_716),
    .y_values = _vec_717,
    .y_values_len = ARRAY_SIZE(_vec_717),
    .series = _vec_740,
    .series_len = ARRAY_SIZE(_vec_740),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_741,
    .properties_len = ARRAY_SIZE(_vec_741),
};
static const NCodecDiagXilDataValue _vec_742[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_662,
        .raw_value_len = ARRAY_SIZE(_vec_662),
        .physical_value = &_obj_663,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_702,
        .raw_value_len = ARRAY_SIZE(_vec_702),
        .physical_value = &_obj_703,
    },
};
static const NCodecDiagXilDidIdentity _obj_743 = {
    .name = "DidIdentity.name",
    .identifier = 1438,
};
static const uint8_t _vec_744[] = { 160, 161, 162 };
static const uint8_t _vec_745[] = { 161, 162, 163 };
static const uint8_t _vec_747[] = { 166, 167, 168 };
static const uint8_t _vec_748[] = { 171, 172, 173 };
static const double  _vec_749[] = { 1198.5, 1199.5, 1200.5 };
static const double  _vec_750[] = { 1199.5, 1200.5, 1201.5 };
static const NCodecDiagXilDiagnosticSeries _vec_751[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_752[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_753[] = { 180, 181, 182 };
static const double  _vec_754[] = { 1207.5, 1208.5, 1209.5 };
static const double  _vec_755[] = { 1208.5, 1209.5, 1210.5 };
static const NCodecDiagXilDiagnosticSeries _vec_756[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_757[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_758[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2192),
        .unsigned_value = UINT64_C(4294968489),
        .float_value = 1194.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_748,
        .bytes_value_len = ARRAY_SIZE(_vec_748),
        .row_count = 66732,
        .column_count = 66733,
        .x_values = _vec_749,
        .x_values_len = ARRAY_SIZE(_vec_749),
        .y_values = _vec_750,
        .y_values_len = ARRAY_SIZE(_vec_750),
        .series = _vec_751,
        .series_len = ARRAY_SIZE(_vec_751),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_752,
        .properties_len = ARRAY_SIZE(_vec_752),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2201),
        .unsigned_value = UINT64_C(4294968498),
        .float_value = 1203.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_753,
        .bytes_value_len = ARRAY_SIZE(_vec_753),
        .row_count = 66741,
        .column_count = 66742,
        .x_values = _vec_754,
        .x_values_len = ARRAY_SIZE(_vec_754),
        .y_values = _vec_755,
        .y_values_len = ARRAY_SIZE(_vec_755),
        .series = _vec_756,
        .series_len = ARRAY_SIZE(_vec_756),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_757,
        .properties_len = ARRAY_SIZE(_vec_757),
    },
};
static const double  _vec_759[] = { 1211.5, 1212.5, 1213.5 };
static const double  _vec_760[] = { 1212.5, 1213.5, 1214.5 };
static const uint8_t _vec_761[] = { 193, 194, 195 };
static const double  _vec_762[] = { 1220.5, 1221.5, 1222.5 };
static const double  _vec_763[] = { 1221.5, 1222.5, 1223.5 };
static const NCodecDiagXilDiagnosticSeries _vec_764[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_765[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_766[] = { 202, 203, 204 };
static const double  _vec_767[] = { 1229.5, 1230.5, 1231.5 };
static const double  _vec_768[] = { 1230.5, 1231.5, 1232.5 };
static const NCodecDiagXilDiagnosticSeries _vec_769[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_770[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_771[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2214),
        .unsigned_value = UINT64_C(4294968511),
        .float_value = 1216.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_761,
        .bytes_value_len = ARRAY_SIZE(_vec_761),
        .row_count = 66754,
        .column_count = 66755,
        .x_values = _vec_762,
        .x_values_len = ARRAY_SIZE(_vec_762),
        .y_values = _vec_763,
        .y_values_len = ARRAY_SIZE(_vec_763),
        .series = _vec_764,
        .series_len = ARRAY_SIZE(_vec_764),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_765,
        .properties_len = ARRAY_SIZE(_vec_765),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2223),
        .unsigned_value = UINT64_C(4294968520),
        .float_value = 1225.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_766,
        .bytes_value_len = ARRAY_SIZE(_vec_766),
        .row_count = 66763,
        .column_count = 66764,
        .x_values = _vec_767,
        .x_values_len = ARRAY_SIZE(_vec_767),
        .y_values = _vec_768,
        .y_values_len = ARRAY_SIZE(_vec_768),
        .series = _vec_769,
        .series_len = ARRAY_SIZE(_vec_769),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_770,
        .properties_len = ARRAY_SIZE(_vec_770),
    },
};
static const uint8_t _vec_772[] = { 211, 212, 213 };
static const double  _vec_773[] = { 1238.5, 1239.5, 1240.5 };
static const double  _vec_774[] = { 1239.5, 1240.5, 1241.5 };
static const NCodecDiagXilDiagnosticSeries _vec_775[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_776[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_777[] = { 220, 221, 222 };
static const double  _vec_778[] = { 1247.5, 1248.5, 1249.5 };
static const double  _vec_779[] = { 1248.5, 1249.5, 1250.5 };
static const NCodecDiagXilDiagnosticSeries _vec_780[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_781[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_782[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2232),
        .unsigned_value = UINT64_C(4294968529),
        .float_value = 1234.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_772,
        .bytes_value_len = ARRAY_SIZE(_vec_772),
        .row_count = 66772,
        .column_count = 66773,
        .x_values = _vec_773,
        .x_values_len = ARRAY_SIZE(_vec_773),
        .y_values = _vec_774,
        .y_values_len = ARRAY_SIZE(_vec_774),
        .series = _vec_775,
        .series_len = ARRAY_SIZE(_vec_775),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_776,
        .properties_len = ARRAY_SIZE(_vec_776),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2241),
        .unsigned_value = UINT64_C(4294968538),
        .float_value = 1243.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_777,
        .bytes_value_len = ARRAY_SIZE(_vec_777),
        .row_count = 66781,
        .column_count = 66782,
        .x_values = _vec_778,
        .x_values_len = ARRAY_SIZE(_vec_778),
        .y_values = _vec_779,
        .y_values_len = ARRAY_SIZE(_vec_779),
        .series = _vec_780,
        .series_len = ARRAY_SIZE(_vec_780),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_781,
        .properties_len = ARRAY_SIZE(_vec_781),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_783[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_771,
        .values_len = ARRAY_SIZE(_vec_771),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_782,
        .values_len = ARRAY_SIZE(_vec_782),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_784[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_746 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-2187),
    .unsigned_value = UINT64_C(4294968484),
    .float_value = 1189.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_747,
    .bytes_value_len = ARRAY_SIZE(_vec_747),
    .elements = _vec_758,
    .elements_len = ARRAY_SIZE(_vec_758),
    .row_count = 66745,
    .column_count = 66746,
    .x_values = _vec_759,
    .x_values_len = ARRAY_SIZE(_vec_759),
    .y_values = _vec_760,
    .y_values_len = ARRAY_SIZE(_vec_760),
    .series = _vec_783,
    .series_len = ARRAY_SIZE(_vec_783),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_784,
    .properties_len = ARRAY_SIZE(_vec_784),
};
static const uint8_t _vec_785[] = { 225, 226, 227 };
static const uint8_t _vec_787[] = { 230, 231, 232 };
static const uint8_t _vec_788[] = { 235, 236, 237 };
static const double  _vec_789[] = { 1262.5, 1263.5, 1264.5 };
static const double  _vec_790[] = { 1263.5, 1264.5, 1265.5 };
static const NCodecDiagXilDiagnosticSeries _vec_791[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_792[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_793[] = { 244, 245, 246 };
static const double  _vec_794[] = { 1271.5, 1272.5, 1273.5 };
static const double  _vec_795[] = { 1272.5, 1273.5, 1274.5 };
static const NCodecDiagXilDiagnosticSeries _vec_796[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_797[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_798[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2256),
        .unsigned_value = UINT64_C(4294968553),
        .float_value = 1258.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_788,
        .bytes_value_len = ARRAY_SIZE(_vec_788),
        .row_count = 66796,
        .column_count = 66797,
        .x_values = _vec_789,
        .x_values_len = ARRAY_SIZE(_vec_789),
        .y_values = _vec_790,
        .y_values_len = ARRAY_SIZE(_vec_790),
        .series = _vec_791,
        .series_len = ARRAY_SIZE(_vec_791),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_792,
        .properties_len = ARRAY_SIZE(_vec_792),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2265),
        .unsigned_value = UINT64_C(4294968562),
        .float_value = 1267.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_793,
        .bytes_value_len = ARRAY_SIZE(_vec_793),
        .row_count = 66805,
        .column_count = 66806,
        .x_values = _vec_794,
        .x_values_len = ARRAY_SIZE(_vec_794),
        .y_values = _vec_795,
        .y_values_len = ARRAY_SIZE(_vec_795),
        .series = _vec_796,
        .series_len = ARRAY_SIZE(_vec_796),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_797,
        .properties_len = ARRAY_SIZE(_vec_797),
    },
};
static const double  _vec_799[] = { 1275.5, 1276.5, 1277.5 };
static const double  _vec_800[] = { 1276.5, 1277.5, 1278.5 };
static const uint8_t _vec_801[] = { 1, 2, 3 };
static const double  _vec_802[] = { 1284.5, 1285.5, 1286.5 };
static const double  _vec_803[] = { 1285.5, 1286.5, 1287.5 };
static const NCodecDiagXilDiagnosticSeries _vec_804[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_805[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_806[] = { 10, 11, 12 };
static const double  _vec_807[] = { 1293.5, 1294.5, 1295.5 };
static const double  _vec_808[] = { 1294.5, 1295.5, 1296.5 };
static const NCodecDiagXilDiagnosticSeries _vec_809[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_810[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_811[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2278),
        .unsigned_value = UINT64_C(4294968575),
        .float_value = 1280.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_801,
        .bytes_value_len = ARRAY_SIZE(_vec_801),
        .row_count = 66818,
        .column_count = 66819,
        .x_values = _vec_802,
        .x_values_len = ARRAY_SIZE(_vec_802),
        .y_values = _vec_803,
        .y_values_len = ARRAY_SIZE(_vec_803),
        .series = _vec_804,
        .series_len = ARRAY_SIZE(_vec_804),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_805,
        .properties_len = ARRAY_SIZE(_vec_805),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2287),
        .unsigned_value = UINT64_C(4294968584),
        .float_value = 1289.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_806,
        .bytes_value_len = ARRAY_SIZE(_vec_806),
        .row_count = 66827,
        .column_count = 66828,
        .x_values = _vec_807,
        .x_values_len = ARRAY_SIZE(_vec_807),
        .y_values = _vec_808,
        .y_values_len = ARRAY_SIZE(_vec_808),
        .series = _vec_809,
        .series_len = ARRAY_SIZE(_vec_809),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_810,
        .properties_len = ARRAY_SIZE(_vec_810),
    },
};
static const uint8_t _vec_812[] = { 19, 20, 21 };
static const double  _vec_813[] = { 1302.5, 1303.5, 1304.5 };
static const double  _vec_814[] = { 1303.5, 1304.5, 1305.5 };
static const NCodecDiagXilDiagnosticSeries _vec_815[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_816[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const uint8_t _vec_817[] = { 28, 29, 30 };
static const double  _vec_818[] = { 1311.5, 1312.5, 1313.5 };
static const double  _vec_819[] = { 1312.5, 1313.5, 1314.5 };
static const NCodecDiagXilDiagnosticSeries _vec_820[] = {
    {
        .name = "DiagnosticSeries.name",
    },
    {
        .name = "DiagnosticSeries.name",
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_821[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _vec_822[] = {
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2296),
        .unsigned_value = UINT64_C(4294968593),
        .float_value = 1298.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_812,
        .bytes_value_len = ARRAY_SIZE(_vec_812),
        .row_count = 66836,
        .column_count = 66837,
        .x_values = _vec_813,
        .x_values_len = ARRAY_SIZE(_vec_813),
        .y_values = _vec_814,
        .y_values_len = ARRAY_SIZE(_vec_814),
        .series = _vec_815,
        .series_len = ARRAY_SIZE(_vec_815),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_816,
        .properties_len = ARRAY_SIZE(_vec_816),
    },
    {
        .name = "DiagnosticValue.name",
        .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
        .boolean_value = true,
        .signed_value = INT64_C(-2305),
        .unsigned_value = UINT64_C(4294968602),
        .float_value = 1307.25,
        .string_value = "DiagnosticValue.string_value",
        .bytes_value = _vec_817,
        .bytes_value_len = ARRAY_SIZE(_vec_817),
        .row_count = 66845,
        .column_count = 66846,
        .x_values = _vec_818,
        .x_values_len = ARRAY_SIZE(_vec_818),
        .y_values = _vec_819,
        .y_values_len = ARRAY_SIZE(_vec_819),
        .series = _vec_820,
        .series_len = ARRAY_SIZE(_vec_820),
        .unit = "DiagnosticValue.unit",
        .description = "DiagnosticValue.description",
        .properties = _vec_821,
        .properties_len = ARRAY_SIZE(_vec_821),
    },
};
static const NCodecDiagXilDiagnosticSeries _vec_823[] = {
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_811,
        .values_len = ARRAY_SIZE(_vec_811),
    },
    {
        .name = "DiagnosticSeries.name",
        .values = _vec_822,
        .values_len = ARRAY_SIZE(_vec_822),
    },
};
static const NCodecDiagXilDiagnosticProperty _vec_824[] = {
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
    {
        .name = "DiagnosticProperty.name",
        .value = "DiagnosticProperty.value",
    },
};
static const NCodecDiagXilDiagnosticValue _obj_786 = {
    .name = "DiagnosticValue.name",
    .kind = NCodecDiagXilDiagnosticValueKindSignalGroup,
    .boolean_value = true,
    .signed_value = INT64_C(-2251),
    .unsigned_value = UINT64_C(4294968548),
    .float_value = 1253.25,
    .string_value = "DiagnosticValue.string_value",
    .bytes_value = _vec_787,
    .bytes_value_len = ARRAY_SIZE(_vec_787),
    .elements = _vec_798,
    .elements_len = ARRAY_SIZE(_vec_798),
    .row_count = 66809,
    .column_count = 66810,
    .x_values = _vec_799,
    .x_values_len = ARRAY_SIZE(_vec_799),
    .y_values = _vec_800,
    .y_values_len = ARRAY_SIZE(_vec_800),
    .series = _vec_823,
    .series_len = ARRAY_SIZE(_vec_823),
    .unit = "DiagnosticValue.unit",
    .description = "DiagnosticValue.description",
    .properties = _vec_824,
    .properties_len = ARRAY_SIZE(_vec_824),
};
static const NCodecDiagXilDataValue _vec_825[] = {
    {
        .path = "DataValue.path",
        .raw_value = _vec_745,
        .raw_value_len = ARRAY_SIZE(_vec_745),
        .physical_value = &_obj_746,
    },
    {
        .path = "DataValue.path",
        .raw_value = _vec_785,
        .raw_value_len = ARRAY_SIZE(_vec_785),
        .physical_value = &_obj_786,
    },
};
static const NCodecDiagXilDidIdentity _obj_826 = {
    .name = "DidIdentity.name",
    .identifier = 1569,
};
static const NCodecDiagXilDidValue _vec_827[] = {
    {
        .did = {
            .kind = NCodecDiagXilDidSelectorKindName,
            .identifier = 1308,
            .name = "DidSelector.name",
        },
        .raw_value = _vec_661,
        .raw_value_len = ARRAY_SIZE(_vec_661),
        .values = _vec_742,
        .values_len = ARRAY_SIZE(_vec_742),
        .identity = &_obj_743,
    },
    {
        .did = {
            .kind = NCodecDiagXilDidSelectorKindName,
            .identifier = 1439,
            .name = "DidSelector.name",
        },
        .raw_value = _vec_744,
        .raw_value_len = ARRAY_SIZE(_vec_744),
        .values = _vec_825,
        .values_len = ARRAY_SIZE(_vec_825),
        .identity = &_obj_826,
    },
};

const NCodecDiagXilMessage xil_variants[] = {
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeExecuteJobCall,
        .payload.execute_job_call = {
            .job_name = "ExecuteJobCall.job_name",
            .parameters = _vec_77,
            .parameters_len = ARRAY_SIZE(_vec_77),
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeExecuteJobReturn,
        .payload.execute_job_return = {
            .status = NCodecDiagXilStatusBusError,
            .values = _vec_158,
            .values_len = ARRAY_SIZE(_vec_158),
            .info = "ExecuteJobReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeSendHexServiceCall,
        .payload.send_hex_service_call = {
            .pdu = _vec_159,
            .pdu_len = ARRAY_SIZE(_vec_159),
            .response_required = true,
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeSendHexServiceReturn,
        .payload.send_hex_service_return = {
            .status = NCodecDiagXilStatusBusError,
            .response_pdu = _vec_160,
            .response_pdu_len = ARRAY_SIZE(_vec_160),
            .info = "SendHexServiceReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeReadDIDValueCall,
        .payload.read_did_value_call = {
            .did = {
                .kind = NCodecDiagXilDidSelectorKindName,
                .identifier = 514,
                .name = "DidSelector.name",
            },
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeReadDIDValueReturn,
        .payload.read_did_value_return = {
            .status = NCodecDiagXilStatusBusError,
            .negative_response_code = 60,
            .value = &_obj_161,
            .info = "ReadDIDValueReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeWriteDIDValueCall,
        .payload.write_did_value_call = {
            .value = {
                .did = {
                    .kind = NCodecDiagXilDidSelectorKindName,
                    .identifier = 647,
                    .name = "DidSelector.name",
                },
                .raw_value = _vec_245,
                .raw_value_len = ARRAY_SIZE(_vec_245),
                .values = _vec_326,
                .values_len = ARRAY_SIZE(_vec_326),
                .identity = &_obj_327,
            },
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeWriteDIDValueReturn,
        .payload.write_did_value_return = {
            .status = NCodecDiagXilStatusBusError,
            .negative_response_code = 123,
            .info = "WriteDIDValueReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeReadFaultMemoryCall,
        .payload.read_fault_memory_call = {
            .filter = NCodecDiagXilDtcFilterTypeSupported,
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeReadFaultMemoryReturn,
        .payload.read_fault_memory_return = {
            .status = NCodecDiagXilStatusBusError,
            .trouble_codes = _vec_490,
            .trouble_codes_len = ARRAY_SIZE(_vec_490),
            .info = "ReadFaultMemoryReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeClearFaultMemoryCall,
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeClearFaultMemoryReturn,
        .payload.clear_fault_memory_return = {
            .status = NCodecDiagXilStatusBusError,
            .info = "ClearFaultMemoryReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeReadFromAddressCall,
        .payload.read_from_address_call = {
            .address = UINT64_C(4294968079),
            .number_of_bytes = UINT64_C(4294968080),
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeReadFromAddressReturn,
        .payload.read_from_address_return = {
            .status = NCodecDiagXilStatusBusError,
            .data = _vec_491,
            .data_len = ARRAY_SIZE(_vec_491),
            .info = "ReadFromAddressReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeWriteToAddressCall,
        .payload.write_to_address_call = {
            .address = UINT64_C(4294968082),
            .data = _vec_492,
            .data_len = ARRAY_SIZE(_vec_492),
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeWriteToAddressReturn,
        .payload.write_to_address_return = {
            .status = NCodecDiagXilStatusBusError,
            .info = "WriteToAddressReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeGetIdentificationDataCall,
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeGetIdentificationDataReturn,
        .payload.get_identification_data_return = {
            .status = NCodecDiagXilStatusBusError,
            .identifiers = _vec_493,
            .identifiers_len = ARRAY_SIZE(_vec_493),
            .info = "GetIdentificationDataReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeGetVariantDataCall,
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeGetVariantDataReturn,
        .payload.get_variant_data_return = {
            .status = NCodecDiagXilStatusBusError,
            .values = _vec_660,
            .values_len = ARRAY_SIZE(_vec_660),
            .info = "GetVariantDataReturn.info",
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeSetVariantDataCall,
        .payload.set_variant_data_call = {
            .values = _vec_827,
            .values_len = ARRAY_SIZE(_vec_827),
        },
    },
    {
        .payload_type = NCodecDiagXilMethodPayloadTypeSetVariantDataReturn,
        .payload.set_variant_data_return = {
            .status = NCodecDiagXilStatusBusError,
            .info = "SetVariantDataReturn.info",
        },
    },
};
const size_t xil_variants_count = ARRAY_SIZE(xil_variants);

void assert_xil_payload_equal(
    const NCodecDiagXilMessage* a, const NCodecDiagXilMessage* b)
{
    assert_int_equal(a->payload_type, b->payload_type);
    switch (a->payload_type) {
    case NCodecDiagXilMethodPayloadTypeExecuteJobCall:
        assert_execute_job_call_equal(
            &a->payload.execute_job_call, &b->payload.execute_job_call);
        break;
    case NCodecDiagXilMethodPayloadTypeExecuteJobReturn:
        assert_execute_job_return_equal(
            &a->payload.execute_job_return, &b->payload.execute_job_return);
        break;
    case NCodecDiagXilMethodPayloadTypeSendHexServiceCall:
        assert_send_hex_service_call_equal(&a->payload.send_hex_service_call,
            &b->payload.send_hex_service_call);
        break;
    case NCodecDiagXilMethodPayloadTypeSendHexServiceReturn:
        assert_send_hex_service_return_equal(
            &a->payload.send_hex_service_return,
            &b->payload.send_hex_service_return);
        break;
    case NCodecDiagXilMethodPayloadTypeReadDIDValueCall:
        assert_read_did_value_call_equal(
            &a->payload.read_did_value_call, &b->payload.read_did_value_call);
        break;
    case NCodecDiagXilMethodPayloadTypeReadDIDValueReturn:
        assert_read_did_value_return_equal(&a->payload.read_did_value_return,
            &b->payload.read_did_value_return);
        break;
    case NCodecDiagXilMethodPayloadTypeWriteDIDValueCall:
        assert_write_did_value_call_equal(
            &a->payload.write_did_value_call, &b->payload.write_did_value_call);
        break;
    case NCodecDiagXilMethodPayloadTypeWriteDIDValueReturn:
        assert_write_did_value_return_equal(&a->payload.write_did_value_return,
            &b->payload.write_did_value_return);
        break;
    case NCodecDiagXilMethodPayloadTypeReadFaultMemoryCall:
        assert_read_fault_memory_call_equal(&a->payload.read_fault_memory_call,
            &b->payload.read_fault_memory_call);
        break;
    case NCodecDiagXilMethodPayloadTypeReadFaultMemoryReturn:
        assert_read_fault_memory_return_equal(
            &a->payload.read_fault_memory_return,
            &b->payload.read_fault_memory_return);
        break;
    case NCodecDiagXilMethodPayloadTypeClearFaultMemoryReturn:
        assert_clear_fault_memory_return_equal(
            &a->payload.clear_fault_memory_return,
            &b->payload.clear_fault_memory_return);
        break;
    case NCodecDiagXilMethodPayloadTypeReadFromAddressCall:
        assert_read_from_address_call_equal(&a->payload.read_from_address_call,
            &b->payload.read_from_address_call);
        break;
    case NCodecDiagXilMethodPayloadTypeReadFromAddressReturn:
        assert_read_from_address_return_equal(
            &a->payload.read_from_address_return,
            &b->payload.read_from_address_return);
        break;
    case NCodecDiagXilMethodPayloadTypeWriteToAddressCall:
        assert_write_to_address_call_equal(&a->payload.write_to_address_call,
            &b->payload.write_to_address_call);
        break;
    case NCodecDiagXilMethodPayloadTypeWriteToAddressReturn:
        assert_write_to_address_return_equal(
            &a->payload.write_to_address_return,
            &b->payload.write_to_address_return);
        break;
    case NCodecDiagXilMethodPayloadTypeGetIdentificationDataReturn:
        assert_get_identification_data_return_equal(
            &a->payload.get_identification_data_return,
            &b->payload.get_identification_data_return);
        break;
    case NCodecDiagXilMethodPayloadTypeGetVariantDataReturn:
        assert_get_variant_data_return_equal(
            &a->payload.get_variant_data_return,
            &b->payload.get_variant_data_return);
        break;
    case NCodecDiagXilMethodPayloadTypeSetVariantDataCall:
        assert_set_variant_data_call_equal(&a->payload.set_variant_data_call,
            &b->payload.set_variant_data_call);
        break;
    case NCodecDiagXilMethodPayloadTypeSetVariantDataReturn:
        assert_set_variant_data_return_equal(
            &a->payload.set_variant_data_return,
            &b->payload.set_variant_data_return);
        break;
    default:
        break;
    }
}
