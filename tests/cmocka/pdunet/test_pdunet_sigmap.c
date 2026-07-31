// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <math.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "mock.h"         // NOLINT(build/include_subdir,build/include_order)
#include <dse/testing.h>  // NOLINT(build/include_order)
#include <dse/log.h>      // NOLINT(build/include_order)
#include <dse/clib/util/yaml.h>          // NOLINT(build/include_order)
#include <dse/ncodec/codec.h>            // NOLINT(build/include_order)
#include <dse/ncodec/stream/stream.h>    // NOLINT(build/include_order)
#include <dse/pdunet/network/network.h>  // NOLINT(build/include_order)
#include <dse/pdunet/pdunet.h>           // NOLINT(build/include_order)


#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))


extern DseLog  dlog;
extern NCODEC* ncodec_create(const char* mime_type);
extern NCODEC* ncodec_open(const char* mime_type, NSTREAM* stream);


static int test_setup(void** state)
{
    PdunetMock* mock = __create_mock("resources/model/pdunet.yaml");
    *state = mock;
    return 0;
}

static int test_teardown(void** state)
{
    PdunetMock* mock = *state;
    dlog.level = LOG_QUIET;
    if (mock) {
        if (mock->net) pdunet_destroy(mock->net);
        if (mock->ncodec) ncodec_close(mock->ncodec);
        if (mock->dl) dse_yaml_destroy_doc_list(mock->dl);
    }
    free(mock);
    return 0;
}


typedef struct {
    const char* sig_name;
    size_t      sig_idx;  // Signal vector.
    size_t      src_idx;  // Network signal (source).
} map_check;

void test_pdunet_sigmap(void** state)
{
    // dlog.level = LOG_DEBUG;
    PdunetMock* mock = *state;

    PduNetworkDesc* net =
        pdunet_create(mock->ncodec, mock->doc, mock->step_size, mock->L, &dlog);
    mock->net = net;  // Teardown will destroy.
    assert_non_null(net);
    assert_int_equal(vector_len(&net->pdus), 4);
    assert_int_equal(net->matrix.signal.count, 8);

    const char* signal[] = {
        "SIG_A",
        "SIG_B",
        "SIG_C",
        "SIG_D",
        "SIG_E",
        "SIG_F",
        "SIG_G",
        "SIG_H",
    };
    double scalar[ARRAY_SIZE(signal)] = {};
    int    rc = 0;

    rc = pdunet_map_signals(net, "foo", ARRAY_SIZE(signal), signal, scalar);
    assert_int_equal(rc, 0);

    assert_string_equal(net->msm.in->name, "foo");
    assert_int_equal(net->msm.in->is_binary, 0);
    assert_int_equal(net->msm.in->count, 4);
    assert_int_equal(net->msm.in->offset, 0);
    map_check in_checks[] = {
        { "SIG_C", 2, 0 },
        { "SIG_D", 3, 1 },
        { "SIG_G", 6, 2 },
        { "SIG_H", 7, 3 },
    };
    for (size_t i = 0; i < ARRAY_SIZE(in_checks); i++) {
        map_check c = in_checks[i];
        log_info(&dlog, "Check IN[%u] name=%s, sig=%u, src=%u", i, c.sig_name,
            c.sig_idx, c.src_idx);
        assert_string_equal(signal[net->msm.in->signal.index[i]], c.sig_name);
        assert_int_equal(net->msm.in->signal.index[i], c.sig_idx);
        assert_int_equal(
            net->msm.in->source.index[i] + net->msm.in->offset, c.src_idx);
    }

    assert_string_equal(net->msm.out->name, "foo");
    assert_int_equal(net->msm.out->is_binary, 0);
    assert_int_equal(net->msm.out->count, 4);
    assert_int_equal(net->msm.out->offset, 4);
    map_check out_checks[] = {
        { "SIG_A", 0, 4 },
        { "SIG_B", 1, 5 },
        { "SIG_E", 4, 6 },
        { "SIG_F", 5, 7 },
    };
    for (size_t i = 0; i < ARRAY_SIZE(out_checks); i++) {
        map_check c = out_checks[i];
        log_info(&dlog, "Check OUT[%u] name=%s, sig=%u, src=%u", i, c.sig_name,
            c.sig_idx, c.src_idx);
        assert_string_equal(signal[net->msm.out->signal.index[i]], c.sig_name);
        assert_int_equal(net->msm.out->signal.index[i], c.sig_idx);
        assert_int_equal(
            net->msm.out->source.index[i] + net->msm.out->offset, c.src_idx);
    }
}


int run_pdunet_sigmap_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_pdunet_sigmap, s, t),
    };

    return cmocka_run_group_tests_name("PDU Net::SigMap", tests, NULL, NULL);
}
