// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/logger.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <flexray_harness.h>


#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define BUFFER_LEN    1024

/*
Operation of Multi-Node test.

1/ Each node _WRITES_ to Write-NC Object.
2/ Each Write-NC Object the same Stream Object
3/ Write-NC-Steam connects TO the Bus Model (use any Write-NC Object).
4/ Single Read-NC Object FROM the Bus Model.
5/ Read triggers the Bus Model.

Strategy:
* Truncate Write-NC Objects.
* Write to all Write-NC Object.
* Flush all Write-NC Objects.
* Read from single Read-NC Object.
* Truncate Read-NC Object.

*/


int test_setup(void** state)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);
    mock->loglevel_save = __log_level__;

    *state = mock;
    return 0;
}

static void __pdu_payload_destory(void* item, void* data)
{
    UNUSED(data);

    NCodecPdu* pdu = item;
    free((uint8_t*)pdu->payload);
    pdu->payload = NULL;
}

static void __pdu_trace_destory(void* item, void* data)
{
    UNUSED(data);
    TestPduTrace* pdu_trace = item;

    vector_clear(&pdu_trace->pdu_list, __pdu_payload_destory, NULL);
    vector_reset(&pdu_trace->pdu_list);
}

int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock == NULL) return 0;

    if (mock->nc) ncodec_close((void*)mock->nc);
    for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
        if (mock->test.config.node[n_idx].nc) {
            ncodec_close((void*)mock->test.config.node[n_idx].nc);
        }
    }
    vector_clear(&mock->test.run.pdu_list, __pdu_payload_destory, NULL);
    vector_reset(&mock->test.run.pdu_list);
    vector_clear(&mock->test.run.pdu_trace, __pdu_trace_destory, NULL);
    vector_reset(&mock->test.run.pdu_trace);

    __log_level__ = mock->loglevel_save;
    free(mock);
    return 0;
}


static int __node_ident_compar(const void* left, const void* right)
{
    NCodecPduFlexrayNodeIdentifier l = ((TestPduTrace*)left)->node_ident;
    NCodecPduFlexrayNodeIdentifier r = ((TestPduTrace*)right)->node_ident;

    if (l.node_id < r.node_id) return -1;
    if (l.node_id > r.node_id) return 1;
    return 0;
}

static void _setup_nodes(TestTxRx* test)
{
    for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
        if (test->config.node[n_idx].mimetype == NULL) {
            break;
        }
        test->config.node[n_idx].stream =
            ncodec_buffer_stream_create(BUFFER_LEN);
        test->config.node[n_idx].nc = (void*)ncodec_open(
            test->config.node[n_idx].mimetype, test->config.node[n_idx].stream);
        assert_non_null(test->config.node[n_idx].nc);
        ncodec_truncate(test->config.node[n_idx].nc);
    }

    test->run.pdu_list = vector_make(sizeof(NCodecPdu), 0, NULL);
    test->run.pdu_trace =
        vector_make(sizeof(TestPduTrace), 0, __node_ident_compar);
}

static void _push_nodes(TestTxRx* test)
{
    int rc;

    /* Calculate frame counts. */
    for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
        if (test->config.node[n_idx].nc == NULL) break;
        for (size_t i = 0; i < TEST_FRAMES; i++) {
            if (test->config.frame_table.map[n_idx][i].slot_id == 0) {
                test->config.node[n_idx].config.frame_config.count = i;
                break;
            }
        }
    }

    /**
     * The effect of a SimBus must be created. Each node produces a
     * sequence of messages, the SimBus combines the sequences from each node
     * to a buffer, that buffer then becomes the input for each node.
     *
     * To recreate the effect; for each node, get the stream object, then
     * for each node, produce a message sequence using _that_ stream object
     * and the properties of the nodes NC object, and flush to the stream.
     */

    /* For each Node. */
    for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
        NCODEC* nc = test->config.node[n_idx].nc;
        if (nc == NULL) break;

        /* Push messages from each node to this Nodes NC Object. */
        for (size_t nc_idx = 0; nc_idx < TEST_NODES; nc_idx++) {
            if (test->config.node[nc_idx].nc == NULL) break;

            /* Underlying stream object is the same. */
            NCODEC*         nc2 = test->config.node[nc_idx].nc;
            NCodecInstance* _nc2 = (NCodecInstance*)nc2;
            _nc2->stream = test->config.node[n_idx].stream;

            NCodecPduFlexrayConfig config = test->config.node[nc_idx].config;
            config.frame_config.table = test->config.frame_table.map[nc_idx];
            rc = ncodec_write(nc2,
                &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray = {
                        .metadata_type = NCodecPduFlexrayMetadataTypeConfig,
                        .metadata.config = config,
                    } });

            if (test->run.push_active == false) goto end__push_nodes__active;

            rc = ncodec_write(nc2,
                &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray = {
                        .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .metadata.status = {
                            .channel[0].poc_command =
                                NCodecPduFlexrayCommandConfig,
                        } } });
            rc |= ncodec_write(nc2,
                &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray = {
                        .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .metadata.status = {
                            .channel[0].poc_command =
                                NCodecPduFlexrayCommandReady,
                        } } });
            rc |= ncodec_write(nc2,
                &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray = {
                        .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                        .metadata.status = {
                            .channel[0].poc_command =
                                NCodecPduFlexrayCommandRun,
                        } } });
            assert_int_equal(rc, 0);

        end__push_nodes__active:

            /* Flush N messages to this Nodes NC Object. */
            ncodec_flush(nc2);
            /* Restore the stream. */
            _nc2->stream = test->config.node[nc_idx].stream;
        }
    }
}

static void _push_frames(TestTxRx* test)
{
    int rc;

    /**
     * The effect of a SimBus must be created. Each node produces a
     * sequence of messages, the SimBus combines the sequences from each node
     * to a buffer, that buffer then becomes the input for each node.
     *
     * To recreate the effect; for each node, get the stream object, then
     * for each node, produce a message sequence using _that_ stream object
     * and the properties of the nodes NC object, and flush to the stream.
     */

    /* For each Node. */
    for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
        NCODEC* nc = test->config.node[n_idx].nc;
        if (nc == NULL) break;

        /* Push messages from each node to this Nodes NC Object. */
        for (size_t nc_idx = 0; nc_idx < TEST_NODES; nc_idx++) {
            if (test->config.node[nc_idx].nc == NULL) break;

            /* Underlying stream object is the same. */
            NCODEC*         nc2 = test->config.node[nc_idx].nc;
            NCodecInstance* _nc2 = (NCodecInstance*)nc2;
            _nc2->stream = test->config.node[n_idx].stream;

            for (size_t p_idx = 0; p_idx < TEST_FRAMES; p_idx++) {
                if (test->run.pdu_map.map[nc_idx][p_idx].slot_id == 0) {
                    break;
                }
                rc = ncodec_write(nc2,
                    &(NCodecPdu){
                        .id = test->run.pdu_map.map[nc_idx][p_idx].slot_id,
                        .payload =
                            (const uint8_t*)test->run.pdu_map.map[nc_idx][p_idx]
                                .payload,
                        .payload_len =
                            test->run.pdu_map.map[nc_idx][p_idx].payload_len,
                        .transport_type = NCodecPduTransportTypeFlexray,
                        .transport.flexray = {
                            .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                            .metadata.lpdu = {
                                .frame_config_index =
                                    test->run.pdu_map.map[nc_idx][p_idx]
                                        .frame_config_index,
                                .status = test->run.pdu_map.map[nc_idx][p_idx]
                                              .lpdu_status,
                            } } });
                assert_int_equal(
                    rc, test->run.pdu_map.map[nc_idx][p_idx].payload_len);
            }

            /* Flush N messages to this Nodes NC Object. */
            ncodec_flush(nc2);
            /* Restore the stream. */
            _nc2->stream = test->config.node[nc_idx].stream;
        }
    }
}

static void _run_network(TestTxRx* test)
{
#define CYCLE_STEPS_MAX (5 * 2 + 5) /* 1 cycle = 5 ms = 10 steps (+5)*/
    int       rc;
    NCodecPdu pdu;
    uint8_t   cycle = 0;
    uint8_t   cycle_loop = 0;
    size_t    cycle_steps = 0;

    for (size_t i = 0; (test->run.steps == 0) || (i < test->run.steps); i++) {
        cycle_steps += 1;
        if (test->run.push_at_cycle && test->run.push_at_cycle == cycle) {
            _push_frames(test);
            test->run.push_at_cycle = 0; /* Disable further pushes. */
        }
        if ((cycle_loop * 64u + cycle) == (test->run.cycles + 1)) {
            break;
        }
        if (cycle_steps > CYCLE_STEPS_MAX) {
            fail_msg("cycle limit exceeded, network not progressing");
            return;
        }

        for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
            NCODEC* nc = test->config.node[n_idx].nc;
            if (nc == NULL) break;

            /* Reset the stream pointer for reading. */
            ncodec_seek(nc, 0, NCODEC_SEEK_SET);

            /* Read a PDU (triggers Bus Model). */
            pdu = (NCodecPdu){ 0 };
            rc = ncodec_read(nc, &pdu);
            assert_int_equal(rc, 0);

            /* First PDU is status. */
            assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
            assert_int_equal(NCodecPduFlexrayMetadataTypeStatus,
                pdu.transport.flexray.metadata_type);
            test->run.status_pdu[n_idx] = pdu;

            /* Cycle progression. */
            if (n_idx == 0 &&
                cycle != pdu.transport.flexray.metadata.status.cycle) {
                cycle = pdu.transport.flexray.metadata.status.cycle;
                if (cycle == 0) {
                    cycle_loop++;
                }
                cycle_steps = 0;
            }

            /* Read remaining PDUs. */
            while ((rc = ncodec_read(nc, &pdu)) >= 0) {
                if (pdu.transport_type != NCodecPduTransportTypeFlexray)
                    continue;
                if (pdu.transport.flexray.metadata_type !=
                    NCodecPduFlexrayMetadataTypeLpdu)
                    continue;
                if (pdu.payload_len) {
                    uint8_t* payload = malloc(pdu.payload_len);
                    memcpy(payload, pdu.payload, pdu.payload_len);
                    pdu.payload = payload;
                }
                vector_push(&test->run.pdu_list, &pdu);
            }

            ncodec_truncate(nc);
            ncodec_flush(nc);
        }
    }
}

static void _expect_status_check(TestTxRx* test)
{
    for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
        NCODEC* nc = test->config.node[n_idx].nc;
        if (nc == NULL) break;
        assert_int_equal(
            test->expect.cycle, test->run.status_pdu[n_idx]
                                    .transport.flexray.metadata.status.cycle);
        assert_int_equal(test->expect.macrotick,
            test->run.status_pdu[n_idx]
                .transport.flexray.metadata.status.macrotick);
        assert_int_equal(test->expect.poc_state,
            test->run.status_pdu[n_idx]
                .transport.flexray.metadata.status.channel[0]
                .poc_state);
        assert_int_equal(test->expect.tcvr_state,
            test->run.status_pdu[n_idx]
                .transport.flexray.metadata.status.channel[0]
                .tcvr_state);
    }
}

static void _expect_pdu_check(TestTxRx* test)
{
    assert_int_equal(test->expect.pdu.count, vector_len(&test->run.pdu_list));
    for (size_t i = 0; i < TEST_PDUS; i++) {
        if (test->expect.pdu.list[i].slot_id == 0) {
            break;
        }
        log_info("Check PDU at index %u (slot_id=%u)", i,
            test->expect.pdu.list[i].slot_id);

        NCodecPdu pdu;
        vector_at(&test->run.pdu_list, i, &pdu);
        assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
        assert_int_equal(NCodecPduFlexrayMetadataTypeLpdu,
            pdu.transport.flexray.metadata_type);
        assert_int_equal(test->expect.pdu.list[i].lpdu_status,
            pdu.transport.flexray.metadata.lpdu.status);
        assert_int_equal(test->expect.pdu.list[i].cycle,
            pdu.transport.flexray.metadata.lpdu.cycle);
        if (test->expect.pdu.list[i].macrotick) {
            assert_int_equal(test->expect.pdu.list[i].macrotick,
                pdu.transport.flexray.metadata.lpdu.macrotick);
        }
        if (test->expect.pdu.list[i].null_frame) {
            assert_int_equal(0, pdu.payload_len);
            assert_null(pdu.payload);
            assert_true(pdu.transport.flexray.metadata.lpdu.null_frame);
        } else {
            assert_memory_equal(test->expect.pdu.list[i].payload, pdu.payload,
                test->expect.pdu.list[i].payload_len);
            assert_false(pdu.transport.flexray.metadata.lpdu.null_frame);
        }
        if (test->expect.pdu.list[i].node_ident.node_id) {
            assert_int_equal(test->expect.pdu.list[i].node_ident.node_id,
                pdu.transport.flexray.node_ident.node_id);
        }
    }
}


void flexray_harness_run_test(TestTxRx* test)
{
    _setup_nodes(test);
    _push_nodes(test);
    if (test->run.push_at_cycle == 0) {
        _push_frames(test);
    }
    _run_network(test);
    _expect_status_check(test);
    _expect_pdu_check(test);
}


static void _run_pop(TestTxRx* test)
{
#define CYCLE_STEPS_MAX (5 * 2 + 5) /* 1 cycle = 5 ms = 10 steps (+5)*/
    int       rc;
    NCodecPdu pdu;
    uint8_t   cycle = 0;
    uint8_t   cycle_loop = 0;
    size_t    cycle_steps = 0;

    for (size_t i = 0; (test->run.steps == 0) || (i < test->run.steps); i++) {
        cycle_steps += 1;
        if (test->run.push_at_cycle && test->run.push_at_cycle == cycle) {
            _push_frames(test);
            test->run.push_at_cycle = 0; /* Disable further pushes. */
        }
        if ((cycle_loop * 64u + cycle) == (test->run.cycles + 1)) {
            break;
        }
        if (cycle_steps > CYCLE_STEPS_MAX) {
            fail_msg("cycle limit exceeded, not progressing");
            return;
        }

        for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
            NCODEC* nc = test->config.node[n_idx].nc;
            if (nc == NULL) break;
            NCodecPduFlexrayNodeIdentifier node_ident =
                test->config.node[n_idx].node_ident;

            /* Get a trace. */
            TestPduTrace* pdu_trace = vector_find(&test->run.pdu_trace,
                &(TestPduTrace){ .node_ident = node_ident }, 0, NULL);
            if (pdu_trace == NULL) {
                vector_push(&test->run.pdu_trace,
                    &(TestPduTrace){
                        .node_ident = node_ident,
                        .pdu_list = vector_make(sizeof(NCodecPdu), 0, NULL),
                    });
                vector_sort(&test->run.pdu_trace);
                pdu_trace = vector_find(&test->run.pdu_trace,
                    &(TestPduTrace){ .node_ident = node_ident }, 0, NULL);
            }
            assert_non_null(pdu_trace);

            /* Reset the stream pointer for reading. */
            ncodec_seek(nc, 0, NCODEC_SEEK_SET);

            /* Read from NC. */
            log_info("POP:READ: step=%u", i);
            size_t pdu_count = 0;
            while ((rc = ncodec_read(nc, &pdu)) >= 0) {
                if (pdu.transport_type != NCodecPduTransportTypeFlexray)
                    continue;
                pdu_count++;
                // Push to trace.
                if (pdu.payload_len) {
                    uint8_t* payload = malloc(pdu.payload_len);
                    memcpy(payload, pdu.payload, pdu.payload_len);
                    pdu.payload = payload;
                }
                vector_push(&pdu_trace->pdu_list, &pdu);
                // Checks.
                if (pdu_count == 1 && node_ident.node_id != 0) {
                    /* First PDU is status. Exclude POP. */
                    assert_int_equal(
                        NCodecPduTransportTypeFlexray, pdu.transport_type);
                    assert_int_equal(NCodecPduFlexrayMetadataTypeStatus,
                        pdu.transport.flexray.metadata_type);
                    test->run.status_pdu[n_idx] = pdu;
                }
            }
            assert_true(pdu_count > 0);

            ncodec_truncate(nc);
            ncodec_flush(nc);
        }
    }
}


static void _dump_trace(TestTxRx* test)
{
    for (size_t i = 0; i < vector_len(&test->run.pdu_trace); i++) {
        TestPduTrace* pdu_trace = vector_at(&test->run.pdu_trace, i, NULL);
        for (size_t j = 0; j < vector_len(&pdu_trace->pdu_list); j++) {
            NCodecPdu* pdu = vector_at(&pdu_trace->pdu_list, j, NULL);

            switch (pdu->transport.flexray.metadata_type) {
            case (NCodecPduFlexrayMetadataTypeConfig):
                log_info("[%u][%u]:(%u:%u:%u) Config", i, j,
                    pdu->transport.flexray.node_ident.node.ecu_id,
                    pdu->transport.flexray.node_ident.node.cc_id,
                    pdu->transport.flexray.node_ident.node.swc_id);
                break;
            case (NCodecPduFlexrayMetadataTypeStatus):
                log_info("[%u][%u]:(%u:%u:%u) Status", i, j,
                    pdu->transport.flexray.node_ident.node.ecu_id,
                    pdu->transport.flexray.node_ident.node.cc_id,
                    pdu->transport.flexray.node_ident.node.swc_id);
                break;
            case (NCodecPduFlexrayMetadataTypeLpdu):
                log_info("[%u][%u]:(%u:%u:%u) LPDU", i, j,
                    pdu->transport.flexray.node_ident.node.ecu_id,
                    pdu->transport.flexray.node_ident.node.cc_id,
                    pdu->transport.flexray.node_ident.node.swc_id);
                break;
            default:
                log_error("trace bad");
                break;
            }
        }
    }
}


static void _expect_trace_map_check(TestTxRx* test)
{
    for (size_t n_idx = 0; n_idx < TEST_NODES; n_idx++) {
        for (size_t p_idx = 0; p_idx < TEST_NODES; p_idx++) {
            NCodecPdu* pdu = &test->expect.trace_map.map[n_idx][p_idx];
            if (pdu->transport_type == 0) break;
            if (pdu->transport_type != NCodecPduTransportTypeFlexray) continue;
            if (pdu->transport.flexray.metadata_type == 0) continue;

            log_trace("Locate trace item: [%u][%u]", n_idx, p_idx);
            TestPduTrace* pdu_trace =
                vector_at(&test->run.pdu_trace, n_idx, NULL);
            if (pdu_trace == NULL) fail_msg("Trace not found at [%u]", n_idx);
            NCodecPdu* trace_pdu = vector_at(&pdu_trace->pdu_list, p_idx, NULL);
            if (trace_pdu == NULL)
                fail_msg("Trace item not found at [%u][%u]", n_idx, p_idx);

            assert_int_equal(trace_pdu->transport.flexray.node_ident.node_id,
                pdu->transport.flexray.node_ident.node_id);
            assert_int_equal(
                trace_pdu->transport.flexray.pop_node_ident.node_id,
                pdu->transport.flexray.pop_node_ident.node_id);
            switch (pdu->transport.flexray.metadata_type) {
            case (NCodecPduFlexrayMetadataTypeStatus):
                if (pdu->transport.flexray.metadata.status.channel[0]
                        .tcvr_state) {
                    assert_int_equal(
                        trace_pdu->transport.flexray.metadata.status.channel[0]
                            .tcvr_state,
                        pdu->transport.flexray.metadata.status.channel[0]
                            .tcvr_state);
                }
                break;
            default:
                break;
            }
        }
    }
}


void flexray_harness_run_pop_test(TestTxRx* test)
{
    _setup_nodes(test);
    _push_nodes(test);
    if (test->run.push_active && test->run.push_at_cycle == 0) {
        _push_frames(test);
    }
    _run_pop(test);
    _dump_trace(test);
    _expect_trace_map_check(test);
}
