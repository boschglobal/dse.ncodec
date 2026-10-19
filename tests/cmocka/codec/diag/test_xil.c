// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/diag/xil.h>
#include <tests/cmocka/codec/diag/test_diag.h>


#define MIMETYPE_NODE(id) MIMETYPE("xil") ";node_id=" id ";name=ecu_" id


/* test_xil_variants.c */
extern const NCodecDiagXilMessage xil_variants[];
extern const size_t               xil_variants_count;
extern void                       assert_xil_payload_equal(
                          const NCodecDiagXilMessage* a, const NCodecDiagXilMessage* b);


static int test_setup(void** state)
{
    return diag_test_setup(state, MIMETYPE_NODE("1"));
}

static int test_teardown(void** state)
{
    return diag_test_teardown(state);
}


void test_xil_variants(void** state)
{
    Mock* mock = *state;

    /* All union variants, including recursive value/series data. */
    assert_int_equal(
        xil_variants_count, NCodecDiagXilMethodPayloadTypeSetVariantDataReturn);
    for (size_t i = 0; i < xil_variants_count; i++) {
        NCodecDiagXilMessage msg = xil_variants[i];
        msg.call_id = i;
        msg.ecu_name = "ecu";
        msg.logical_link_name = "link";
        msg.ecu_information = "info";
        msg.node_uid = 100 + i;
        msg.simulation_time = 0.5 * i;
        msg.utc_time = 1000 + i;
        assert_true(ncodec_write(mock->nc, &msg) > 0);
    }
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    for (size_t i = 0; i < xil_variants_count; i++) {
        NCodecDiagXilMessage msg = {};
        assert_true(ncodec_read(mock->nc, &msg) > 0);
        assert_int_equal(msg.call_id, i);
        assert_string_equal(msg.ecu_name, "ecu");
        assert_string_equal(msg.logical_link_name, "link");
        assert_string_equal(msg.ecu_information, "info");
        assert_int_equal(msg.node_uid, 100 + i);
        assert_true(msg.simulation_time == 0.5 * i);
        assert_int_equal(msg.utc_time, 1000 + i);
        assert_xil_payload_equal(&xil_variants[i], &msg);
    }
    NCodecDiagXilMessage msg = {};
    assert_int_equal(ncodec_read(mock->nc, &msg), -ENOMSG);
}


void test_xil_metadata(void** state)
{
    Mock* mock = *state;

    /* ecu_name/node_uid from codec parameters, unless set. */
    NCodecDiagXilMessage msg = { .call_id = 1 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);
    msg = (NCodecDiagXilMessage){
        .call_id = 2, .ecu_name = "target", .node_uid = 300
    };
    assert_true(ncodec_write(mock->nc, &msg) > 0);

    NCODEC* nc = ncodec_open(
        MIMETYPE_NODE("9"), ncodec_buffer_stream_create(BUFFER_LEN));
    stream_append(nc, mock->nc);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    NCodecDiagXilMessage out = {};
    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.call_id, 1);
    assert_string_equal(out.ecu_name, "ecu_1");
    assert_int_equal(out.node_uid, 1);
    assert_null(out.logical_link_name);
    assert_true(ncodec_read(nc, &out) > 0);
    assert_int_equal(out.call_id, 2);
    assert_string_equal(out.ecu_name, "target");
    assert_int_equal(out.node_uid, 300);
    ncodec_close(nc);
}


void test_xil_filter(void** state)
{
    Mock* mock = *state;

    NCodecDiagXilMessage msg = { .call_id = 1 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);
    msg = (NCodecDiagXilMessage){ .call_id = 2, .node_uid = 7 };
    assert_true(ncodec_write(mock->nc, &msg) > 0);

    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    NCodecDiagXilMessage out = {};
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(out.call_id, 2);
    assert_int_equal(ncodec_read(mock->nc, &out), -ENOMSG);

    ncodec_config(mock->nc,
        (struct NCodecConfigItem){ .name = "loopback", .value = "1" });
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(out.call_id, 1);
    assert_true(ncodec_read(mock->nc, &out) > 0);
    assert_int_equal(out.call_id, 2);
}


void test_xil_required(void** state)
{
    Mock* mock = *state;

    /* ecu_name is required, no message or codec value. */
    NCODEC* nc =
        ncodec_open(MIMETYPE("xil"), ncodec_buffer_stream_create(BUFFER_LEN));
    NCodecDiagXilMessage msg = { .call_id = 1 };
    assert_int_equal(ncodec_write(nc, &msg), -EINVAL);
    assert_int_equal(ncodec_tell(nc), 0);
    ncodec_close(nc);

    /* Nested required fields. */
    static const NCodecDiagXilNamedDiagnosticValue unnamed[1];
    static const NCodecDiagXilDiagnosticProperty   property[1];
    static const NCodecDiagXilDiagnosticValue value = { .properties = property,
        .properties_len = 1 };
    static const NCodecDiagXilNamedDiagnosticValue nested[] = {
        { .name = "p", .value = { .elements = &value, .elements_len = 1 } },
    };
    static const NCodecDiagXilDidValue did_values[1];
    NCodecDiagXilMessage               invalid[] = {
        { .payload_type = NCodecDiagXilMethodPayloadTypeExecuteJobCall },
        { .payload_type = NCodecDiagXilMethodPayloadTypeExecuteJobCall,
                          .payload.execute_job_call = { .job_name = "job",
                              .parameters = unnamed,
                              .parameters_len = 1 } },
        { .payload_type = NCodecDiagXilMethodPayloadTypeExecuteJobCall,
                          .payload.execute_job_call = { .job_name = "job",
                              .parameters = nested,
                              .parameters_len = 1 } },
        { .payload_type = NCodecDiagXilMethodPayloadTypeSendHexServiceCall },
        { .payload_type = NCodecDiagXilMethodPayloadTypeWriteToAddressCall },
        { .payload_type = NCodecDiagXilMethodPayloadTypeSetVariantDataCall },
    };
    for (size_t i = 0; i < ARRAY_SIZE(invalid); i++) {
        assert_int_equal(ncodec_write(mock->nc, &invalid[i]), -EINVAL);
        assert_int_equal(ncodec_tell(mock->nc), 0);
    }

    /* Present, empty required vectors are valid. */
    static const uint8_t empty[1];
    NCodecDiagXilMessage valid[] = {
        { .payload_type = NCodecDiagXilMethodPayloadTypeSendHexServiceCall,
            .payload.send_hex_service_call = { .pdu = empty } },
        { .payload_type = NCodecDiagXilMethodPayloadTypeSetVariantDataCall,
            .payload.set_variant_data_call = { .values = did_values } },
    };
    for (size_t i = 0; i < ARRAY_SIZE(valid); i++) {
        assert_true(ncodec_write(mock->nc, &valid[i]) > 0);
    }
    nc = ncodec_open(
        MIMETYPE_NODE("9"), ncodec_buffer_stream_create(BUFFER_LEN));
    stream_append(nc, mock->nc);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    for (size_t i = 0; i < ARRAY_SIZE(valid); i++) {
        NCodecDiagXilMessage out = {};
        assert_true(ncodec_read(nc, &out) > 0);
        assert_xil_payload_equal(&valid[i], &out);
    }
    ncodec_close(nc);
}


int run_xil_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_xil_variants, s, t),
        cmocka_unit_test_setup_teardown(test_xil_metadata, s, t),
        cmocka_unit_test_setup_teardown(test_xil_filter, s, t),
        cmocka_unit_test_setup_teardown(test_xil_required, s, t),
    };

    return cmocka_run_group_tests_name("XIL", tests, NULL, NULL);
}
