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
    PdunetMock* mock = __create_mock("resources/model/pdunet_frnet.yaml");

    /* Reset the Lua state, should be internally created/destroyed. */
    lua_close(mock->L);
    mock->L = NULL;

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


static void __ncodec_trace_log__(
    void* nc, NCodecTraceLogLevel level, const char* msg)
{
    if (level >= dlog.level) dse_log2console(&dlog, level, NULL, 0, "%s", msg);
}

static void _visit_count_tx(PduNetwork* net, PduObject* pdu, void* data)
{
    assert_non_null(net);
    assert_non_null(data);
    if (pdu->pdu->dir == PduDirectionTx && pdu->needs_tx) {
        (*(int*)data)++;
    }
}


void test_pdunet_setup(void** state)
{
    // dlog.level = LOG_DEBUG;
    int         rc = 0;
    int         len;
    NCodecPdu   pdu;
    PdunetMock* mock = *state;

    // Stub the BusModel.
    NCODEC* nc = mock->ncodec;
    assert_non_null(nc);
    mock->bm_save = ((ABCodecInstance*)nc)->reader.bus_model;
    ((ABCodecInstance*)nc)->reader.bus_model = (ABCodecBusModel){};
    ((NCodecInstance*)nc)->trace.log = __ncodec_trace_log__;

    mock->step_size = 0.005;

    PduNetwork* net =
        pdunet_create(mock->ncodec, mock->doc, mock->step_size, mock->L, &dlog);
    mock->net = net;  // Teardown will destroy.
    assert_non_null(net);
    assert_double_equal(net->schedule.step_size, mock->step_size, 0.0);

    // Stub the SignalVector.
    const char* signal[] = {
        "Alive",
        "AliveRx",
        "FOO",
        "BAR",
    };
    double scalar[ARRAY_SIZE(signal)] = {};
    rc = pdunet_map_signals(net, "foo", ARRAY_SIZE(signal), signal, scalar);
    assert_int_equal(rc, 0);

    int tx_count = 0;
    pdunet_tx(net, NULL, _visit_count_tx, &tx_count, 0.0);
    assert_int_equal(tx_count, 0);

    tx_count = 0;
    pdunet_tx(net, NULL, _visit_count_tx, &tx_count, mock->step_size);
    assert_int_equal(tx_count, 1);

    tx_count = 0;
    pdunet_tx(net, NULL, _visit_count_tx, &tx_count, 2 * mock->step_size);
    assert_int_equal(tx_count, 1);
}


static void _visit_func(PduNetwork* net, PduObject* pdu, void* data)
{
    assert_non_null(net);
    vector_push((Vector*)data, (void*)pdu->pdu->name);
}

void test_pdunet_visit(void** state)
{
    // dlog.level = LOG_DEBUG;
    int         rc = 0;
    int         len;
    NCodecPdu   pdu;
    PdunetMock* mock = *state;

    // Stub the BusModel.
    NCODEC* nc = mock->ncodec;
    assert_non_null(nc);
    mock->bm_save = ((ABCodecInstance*)nc)->reader.bus_model;
    ((ABCodecInstance*)nc)->reader.bus_model = (ABCodecBusModel){};
    ((NCodecInstance*)nc)->trace.log = __ncodec_trace_log__;

    PduNetwork* net =
        pdunet_create(mock->ncodec, mock->doc, mock->step_size, mock->L, &dlog);
    mock->net = net;  // Teardown will destroy.
    assert_non_null(net);

    Vector visit_list = vector_make(sizeof(char*), 10, NULL);

    pdunet_visit(net, NULL, _visit_func, &visit_list);
    assert_int_equal(vector_len(&visit_list), 2);
    assert_string_equal(vector_at(&visit_list, 0, NULL), "ONE_RX");
    assert_string_equal(vector_at(&visit_list, 1, NULL), "ONE_TX");

    vector_reset(&visit_list);
}


void test_pdunet_tx_fr(void** state)
{
    // dlog.level = LOG_DEBUG;
    int         rc = 0;
    int         len;
    NCodecPdu   pdu;
    PdunetMock* mock = *state;

    // Stub the BusModel.
    NCODEC* nc = mock->ncodec;
    assert_non_null(nc);
    mock->bm_save = ((ABCodecInstance*)nc)->reader.bus_model;
    ((ABCodecInstance*)nc)->reader.bus_model = (ABCodecBusModel){};
    ((NCodecInstance*)nc)->trace.log = __ncodec_trace_log__;

    PduNetwork* net =
        pdunet_create(mock->ncodec, mock->doc, mock->step_size, mock->L, &dlog);
    mock->net = net;  // Teardown will destroy.
    assert_non_null(net);

    // Stub the SignalVector.
    const char* signal[] = {
        "Alive",
        "AliveRx",
        "FOO",
        "BAR",
    };
    double scalar[ARRAY_SIZE(signal)] = {};
    rc = pdunet_map_signals(net, "foo", ARRAY_SIZE(signal), signal, scalar);
    assert_int_equal(rc, 0);

    // Set some signals
    PduObject* o_pdu = vector_at(&net->matrix.pdu, 1, NULL);
    assert_non_null(o_pdu);
    assert_int_equal(o_pdu->needs_tx, false);
    assert_int_equal(o_pdu->ncodec.pdu.id, 101);
    assert_int_equal(o_pdu->ncodec.pdu.payload_len, 64);
    assert_int_equal(o_pdu->pdu->id, 101);
    assert_int_equal(o_pdu->pdu->dir, PduDirectionTx);
    scalar[0] = 24;
    scalar[2] = 42;

    // Disable the schedule for this pdu.
    o_pdu->schedule.interval = 0;
    o_pdu->schedule.phase = 0;

    // Send vtable.config(), but no PDUs have needs_tx set, rx armed.
    //__log_level__ = 0;
    pdunet_tx(net, NULL, pdunet_visit_clear_tx_flag, NULL, 0);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){ 0 };
    len = ncodec_read(nc, &pdu);
    assert_int_equal(pdu.ecu_id, 5);
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeFlexray);
    assert_int_equal(pdu.transport.flexray.metadata_type,
        NCodecPduFlexrayMetadataTypeConfig);
    // Rx armed.
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, 0);
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeFlexray);
    assert_int_equal(
        pdu.transport.flexray.metadata_type, NCodecPduFlexrayMetadataTypeLpdu);
    assert_int_equal(pdu.transport.flexray.metadata.lpdu.status,
        NCodecPduFlexrayLpduStatusNotReceived);
    assert_int_equal(pdu.transport.flexray.metadata.lpdu.frame_config_index, 1);
    // End
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Send vtable.lpdu(), set needs_tx, PDUs sent.
    pdunet_tx(net, NULL, NULL, NULL, 0);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){ 0 };
    len = ncodec_read(nc, &pdu);
    uint8_t expect[] = { 26 /* 18 + 1 + 1 */, 42 };
    assert_memory_equal(pdu.payload, expect, 2);
    assert_int_equal(pdu.ecu_id, 5);
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeFlexray);
    assert_int_equal(
        pdu.transport.flexray.metadata_type, NCodecPduFlexrayMetadataTypeLpdu);
    assert_int_equal(pdu.transport.flexray.metadata.lpdu.status,
        NCodecPduFlexrayLpduStatusNotTransmitted);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);
}

static void _visit_count_update_flag(
    PduNetwork* net, PduObject* pdu, void* data)
{
    assert_non_null(net);
    assert_non_null(data);
    int* counter = data;
    if (pdu->update_signals == true) {
        (*counter)++;
    }
}

static void _visit_count_csum_set(PduNetwork* net, PduObject* pdu, void* data)
{
    assert_non_null(net);
    assert_non_null(data);
    int* counter = data;
    if (pdu->checksum != 0) {
        (*counter)++;
    }
}

void test_pdunet_rx_fr(void** state)
{
    // dlog.level = LOG_DEBUG;
    int         rc = 0;
    int         len;
    NCodecPdu   pdu;
    PdunetMock* mock = *state;

    // Stub the BusModel.
    NCODEC* nc = mock->ncodec;
    assert_non_null(nc);
    mock->bm_save = ((ABCodecInstance*)nc)->reader.bus_model;
    ((ABCodecInstance*)nc)->reader.bus_model = (ABCodecBusModel){};
    ((NCodecInstance*)nc)->trace.log = __ncodec_trace_log__;

    PduNetwork* net =
        pdunet_create(mock->ncodec, mock->doc, mock->step_size, mock->L, &dlog);
    mock->net = net;  // Teardown will destroy.
    assert_non_null(net);

    // Stub the SignalVector.
    const char* signal[] = {
        "Alive",
        "AliveRx",
        "FOO",
        "BAR",
    };
    double scalar[ARRAY_SIZE(signal)] = {};
    rc = pdunet_map_signals(net, "foo", ARRAY_SIZE(signal), signal, scalar);
    assert_int_equal(rc, 0);

    // Recv, non-FlexRay is discarded.
    pdunet_visit(net, NULL, pdunet_visit_clear_update_flag, NULL);
    ncodec_truncate(nc);
    ncodec_write(nc, &(struct NCodecPdu){
                         .id = 101,
                         .payload = (const uint8_t*)"\x18\x2a",
                         .payload_len = 2,
                         .transport_type = NCodecPduTransportTypeCan,
                     });
    ncodec_flush(nc);
    int rx_count = 0;
    pdunet_rx(net, NULL, _visit_count_update_flag, &rx_count);
    assert_int_equal(rx_count, 0);


    // Recv, status is discarded, payload is decoded.
    pdunet_visit(net, NULL, pdunet_visit_clear_update_flag, NULL);
    ncodec_truncate(nc);
    ncodec_write(nc, &(struct NCodecPdu){
                         .transport_type = NCodecPduTransportTypeFlexray,
                         .transport.flexray.metadata_type =
                             NCodecPduFlexrayMetadataTypeStatus,
                     });
    ncodec_write(nc,
        &(struct NCodecPdu){
            .id = 101,
            .payload = (const uint8_t*)"\x18\x2a",
            .payload_len = 2,
            .transport_type = NCodecPduTransportTypeFlexray,
            .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
        });
    ncodec_flush(nc);
    rx_count = 0;
    // Flag is set _during_ rx.
    pdunet_rx(net, NULL, _visit_count_update_flag, &rx_count);
    assert_int_equal(rx_count, 1);
    PduObject* o_pdu = vector_at(&net->matrix.pdu, 0, NULL);
    assert_non_null(o_pdu);
    // Flag was cleared _after_ rx.
    assert_int_equal(o_pdu->update_signals, false);
    assert_int_equal(o_pdu->ncodec.pdu.id, 101);
    assert_int_equal(o_pdu->ncodec.pdu.payload_len, 64);
    uint8_t expect[] = { 24, 42 };
    assert_memory_equal(o_pdu->ncodec.pdu.payload, expect, 2);
    assert_int_equal(o_pdu->pdu->id, 101);
    assert_int_equal(o_pdu->pdu->dir, PduDirectionRx);
    assert_double_equal(scalar[1], 24, 0);
    assert_double_equal(scalar[3], 42, 0);
    rx_count = 0;
    // Flag remains cleared _after_ rx.
    pdunet_rx(net, NULL, _visit_count_update_flag, &rx_count);
    assert_int_equal(rx_count, 0);
}


int run_pdunet_ncodec_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_pdunet_setup, s, t),
        cmocka_unit_test_setup_teardown(test_pdunet_visit, s, t),
        cmocka_unit_test_setup_teardown(test_pdunet_tx_fr, s, t),
        cmocka_unit_test_setup_teardown(test_pdunet_rx_fr, s, t),
    };

    return cmocka_run_group_tests_name("PDU Net::NCodec", tests, NULL, NULL);
}
