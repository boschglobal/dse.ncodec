// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/diag/uds.h>
#include <tests/cmocka/codec/diag/test_diag.h>


/* test_uds_variants.c */
extern const NCodecDiagUdsMessage uds_variants[];
extern const size_t               uds_variants_count;
extern void                       assert_uds_primitive_equal(
                          const NCodecDiagUdsMessage* a, const NCodecDiagUdsMessage* b);


static int test_setup(void** state)
{
    return diag_test_setup(state, MIMETYPE("uds"));
}

static int test_teardown(void** state)
{
    return diag_test_teardown(state);
}


void test_uds_variants(void** state)
{
    Mock* mock = *state;

    /* All union variants, in a single stream. */
    assert_int_equal(uds_variants_count,
        NCodecDiagUdsServicePrimitiveTypeRequestTransferExitResponse);
    for (size_t i = 0; i < uds_variants_count; i++) {
        NCodecDiagUdsMessage msg = uds_variants[i];
        msg.is_request = i % 2;
        msg.service_id = (NCodecDiagUdsServiceId)(0x10 + i);
        assert_true(ncodec_write(mock->nc, &msg) > 0);
    }
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    for (size_t i = 0; i < uds_variants_count; i++) {
        NCodecDiagUdsMessage msg = {};
        assert_true(ncodec_read(mock->nc, &msg) > 0);
        assert_int_equal(msg.is_request, i % 2);
        assert_int_equal(msg.service_id, 0x10 + i);
        assert_uds_primitive_equal(&uds_variants[i], &msg);
    }
    NCodecDiagUdsMessage msg = {};
    assert_int_equal(ncodec_read(mock->nc, &msg), -ENOMSG);
}


void test_uds_none(void** state)
{
    Mock* mock = *state;

    NCodecDiagUdsMessage msg = {
        .is_request = true,
        .service_id = NCodecDiagUdsServiceIdTesterPresentRequest,
    };
    int32_t len = ncodec_write(mock->nc, &msg);
    assert_true(len > 0);
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    msg = (NCodecDiagUdsMessage){};
    assert_int_equal(ncodec_read(mock->nc, &msg), len);
    assert_true(msg.is_request);
    assert_int_equal(
        msg.service_id, NCodecDiagUdsServiceIdTesterPresentRequest);
    assert_int_equal(msg.primitive_type, NCodecDiagUdsServicePrimitiveTypeNone);
}


void test_uds_vectors(void** state)
{
    Mock* mock = *state;

    /* NULL is absent, non-NULL with zero length is present and empty. */
    static const uint8_t  empty_u8[1];
    static const uint16_t empty_u16[1];
    NCodecDiagUdsMessage  msg[] = {
        { .primitive_type =
                 NCodecDiagUdsServicePrimitiveTypeTransferDataRequest,
             .primitive.transfer_data_request = { .block_sequence_counter =
                                                      1 } },
        { .primitive_type =
                 NCodecDiagUdsServicePrimitiveTypeTransferDataRequest,
             .primitive.transfer_data_request = { .block_sequence_counter = 2,
                 .parameter_record = empty_u8 } },
        { .primitive_type =
                 NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierRequest },
        { .primitive_type =
                 NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierRequest,
             .primitive.read_data_by_identifier_request = { .data_id =
                                                                empty_u16 } },
        { .primitive_type =
                 NCodecDiagUdsServicePrimitiveTypeReadDataByIdentifierResponse },
    };
    for (size_t i = 0; i < ARRAY_SIZE(msg); i++) {
        assert_true(ncodec_write(mock->nc, &msg[i]) > 0);
    }
    ncodec_seek(mock->nc, 0, NCODEC_SEEK_SET);
    for (size_t i = 0; i < ARRAY_SIZE(msg); i++) {
        NCodecDiagUdsMessage out = {};
        assert_true(ncodec_read(mock->nc, &out) > 0);
        assert_uds_primitive_equal(&msg[i], &out);
    }
}


int run_uds_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_uds_variants, s, t),
        cmocka_unit_test_setup_teardown(test_uds_none, s, t),
        cmocka_unit_test_setup_teardown(test_uds_vectors, s, t),
    };

    return cmocka_run_group_tests_name("UDS", tests, NULL, NULL);
}
