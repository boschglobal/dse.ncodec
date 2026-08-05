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
    *state = mock;
    return 0;
}

static int test_setup_c(void** state)
{
    PdunetMock* mock = __create_mock("resources/model/pdunet_frcontainer.yaml");
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

static void _visit_count_csum_set(PDUNET* n, PDUOBJECT* p, void* data)
{
    PduNetworkDesc* net = (PduNetworkDesc*)n;
    PduObject*      pdu = (PduObject*)p;
    assert_non_null(net);
    assert_non_null(data);
    int* counter = data;
    if (pdu->checksum != 0) {
        (*counter)++;
    }
}


void test_pdunet_schedule_pdu_fr(void** state)
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

    PduNetworkDesc* net =
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

    // Set initial condition (calculate checksum, set needs_tx to false).
    pdunet_visit(net, NULL, pdunet_visit_set_checksum, NULL);
    pdunet_visit(net, NULL, pdunet_visit_clear_tx_flag, NULL);
    {
        int count = 0;
        pdunet_visit(net, NULL, _visit_count_csum_set, &count);
        assert_int_equal(count, 1);
    }

    // Send vtable.config(), but no PDUs have needs_tx set.
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

    // Count set checksums.
    {
        int count = 0;
        pdunet_visit(net, NULL, _visit_count_csum_set, &count);
        assert_int_equal(count, 1);
    }

    // Push the simulation to interval + phase - 1 step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0035);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.004);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){ 0 };
    len = ncodec_read(nc, &pdu);
    assert_true(len > 0);
    uint8_t expect[] = { 25 /* 18 + 1 */, 42 };
    assert_memory_equal(pdu.payload, expect, 2);
    assert_int_equal(pdu.ecu_id, 5);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0045);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation to interval + phase - 1 step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0085);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.009);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){ 0 };
    len = ncodec_read(nc, &pdu);
    assert_true(len > 0);
    uint8_t expect2[] = { 26 /* 18 + 1 + 1 */, 42 };
    assert_memory_equal(pdu.payload, expect2, 2);
    assert_int_equal(pdu.ecu_id, 5);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0095);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);
}


void test_pdunet_schedule_net_fr(void** state)
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

    PduNetworkDesc* net =
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

    // Set initial condition (calculate checksum, set needs_tx to false).
    pdunet_visit(net, NULL, pdunet_visit_set_checksum, NULL);
    pdunet_visit(net, NULL, pdunet_visit_clear_tx_flag, NULL);
    {
        int count = 0;
        pdunet_visit(net, NULL, _visit_count_csum_set, &count);
        assert_int_equal(count, 1);
    }

    // Send vtable.config(), but no PDUs have needs_tx set.
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

    // Count set checksums.
    {
        int count = 0;
        pdunet_visit(net, NULL, _visit_count_csum_set, &count);
        assert_int_equal(count, 1);
    }

    // Set the epoch_offset
    net->schedule.epoch_offset = 0.002;

    // Push the simulation to offset + interval + phase - 1 step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0055);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.006);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){ 0 };
    len = ncodec_read(nc, &pdu);
    assert_true(len > 0);
    uint8_t expect[] = { 25 /* 18 + 1 */, 42 };
    assert_memory_equal(pdu.payload, expect, 2);
    assert_int_equal(pdu.ecu_id, 5);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0065);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);
}


void test_pdunet_schedule_status_fr(void** state)
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

    PduNetworkDesc* net =
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

    // Configure the PDU Net.
    net->schedule.simulation_time = 4; /* 2mS */
    net->network.vtable.flexray.cycle = 1;

    // Recv, status - cycle set.
    pdunet_visit(net, NULL, pdunet_visit_clear_update_flag, NULL);
    ncodec_truncate(nc);
    ncodec_write(nc, &(struct NCodecPdu){
                         .transport_type = NCodecPduTransportTypeFlexray,
                         .transport.flexray.metadata_type =
                             NCodecPduFlexrayMetadataTypeStatus,
                         .transport.flexray.metadata.status = { .cycle = 1,
                             .macrotick = 600 },
                     });
    ncodec_flush(nc);
    pdunet_rx(net, NULL, NULL, NULL);
    assert_double_equal(net->schedule.epoch_offset, 0.000, 0.0);

    // Recv, status - cycle change, should set epoch_offset to align.
    pdunet_visit(net, NULL, pdunet_visit_clear_update_flag, NULL);
    ncodec_truncate(nc);
    ncodec_write(nc, &(struct NCodecPdu){
                         .transport_type = NCodecPduTransportTypeFlexray,
                         .transport.flexray.metadata_type =
                             NCodecPduFlexrayMetadataTypeStatus,
                         .transport.flexray.metadata.status = { .cycle = 2,
                             .macrotick = 30 },
                     });
    ncodec_flush(nc);
    pdunet_rx(net, NULL, NULL, NULL);
    assert_double_equal(net->schedule.epoch_offset, 0.002, 0.0);
}


void test_pdunet_schedule_container_fr(void** state)
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

    PduNetworkDesc* net =
        pdunet_create(mock->ncodec, mock->doc, mock->step_size, mock->L, &dlog);
    mock->net = net;  // Teardown will destroy.
    assert_non_null(net);

    // Stub the SignalVector.
    const char* signal[] = {
        "SIG_1",
        "SIG_2",
        "SIG_3",
        "SIG_4",
        "SIG_5",
        "SIG_6",
    };
    double scalar[ARRAY_SIZE(signal)] = {};
    rc = pdunet_map_signals(net, "foo", ARRAY_SIZE(signal), signal, scalar);
    assert_int_equal(rc, 0);

    // Set some signals
    scalar[0] = 11;
    scalar[1] = 22;
    scalar[2] = 33;

    // Set initial condition (calculate checksum, set needs_tx to false).
    pdunet_visit(net, NULL, pdunet_visit_set_checksum, NULL);
    pdunet_visit(net, NULL, pdunet_visit_clear_tx_flag, NULL);
    {
        int count = 0;
        pdunet_visit(net, NULL, _visit_count_csum_set, &count);
        assert_int_equal(count, 4);
    }

    // Send vtable.config(), but no PDUs have needs_tx set.
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

    // Count set checksums.
    {
        int count = 0;
        pdunet_visit(net, NULL, _visit_count_csum_set, &count);
        assert_int_equal(count, 4);  // FIXME why not 4?
    }

    // Push the simulation to interval + phase - 1 step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0035);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. Tx.
    // __log_level__ = LOG_TRACE;
    pdunet_tx(net, NULL, NULL, NULL, 0.004);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){ 0 };
    len = ncodec_read(nc, &pdu);
    assert_true(len > 0);
    uint8_t expect[24] = {
        [0] = 0x00,  // header - 24bit : 403
        [1] = 0x01,
        [2] = 0x93,
        [3] = 0x08,
        [4] = 0x21,   // 33
        [12] = 0x00,  // header - 24bit : 401
        [13] = 0x01,
        [14] = 0x91,
        [15] = 0x08,
        [16] = 0x0b,  // 11
    };
    assert_memory_equal(pdu.payload, expect, 24);
    assert_int_equal(pdu.ecu_id, 5);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.009);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    pdu = (NCodecPdu){ 0 };
    len = ncodec_read(nc, &pdu);
    assert_true(len > 0);
    uint8_t expect2[24] = {
        [0] = 0x00,  // header - 24bit : 402
        [1] = 0x01,
        [2] = 0x92,
        [3] = 0x08,
        [4] = 0x16,   // 22
        [12] = 0x00,  // No PDU, 401/403 have no_change
        [13] = 0x00,
        [14] = 0x00,
        [15] = 0x00,
        [16] = 0x00,
    };
    assert_memory_equal(pdu.payload, expect2, 24);
    assert_int_equal(pdu.ecu_id, 5);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);

    // Push the simulation one step. No Tx.
    pdunet_tx(net, NULL, NULL, NULL, 0.0095);
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);
}


int run_pdunet_schedule_tests(void)
{
    void* s = test_setup;
    void* sc = test_setup_c;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_pdunet_schedule_pdu_fr, s, t),
        cmocka_unit_test_setup_teardown(test_pdunet_schedule_net_fr, s, t),
        cmocka_unit_test_setup_teardown(test_pdunet_schedule_status_fr, s, t),
        cmocka_unit_test_setup_teardown(
            test_pdunet_schedule_container_fr, sc, t),
    };

    return cmocka_run_group_tests_name("PDU Net::Schedule", tests, NULL, NULL);
}
