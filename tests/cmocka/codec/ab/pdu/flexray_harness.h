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
    const char*            mimetype;
    NCodecPduFlexrayConfig config;

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

typedef struct TestPduMap {
    TestPdu map[TEST_NODES][TEST_FRAMES];
} TestPduMap;
typedef struct TestFrameMap {
    NCodecPduFlexrayLpduConfig map[TEST_NODES][TEST_FRAMES];
} TestFrameMap;
typedef struct TestPduList {
    size_t  count;
    TestPdu list[TEST_PDUS];
} TestPduList;

typedef struct {
    /* Config */
    struct {
        TestNode     node[TEST_NODES];
        TestFrameMap frame_table;
    } config;

    /* Run Condition */
    struct {
        bool       push_active;
        uint8_t    push_at_cycle;
        TestPduMap pdu_map;

        NCodecPdu status_pdu[TEST_NODES];
        Vector    pdu_list;

        size_t cycles;
        size_t steps;
    } run;

    /* Expect */
    struct {
        /* End status. */
        uint8_t                          cycle;
        uint16_t                         macrotick;
        NCodecPduFlexrayPocState         poc_state;
        NCodecPduFlexrayTransceiverState tcvr_state;

        TestPduList pdu;
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


#endif  // TESTS_CMOCKA_CODEC_AB_PDU_FLEXRAY_HARNESS_H_
