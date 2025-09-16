// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef TESTS_CMOCKA_CODEC_AB_PDU_FLEXRAY_HARNESS_H_
#define TESTS_CMOCKA_CODEC_AB_PDU_FLEXRAY_HARNESS_H_

#include <stdint.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>


#define TEST_NODES  10
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
} TestPdu;

typedef struct {
    /* Config */
    struct {
        TestNode                   node[TEST_NODES];
        NCodecPduFlexrayLpduConfig frame_table[TEST_NODES][TEST_FRAMES];
    } config;

    /* Run Condition */
    struct {
        bool    push_active;
        uint8_t push_at_cycle;
        TestPdu pdu[TEST_FRAMES];

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

        TestPdu pdu[TEST_PDUS];
        size_t  pdu_count;
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
