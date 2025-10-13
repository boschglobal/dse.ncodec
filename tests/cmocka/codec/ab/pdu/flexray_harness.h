// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef TESTS_CMOCKA_CODEC_AB_PDU_FLEXRAY_HARNESS_H_
#define TESTS_CMOCKA_CODEC_AB_PDU_FLEXRAY_HARNESS_H_

#include <stdint.h>
#include <dse/ncodec/codec/ab/vector.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>


#define TEST_NODES  4
#define TEST_FRAMES 50
#define TEST_PDUS   100


typedef struct {
    const char*                    mimetype;
    NCodecPduFlexrayConfig         config;
    NCodecPduFlexrayNodeIdentifier node_ident;

    NCodecStreamVTable* stream;
    NCODEC*             nc;
} TestNode;

typedef struct {
    uint16_t    frame_config_index;
    uint16_t    slot_id;
    uint8_t     lpdu_status;
    const char* payload;
    uint8_t     payload_len;
    uint8_t     cycle;
    uint16_t    macrotick;
    bool        null_frame;

    NCodecPduFlexrayNodeIdentifier node_ident;
} TestPdu;


typedef struct TestFrameTable {
    NCodecPduFlexrayLpduConfig list[TEST_FRAMES];
} TestFrameTable;
typedef struct TestFrameMap {
    TestFrameTable map[TEST_NODES];
} TestFrameMap;


typedef struct TestPduTable {
    size_t  count;
    TestPdu list[TEST_FRAMES];
} TestPduTable;
typedef struct TestPduMap {
    TestPduTable map[TEST_NODES];
} TestPduMap;


typedef struct TestPduMapOld {
    TestPdu map[TEST_NODES][TEST_FRAMES];
} TestPduMapOld;
typedef struct TestPduList {
    size_t  count;
    TestPdu list[TEST_PDUS];
} TestPduList;

typedef struct TestPduPlayback {
    size_t    step;
    uint8_t   node_idx;
    NCodecPdu pdu;
} TestPduPlayback;

typedef struct TestPduTrace {
    NCodecPduFlexrayNodeIdentifier node_ident;
    Vector                         pdu_list; /* NCodecPdu */
} TestPduTrace;
typedef struct TestTraceMap {
    NCodecPdu map[TEST_NODES][TEST_FRAMES];
} TestTraceMap;


typedef struct {
    /* Config */
    struct {
        TestNode     node[TEST_NODES];
        TestFrameMap frame_table;
    } config;

    /* Run Condition */
    struct {
        bool       no_push;
        bool       push_active;
        uint8_t    push_at_cycle;
        TestPduMap pdu_map;

        NCodecPdu status_pdu[TEST_NODES];
        Vector    pdu_list;
        Vector    pdu_trace; /* TestPduTrace by node_id */

        size_t cycles;
        size_t steps;

        TestPduPlayback pop_playback_list[TEST_FRAMES];
    } run;

    /* Expect */
    struct {
        /* End status. */
        uint8_t                          cycle;
        uint16_t                         macrotick;
        NCodecPduFlexrayPocState         poc_state;
        NCodecPduFlexrayTransceiverState tcvr_state;

        TestPduList  pdu;
        TestTraceMap trace_map;
    } expect;
} TestTxRx;

typedef struct Mock {
    NCODEC* nc;

    TestTxRx test;
    uint8_t  loglevel_save;
} Mock;


int  test_setup(void** state);
int  test_teardown(void** state);
void flexray_harness_run_test(TestTxRx* test);
void flexray_harness_run_pop_test(TestTxRx* test);


#endif  // TESTS_CMOCKA_CODEC_AB_PDU_FLEXRAY_HARNESS_H_
