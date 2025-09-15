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

int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock == NULL) return 0;

    if (mock->nc) ncodec_close((void*)mock->nc);
    if (mock->test.config.node[0].nc)
        ncodec_close((void*)mock->test.config.node[0].nc);
    vector_clear(&mock->test.run.pdu_list, __pdu_payload_destory, NULL);
    vector_reset(&mock->test.run.pdu_list);

    __log_level__ = mock->loglevel_save;
    free(mock);
    return 0;
}


static void _setup_nodes(TestTxRx* test)
{
    test->config.node[0].stream = ncodec_buffer_stream_create(BUFFER_LEN);
    test->config.node[0].nc = (void*)ncodec_open(
        test->config.node[0].mimetype, test->config.node[0].stream);
    assert_non_null(test->config.node[0].nc);

    test->run.pdu_list = vector_make(sizeof(NCodecPdu), 0, NULL);

    NCODEC* nc = test->config.node[0].nc;
    ncodec_truncate(nc);
}

static void _push_nodes(TestTxRx* test)
{
    int                    rc;
    NCODEC*                nc = test->config.node[0].nc;
    NCodecPduFlexrayConfig config = test->config.node[0].config;
    config.frame_config.table = test->config.frame_table[0];
    for (size_t i = 0; i < TEST_FRAMES; i++) {
        if (test->config.frame_table[0][i].slot_id == 0) {
            config.frame_config.count = i;
            break;
        }
    }

    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeConfig,
                    .metadata.config = config,
                } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandConfig,
                    } } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandReady,
                    } } });
    rc = ncodec_write(
        nc, &(NCodecPdu){ .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = {
                    .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                    .metadata.status = {
                        .channel[0].poc_command = NCodecPduFlexrayCommandRun,
                    } } });
    assert_int_equal(rc, 0);
    ncodec_flush(nc);
}

static void _push_frames(TestTxRx* test)
{
    int                    rc;
    NCODEC*                nc = test->config.node[0].nc;
    NCodecPduFlexrayConfig config = test->config.node[0].config;

    for (size_t i = 0; i < TEST_FRAMES; i++) {
        if (test->run.pdu[i].lpdu_status == 0) {
            break;
        }
        rc = ncodec_write(
            nc, &(NCodecPdu){ .id = test->run.pdu[i].slot_id,
                    .payload = (const uint8_t*)test->run.pdu[i].payload,
                    .payload_len = test->run.pdu[i].payload_len,
                    .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray = {
                        .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                        .metadata.lpdu = {
                            .frame_config_index =
                                test->run.pdu[i].frame_config_index,
                            .status = test->run.pdu[i].lpdu_status,
                        } } });
        assert_int_equal(rc, test->run.pdu[i].payload_len);
    }

    ncodec_flush(nc);
}

static void _run_network(TestTxRx* test)
{
    int       rc;
    NCODEC*   nc = test->config.node[0].nc;
    NCodecPdu pdu;
    uint8_t   cycle = 0;
    uint8_t   cycle_loop = 0;


    for (size_t i = 0; (test->run.steps == 0) || (i < test->run.steps); i++) {
        if (test->run.push_at_cycle && test->run.push_at_cycle == cycle) {
            _push_frames(test);
            test->run.push_at_cycle = 0; /* Disable futher pushes. */
        }
        if ((cycle_loop * 64u + cycle) == (test->run.cycles + 1)) {
            break;
        }

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
        test->run.status_pdu[0] = pdu;
        if (pdu.transport.flexray.metadata.status.cycle < cycle) {
            cycle_loop++;
        }
        cycle = pdu.transport.flexray.metadata.status.cycle;

        /* Read remaining PDUs. */
        while ((rc = ncodec_read(nc, &pdu)) >= 0) {
            if (pdu.transport_type != NCodecPduTransportTypeFlexray) continue;
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

static void _expect_status_check(TestTxRx* test)
{
    assert_int_equal(test->expect.cycle,
        test->run.status_pdu[0].transport.flexray.metadata.status.cycle);
    assert_int_equal(test->expect.macrotick,
        test->run.status_pdu[0].transport.flexray.metadata.status.macrotick);
    assert_int_equal(test->expect.poc_state,
        test->run.status_pdu[0]
            .transport.flexray.metadata.status.channel[0]
            .poc_state);
    assert_int_equal(test->expect.tcvr_state,
        test->run.status_pdu[0]
            .transport.flexray.metadata.status.channel[0]
            .tcvr_state);
}

static void _expect_pdu_check(TestTxRx* test)
{
    assert_int_equal(test->expect.pdu_count, vector_len(&test->run.pdu_list));
    for (size_t i = 0; i < TEST_PDUS; i++) {
        if (test->expect.pdu[i].slot_id == 0) {
            break;
        }

        NCodecPdu pdu;
        vector_at(&test->run.pdu_list, i, &pdu);
        assert_int_equal(NCodecPduTransportTypeFlexray, pdu.transport_type);
        assert_int_equal(NCodecPduFlexrayMetadataTypeLpdu,
            pdu.transport.flexray.metadata_type);
        assert_int_equal(test->expect.pdu[i].lpdu_status,
            pdu.transport.flexray.metadata.lpdu.status);
        assert_int_equal(test->expect.pdu[i].cycle,
            pdu.transport.flexray.metadata.lpdu.cycle);
        if (test->expect.pdu[i].macrotick) {
            assert_int_equal(test->expect.pdu[i].macrotick,
                pdu.transport.flexray.metadata.lpdu.macrotick);
        }
        assert_memory_equal(test->expect.pdu[i].payload, pdu.payload,
            test->expect.pdu[i].payload_len);
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
