// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/pdu.h>

#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define BUFFER_LEN    1024


extern NCodecConfigItem codec_stat(NCODEC* nc, int* index);
extern NCODEC*          ncodec_create(const char* mime_type);
extern int32_t stream_read(NCODEC* nc, uint8_t** data, size_t* len, int pos_op);


typedef struct Mock {
    NCODEC* nc;
} Mock;


#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "swc_id=4;ecu_id=5;loopback=1"


static int test_setup(void** state)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);

    NSTREAM* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(MIMETYPE, stream);
    assert_non_null(mock->nc);

    *state = mock;
    return 0;
}


static int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock && mock->nc) ncodec_close((void*)mock->nc);
    if (mock) free(mock);

    return 0;
}

void test_flexray__utime_api(void** state)
{
    Mock*            mock = *state;
    NCODEC*          nc = mock->nc;
    ABCodecInstance* abci = (ABCodecInstance*)nc;

    struct TestCase {
        const char* name;
        double      simulation_time;
        double      step_size;
        int         expected_errno;
    };

    // Initial conditions.
    assert_double_equal(abci->simulation_time.value, 0.0, 0.0);
    assert_double_equal(abci->simulation_time.step_size, 0.0005, 0.0);
    assert_double_equal(abci->simulation_time.step_size_correction, 0.0, 0.0);

    // clang-format off
    const struct TestCase cases[] = {
        { "no data",           0.0,  0.0, ENODATA },
        { "negative sim time", -0.1, 0.0, EINVAL  },
        { "negative step",     0.0, -0.1, EINVAL  },
        { "valid sim time",    0.1,  0.0, 0       },
        { "valid step",        0.0,  0.1, 0       },
    };
    // clang-format on

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        // Entry condition.
        double sim_time = abci->simulation_time.value;
        double step_size = abci->simulation_time.step_size;
        double correction = abci->simulation_time.step_size_correction = 1.0;

        int rc =
            ncodec_utime(nc, (struct NCodecUtimeOperation){
                                 .simulation_time = cases[i].simulation_time,
                                 .step_size = cases[i].step_size });

        if (-rc != cases[i].expected_errno) {
            fail_msg("case[%zu] (%s): simulation_time=%f, step_size=%f, "
                     "expected=%d, got=%d",
                i, cases[i].name, cases[i].simulation_time, cases[i].step_size,
                cases[i].expected_errno, -rc);
        }

        if (cases[i].expected_errno == 0) {
            if (cases[i].simulation_time > 0) {
                assert_double_equal(
                    abci->simulation_time.value, cases[i].simulation_time, 0.0);
                assert_double_equal(
                    abci->simulation_time.step_size_correction, 0.0, 0.0);
            }
            if (cases[i].step_size > 0) {
                assert_double_equal(
                    abci->simulation_time.step_size, cases[i].step_size, 0.0);
            }
        } else {
            // Values unchanged.
            assert_double_equal(abci->simulation_time.value, sim_time, 0.0);
            assert_double_equal(
                abci->simulation_time.step_size, step_size, 0.0);
            assert_double_equal(
                abci->simulation_time.step_size_correction, correction, 0.0);
        }
    }
}


void test_flexray__utime_force(void** state)
{
    Mock*            mock = *state;
    NCODEC*          nc = mock->nc;
    ABCodecInstance* abci = (ABCodecInstance*)nc;

    struct TestCase {
        const char* name;
        double      simulation_time;
        double      step_size;
        int         expected_errno;
    };

    // Initial conditions.
    assert_double_equal(abci->simulation_time.value, 0.0, 0.0);
    assert_double_equal(abci->simulation_time.step_size, 0.0005, 0.0);
    assert_double_equal(abci->simulation_time.step_size_correction, 0.0, 0.0);

    // Push time.
    abci->simulation_time.value = 1.0;
    abci->simulation_time.step_size_correction = 1.0;

    // Force a time set (overrides checks).
    int rc = ncodec_utime(nc,
        (struct NCodecUtimeOperation){ .simulation_time = 0.5, .force = true });
    assert_int_equal(rc, 0);
    assert_double_equal(abci->simulation_time.value, 0.5, 0.0);
    assert_double_equal(abci->simulation_time.step_size_correction, 0.0, 0.0);

    // Force an invalid time set (should fail).
    rc = ncodec_utime(nc, (struct NCodecUtimeOperation){
                              .simulation_time = -1.5, .force = true });
    assert_int_equal(-rc, EINVAL);
    assert_double_equal(abci->simulation_time.value, 0.5, 0.0);
}


void test_flexray__utime_broadcast(void** state)
{
    skip();
}


void test_flexray__utime_broadcast_force(void** state)
{
    skip();
}


int run_pdu_flexray_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_flexray__utime_api, s, t),
        cmocka_unit_test_setup_teardown(test_flexray__utime_force, s, t),
    };

    return cmocka_run_group_tests_name("PDU FLEXRAY", tests, NULL, NULL);
}
