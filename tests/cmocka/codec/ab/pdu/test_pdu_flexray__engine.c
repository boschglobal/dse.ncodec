// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/logger.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>

#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define BUFFER_LEN    1024
#define SIM_STEP_SIZE 0.0005


extern NCodecConfigItem codec_stat(NCODEC* nc, int* index);
extern NCODEC*          ncodec_create(const char* mime_type);
extern int32_t stream_read(NCODEC* nc, uint8_t** data, size_t* len, int pos_op);


typedef struct Mock {
    NCODEC*       nc;
    FlexrayEngine engine;
    uint8_t       loglevel_save;
} Mock;


#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=1;cc_id=0;swc_id=1;vcn=2"


static NCodecPduFlexrayConfig cc_config = {
    .bit_rate = NCodecPduFlexrayBitrate10,
    .channel_enable = NCodecPduFlexrayChannelA,
    .macrotick_per_cycle = 3361u,
    .microtick_per_cycle = 200000u,
    .network_idle_start = (3361u - 5u - 1u),
    .static_slot_length = 55u,
    .static_slot_count = 38u,
    .minislot_length = 6u,
    .minislot_count = 211u,
    .static_slot_payload_length = (32u * 2), /* Word to Byte */

    .coldstart_node = false,
    .sync_node = false,
    .coldstart_attempts = 8u,
    .wakeup_channel_select = 0, /* Channel A */
    .single_slot_enabled = false,
    .key_slot_id = 0u,

    .inhibit_null_frames = true,
};

static NCodecPduFlexrayLpduConfig frame_config__empty[0] = {};


static int test_setup(void** state)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);

    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(MIMETYPE, stream);
    assert_non_null(mock->nc);
    mock->engine = (FlexrayEngine){ 0 };
    mock->loglevel_save = __log_level__;

    *state = mock;
    return 0;
}


static int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock && mock->nc) ncodec_close((void*)mock->nc);
    release_config(&mock->engine);

    __log_level__ = mock->loglevel_save;
    free(mock);
    return 0;
}


void test_flexray__communication_parameters(void** state)
{
    Mock*                       mock = *state;
    NCodecPduFlexrayConfig      config = cc_config;
    NCodecPduFlexrayLpduConfig* frame_table = frame_config__empty;
    config.frame_config.table = frame_table;
    config.frame_config.count = ARRAY_SIZE(frame_config__empty);

    FlexrayEngine* engine = &mock->engine;
    *engine = (FlexrayEngine){ .sim_step_size = SIM_STEP_SIZE };

    NCodecPdu pdu = {
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeConfig,
    };

    // Check calculated parameters.
    pdu.transport.flexray.metadata.config = config;
    assert_int_equal(0, process_config(&pdu, engine));

    assert_int_equal(200000, engine->microtick_per_cycle);
    assert_int_equal(3361, engine->macrotick_per_cycle);

    assert_int_equal(55, engine->static_slot_length_mt);
    assert_int_equal(38, engine->static_slot_count);
    assert_int_equal(6, engine->minislot_length_mt);
    assert_int_equal(211, engine->minislot_count);
    assert_int_equal(64, engine->static_slot_payload_length);

    assert_int_equal(
        0.0005, engine->sim_step_size); /* Fixed cadence, optional. */
    assert_int_equal(25, engine->microtick_ns);
    assert_int_equal(59, engine->macro2micro);    /* 200000 / 3361 */
    assert_int_equal(1475, engine->macrotick_ns); /* 59 * 25 */
    assert_int_equal(0, engine->step_budget_ut);
    assert_int_equal(0, engine->step_budget_mt);
    assert_int_equal(0, engine->offset_static_mt);     /* 0 */
    assert_int_equal(2090, engine->offset_dynamic_mt); /* 55 * 38 */
    assert_int_equal(3355, engine->offset_network_mt); /* 3361u  - 5u  - 1u */
    assert_int_equal(88, engine->bits_per_minislot);   /* 1475 * 6 / 100 */

    // Budget allocation.
    assert_int_equal(0, calculate_budget(engine, 0));
    assert_int_equal(20000, engine->step_budget_ut); /* 500000 / 25 */
    assert_int_equal(338, engine->step_budget_mt);   /* 20000 / 59 */
    assert_int_equal(0, calculate_budget(engine, SIM_STEP_SIZE));
    assert_int_equal(40000, engine->step_budget_ut);
    assert_int_equal(677, engine->step_budget_mt); /* 40000 / 59 */

    // Position
    assert_int_equal(0, engine->pos_mt);
    assert_int_equal(1, engine->pos_slot);
    assert_int_equal(0, engine->pos_cycle);


    // TMerge Config.
    __log_level__ = LOG_FATAL;
    config.static_slot_length = 4;
    pdu.transport.flexray.metadata.config = config;
    assert_int_equal(-EINVAL, process_config(&pdu, engine));
    assert_int_equal(55, engine->static_slot_length_mt);

    config.bit_rate = 0; /* Null config / No config. */
    pdu.transport.flexray.metadata.config = config;
    assert_int_equal(-EINVAL, process_config(&pdu, engine));
    assert_int_equal(55, engine->static_slot_length_mt);
}

typedef struct {
    uint32_t slot;
    uint32_t mt;
} CycleCheck;

void test_flexray__engine_cycle__empty_frame_config(void** state)
{
    Mock*                       mock = *state;
    NCodecPduFlexrayConfig      config = cc_config;
    NCodecPduFlexrayLpduConfig* frame_table = frame_config__empty;
    config.frame_config.table = frame_table;
    config.frame_config.count = ARRAY_SIZE(frame_config__empty);
    NCodecPdu pdu = {
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeConfig,
        .transport.flexray.metadata.config = config,
    };

    FlexrayEngine* engine = &mock->engine;
    *engine = (FlexrayEngine){ 0 };
    assert_int_equal(0, process_config(&pdu, engine));

    CycleCheck checks[] = {
        { .slot = 1, .mt = 0 },
        { .slot = 7, .mt = 330 },
        { .slot = 13, .mt = 660 },
        { .slot = 19, .mt = 990 },
        { .slot = 25, .mt = 1320 },
        { .slot = 31, .mt = 1650 },
        { .slot = 37, .mt = 1980 },
        { .slot = 86, .mt = 2372 },
        { .slot = 142, .mt = 2708 },
        { .slot = 199, .mt = 3050 },
        { .slot = 1, .mt = 0 },
    };
    for (size_t step = 0; step < ARRAY_SIZE(checks); step++) {
        /* Check the position. */
        assert_int_equal(checks[step].slot, engine->pos_slot);
        assert_int_equal(checks[step].mt, engine->pos_mt);

        /* Calc the budget. */
        uint32_t budget = engine->step_budget_ut;
        size_t   slot = engine->pos_slot;
        assert_int_equal(0, calculate_budget(engine, SIM_STEP_SIZE));
        assert_int_equal(budget + 20000, engine->step_budget_ut);
        assert_int_equal(
            (budget + 20000) / engine->macro2micro, engine->step_budget_mt);

        /* Consume the budget. */
        for (; consume_slot(engine) == 0;) {
        }
    }
    assert_int_equal(1, engine->pos_cycle);
}

void test_flexray__engine_cycle__with_frame_config(void** state)
{
    Mock*                      mock = *state;
    NCodecPduFlexrayConfig     config = cc_config;
    NCodecPduFlexrayLpduConfig frame_table[] = {
        { .slot_id = 24,
            .payload_length = 64,
            .base_cycle = 0,
            .cycle_repetition = 1 },
        { .slot_id = 155,
            .payload_length = 64,
            .direction = NCodecPduFlexrayDirectionTx,
            .status = NCodecPduFlexrayLpduStatusNotTransmitted },
    };
    config.frame_config.table = frame_table;
    config.frame_config.count = ARRAY_SIZE(frame_table);
    NCodecPdu pdu = {
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeConfig,
        .transport.flexray.metadata.config = config,
    };
    FlexrayEngine* engine = &mock->engine;
    *engine = (FlexrayEngine){ 0 };
    assert_int_equal(0, process_config(&pdu, engine));

    CycleCheck checks[] = {
        { .slot = 1, .mt = 0 },
        { .slot = 7, .mt = 330 },
        { .slot = 13, .mt = 660 },
        { .slot = 19, .mt = 990 }, /* 24 has frame with payload 64. */
        { .slot = 25, .mt = 1320 },
        { .slot = 31, .mt = 1650 },
        { .slot = 37, .mt = 1980 },
        { .slot = 86, .mt = 2372 },
        { .slot = 142, .mt = 2708 }, /* 155 has frame with payload 64 */
        { .slot = 193, .mt = 3050 },
        { .slot = 1, .mt = 0 },
    };
    for (size_t step = 0; step < ARRAY_SIZE(checks); step++) {
        /* Check the position. */
        assert_int_equal(checks[step].slot, engine->pos_slot);
        assert_int_equal(checks[step].mt, engine->pos_mt);

        /* Calc the budget. */
        uint32_t budget = engine->step_budget_ut;
        size_t   slot = engine->pos_slot;
        assert_int_equal(0, calculate_budget(engine, SIM_STEP_SIZE));
        assert_int_equal(budget + 20000, engine->step_budget_ut);
        assert_int_equal(
            (budget + 20000) / engine->macro2micro, engine->step_budget_mt);

        /* Consume the budget. */
        for (; consume_slot(engine) == 0;) {
        }
    }
    assert_int_equal(1, engine->pos_cycle);
}

void test_flexray__engine_cycle__wrap(void** state)
{
    Mock*                       mock = *state;
    NCodecPduFlexrayConfig      config = cc_config;
    NCodecPduFlexrayLpduConfig* frame_table = frame_config__empty;
    config.frame_config.table = frame_table;
    config.frame_config.count = ARRAY_SIZE(frame_config__empty);
    NCodecPdu pdu = {
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeConfig,
        .transport.flexray.metadata.config = config,
    };
    FlexrayEngine* engine = &mock->engine;
    *engine = (FlexrayEngine){ 0 };
    assert_int_equal(0, process_config(&pdu, engine));

    CycleCheck checks[] = {
        { .slot = 1, .mt = 0 },
        { .slot = 7, .mt = 330 },
        { .slot = 13, .mt = 660 },
        { .slot = 19, .mt = 990 },
        { .slot = 25, .mt = 1320 },
        { .slot = 31, .mt = 1650 },
        { .slot = 37, .mt = 1980 },
        { .slot = 86, .mt = 2372 },
        { .slot = 142, .mt = 2708 },
        { .slot = 199, .mt = 3050 },
    };
    /* Now run the check loop, for cycles ... */
    for (size_t cycle = 0; cycle < 65; cycle++) {
        for (size_t step = 0; step < ARRAY_SIZE(checks); step++) {
            /* Check the position. */
            assert_int_equal(checks[step].slot, engine->pos_slot);
            assert_int_equal(checks[step].mt, engine->pos_mt);
            assert_int_equal(cycle % 64, engine->pos_cycle);

            /* Calc the budget. */
            uint32_t budget = engine->step_budget_ut;
            size_t   slot = engine->pos_slot;
            assert_int_equal(0, calculate_budget(engine, SIM_STEP_SIZE));
            assert_int_equal(budget + 20000, engine->step_budget_ut);
            assert_int_equal(
                (budget + 20000) / engine->macro2micro, engine->step_budget_mt);

            /* Consume the budget. */
            for (; consume_slot(engine) == 0;) {
            }
        }
    }
    assert_int_equal(1, engine->pos_cycle);
}

void test_flexray__engine_cycle__shift(void** state)
{
    Mock*                       mock = *state;
    NCodecPduFlexrayConfig      config = cc_config;
    NCodecPduFlexrayLpduConfig* frame_table = frame_config__empty;
    config.frame_config.table = frame_table;
    config.frame_config.count = ARRAY_SIZE(frame_config__empty);
    NCodecPdu pdu = {
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeConfig,
        .transport.flexray.metadata.config = config,
    };
    FlexrayEngine* engine = &mock->engine;
    *engine = (FlexrayEngine){ 0 };
    assert_int_equal(0, process_config(&pdu, engine));

    /* Shift into dynamic part should fail. */
    assert_int_equal(1, shift_cycle(engine, 55 * 38, 4, false));
    assert_int_equal(1, engine->pos_slot);
    assert_int_equal(0, engine->pos_mt);
    assert_int_equal(0, engine->pos_cycle);

    /* Shift MT to 1000 somewhere in the static part (external sync event). */
    assert_int_equal(0, shift_cycle(engine, 1100, 4, false));
    assert_int_equal(21, engine->pos_slot);
    assert_int_equal(1100, engine->pos_mt);
    assert_int_equal(4, engine->pos_cycle);
    assert_int_equal(4, engine->pos_cycle);
    CycleCheck checks[] = {
        { .slot = 21, .mt = 1100 },
        { .slot = 27, .mt = 1430 },
        { .slot = 33, .mt = 1760 },
        { .slot = 43, .mt = 2114 },
        { .slot = 99, .mt = 2450 },
        { .slot = 156, .mt = 2792 },
        { .slot = 212, .mt = 3128 },
        { .slot = 2, .mt = 55 },
        { .slot = 8, .mt = 385 },
        { .slot = 14, .mt = 715 },
        { .slot = 21, .mt = 1100 },
    };
    for (size_t step = 0; step < ARRAY_SIZE(checks); step++) {
        /* Check the position. */
        assert_int_equal(checks[step].slot, engine->pos_slot);
        assert_int_equal(checks[step].mt, engine->pos_mt);

        /* Calc the budget. */
        uint32_t budget = engine->step_budget_ut;
        size_t   slot = engine->pos_slot;
        assert_int_equal(0, calculate_budget(engine, SIM_STEP_SIZE));
        assert_int_equal(budget + 20000, engine->step_budget_ut);
        assert_int_equal(
            (budget + 20000) / engine->macro2micro, engine->step_budget_mt);

        /* Consume the budget. */
        for (; consume_slot(engine) == 0;) {
        }
    }
    assert_int_equal(5, engine->pos_cycle);
}


typedef struct {
    struct {
        uint32_t mt;
        uint32_t cycle;
        union {
            uint32_t node_id;
            struct {
                uint16_t ecu_id;
                uint16_t cc_id;
            } node;
        };
    } condition;
    struct {
        /* Frame Config Entry - TX.*/
        uint16_t                     slot_id;
        int8_t                       base;
        int8_t                       repeat;
        uint64_t                     node_id;
        NCodecPduFlexrayLpduStatus   status;
        NCodecPduFlexrayTransmitMode tx_mode;
    } lpdu_tx;
    struct {
        /* Frame Config Entry - RX.*/
        uint16_t                   slot_id;
        int8_t                     base;
        int8_t                     repeat;
        uint64_t                   node_id;
        NCodecPduFlexrayLpduStatus status;
    } lpdu_rx;
    struct {
        bool                       tx; /* TX LPDU in the inform list. */
        bool                       rx; /* RX LPDU in the inform list. */
        NCodecPduFlexrayLpduStatus tx_status;
        NCodecPduFlexrayLpduStatus rx_status;
    } expect;
} TxRxCheck;

#define S_TX     NCodecPduFlexrayLpduStatusTransmitted
#define S_NOT_TX NCodecPduFlexrayLpduStatusNotTransmitted
#define S_RX     NCodecPduFlexrayLpduStatusReceived
#define S_NOT_RX NCodecPduFlexrayLpduStatusNotReceived
static TxRxCheck checks[] = {
    {
        /* 0 TX 0 1 / RX 0 1 : Cycle 0 -  xfer */
        .condition = { .mt = 0, .cycle = 0, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 1 TX 0 1 / RX 0 1 : Cycle 3 -  xfer */
        .condition = { .mt = 0, .cycle = 3, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 2 TX 0 4 / RX 0 4 : Cycle 2 - no xfer */
        .condition = { .mt = 0, .cycle = 2, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 3 TX 0 4 / RX 0 4 : Cycle 4 - xfer */
        .condition = { .mt = 0, .cycle = 4, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 4 TX 3 4 / RX 3 4 : Cycle 4 -  no xfer */
        .condition = { .mt = 0, .cycle = 4, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 5 TX 3 4 / RX 3 4 : Cycle 7 -  xfer */
        .condition = { .mt = 0, .cycle = 7, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 6 TX 0 4 / RX 0 4 : Cycle 4 - status no xfer */
        .condition = { .mt = 0, .cycle = 4, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 7 TX 3 4 / RX 3 4 : Cycle 11 -  tx diff node, rx xfer only */
        .condition = { .mt = 0, .cycle = 11, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 4,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 8 TX 3 4 / RX 3 4 : Cycle 11 -  rx diff node, tx xfer only */
        .condition = { .mt = 0, .cycle = 11, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 4,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 9 TX 3 4 / RX 3 4 : Cycle 11 -  tx/rx diff node, no xfer */
        .condition = { .mt = 0, .cycle = 11, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 3,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 3,
            .repeat = 4,
            .node_id = 4,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 10 TX 0 4 / RX 0 4 : Cycle 4 - boundary static, no xfer */
        .condition = { .mt = (55 * (38 - 3)), .cycle = 5, .node_id = 1 },
        .lpdu_tx = { .slot_id = 38,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 38,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 11 TX 0 4 / RX 0 4 : Cycle 4 - boundary static, xfer */
        .condition = { .mt = (55 * (38 - 3)), .cycle = 4, .node_id = 1 },
        .lpdu_tx = { .slot_id = 38,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 38,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 12 TX - - / RX - - : boundary dynamic, xfer */
        .condition = { .mt = (55 * (38 - 3)), .cycle = 7, .node_id = 1 },
        .lpdu_tx = { .slot_id = 39,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 39,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 13 TX - - / RX - - : limit dynamic, xfer */
        .condition = { .mt = ((55 * 38) + (6 * 211 - (6 * 3))),
            .cycle = 9,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 249,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 249,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 14 TX - - / RX - - : dynamic, tx diff node, rx xfer only */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 42,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 0,
            .repeat = 1,
            .node_id = 4,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 15 TX - - / RX - - : dynamic, rx diff node, tx xfer only */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 42,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 0,
            .repeat = 1,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 0,
            .repeat = 1,
            .node_id = 5,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 16 TX - - / RX - - : dynamic, tx/rx diff node, no xfer  */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 42,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 0,
            .repeat = 1,
            .node_id = 6,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 0,
            .repeat = 1,
            .node_id = 7,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 17 TX 0 4 / RX 0 4 : Cycle 4 - TransmitModeContinuous at frame
           level */
        .condition = { .mt = 0, .cycle = 4, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX,
            .tx_mode = NCodecPduFlexrayTransmitModeContinuous },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_NOT_TX,
            .rx_status = S_RX },
    },
    {
        /* 18 TX 0 4 / RX 0 4 : Cycle 4 - TransmitModeContinuous at frame
           level, diff tx node */
        .condition = { .mt = 0, .cycle = 4, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 4,
            .status = S_NOT_TX,
            .tx_mode = NCodecPduFlexrayTransmitModeContinuous },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = true,
            .tx_status = S_NOT_TX,
            .rx_status = S_RX },
    },
    {
        /* 19 TX 0 4 / RX 0 4 : Cycle 2 - no xfer */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 2,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 20 TX 0 4 / RX 0 4 : Cycle 4 - xfer */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 4,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 21 TX 3 4 / RX 3 4 : Cycle 4 -  no xfer */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 4,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 22 TX 3 4 / RX 3 4 : Cycle 7 -  xfer */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 7,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 23 TX 0 4 / RX 0 4 : Cycle 4 - status no xfer */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 4,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 0,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 24 TX 3 4 / RX 3 4 : Cycle 11 -  tx diff node, rx xfer only */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 11,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 4,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = true,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 25 TX 3 4 / RX 3 4 : Cycle 11 -  rx diff node, tx xfer only */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 11,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 4,
            .status = S_NOT_RX },
        .expect = { .tx = true,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 26 TX 3 4 / RX 3 4 : Cycle 11 -  tx/rx diff node, no xfer */
        .condition = { .mt = ((55 * 38) + (6 * (42 - 38 - 3))),
            .cycle = 11,
            .node_id = 1 },
        .lpdu_tx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 3,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 42,
            .base = 3,
            .repeat = 4,
            .node_id = 4,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
    {
        /* 27 TX 0 4 / RX 0 4 : Cycle 4 - repeat 0, no xfer */
        .condition = { .mt = 0, .cycle = 4, .node_id = 1 },
        .lpdu_tx = { .slot_id = 2,
            .base = 0,
            .repeat = 0,
            .node_id = 1,
            .status = S_NOT_TX },
        .lpdu_rx = { .slot_id = 2,
            .base = 0,
            .repeat = 0,
            .node_id = 1,
            .status = S_NOT_RX },
        .expect = { .tx = false,
            .rx = false,
            .tx_status = S_TX,
            .rx_status = S_RX },
    },
};
void test_flexray__engine_txrx__frames(void** state)
{
    Mock* mock = *state;

    /* */
    NCodecPduFlexrayConfig     config_0 = cc_config; /* TX */
    NCodecPduFlexrayConfig     config_1 = cc_config; /* RX */
    NCodecPduFlexrayLpduConfig frame_table_0[] = {
        { .payload_length = 64 }, /* TX */
    };
    NCodecPduFlexrayLpduConfig frame_table_1[] = {
        { .payload_length = 64 }, /* RX */
    };
    config_0.frame_config.table = frame_table_0;
    config_0.frame_config.count = ARRAY_SIZE(frame_table_0);
    config_1.frame_config.table = frame_table_1;
    config_1.frame_config.count = ARRAY_SIZE(frame_table_1);

    // __log_level__ = LOG_DEBUG;
    for (size_t step = 0; step < ARRAY_SIZE(checks); step++) {
        log_info("STEP %u", step);

        /* Configure and position. */
        config_0.node_ident.node_id = checks[step].lpdu_tx.node_id;
        config_1.node_ident.node_id = checks[step].lpdu_rx.node_id;
        FlexrayEngine* engine = &mock->engine;
        *engine = (FlexrayEngine){ .node_ident.node_id =
                                       checks[step].condition.node_id };

        /* TX Frame. */
        // frame_table[0].node_ident.node_id =  checks[step].lpdu_tx.node_id;
        frame_table_0[0].direction = NCodecPduFlexrayDirectionTx;
        frame_table_0[0].slot_id = checks[step].lpdu_tx.slot_id;
        frame_table_0[0].base_cycle = checks[step].lpdu_tx.base;
        frame_table_0[0].cycle_repetition = checks[step].lpdu_tx.repeat;
        frame_table_0[0].transmit_mode = checks[step].lpdu_tx.tx_mode;
        frame_table_0[0].status = checks[step].lpdu_tx.status;
        /* RX Frame. */
        // frame_table[0].node_ident.node_id =  checks[step].lpdu_rx.node_id;
        frame_table_1[0].direction = NCodecPduFlexrayDirectionRx;
        frame_table_1[0].slot_id = checks[step].lpdu_rx.slot_id;
        frame_table_1[0].base_cycle = checks[step].lpdu_rx.base;
        frame_table_1[0].cycle_repetition = checks[step].lpdu_rx.repeat;
        frame_table_1[0].status = checks[step].lpdu_rx.status;
        NCodecPdu pdu = {
            .transport_type = NCodecPduTransportTypeFlexray,
            .transport.flexray.metadata_type =
                NCodecPduFlexrayMetadataTypeConfig,
        };
        pdu.transport.flexray.metadata.config = config_0,
        assert_int_equal(0, process_config(&pdu, engine));
        pdu.transport.flexray.metadata.config = config_1,
        assert_int_equal(0, process_config(&pdu, engine));
        assert_int_equal(0, shift_cycle(engine, checks[step].condition.mt,
                                checks[step].condition.cycle, true));

/* Set the TX Payload. */
#define PAYLOAD "hello world"
        assert_int_equal(
            0, set_lpdu(engine, config_0.node_ident.node_id,
                   frame_table_0[0].slot_id, 0, frame_table_0[0].status,
                   (uint8_t*)PAYLOAD, strlen(PAYLOAD)));

        /* Progress one sim step. */
        assert_int_equal(0, calculate_budget(engine, SIM_STEP_SIZE));
        for (; consume_slot(engine) == 0;) {
        }

        /* Evaluate. */
        size_t txrx_len = checks[step].expect.tx;
        if (checks[step].expect.rx) txrx_len += 1;
        assert_int_equal(txrx_len, vector_len(&engine->txrx_list));
        if (checks[step].expect.tx) {
            FlexrayLpdu* tx_lpdu = NULL;
            vector_at(&engine->txrx_list, 0, &tx_lpdu);
            assert_int_equal(
                checks[step].expect.tx_status, tx_lpdu->lpdu_config.status);
        }
        if (checks[step].expect.rx) {
            FlexrayLpdu* rx_lpdu = NULL;
            vector_at(&engine->txrx_list, txrx_len - 1, &rx_lpdu);
            assert_int_equal(
                checks[step].expect.rx_status, rx_lpdu->lpdu_config.status);
            assert_memory_equal(PAYLOAD, rx_lpdu->payload, strlen(PAYLOAD));
        }
        release_config(engine);
    }
}

int run_pdu_flexray_engine_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_flexray__communication_parameters, s, t),
        cmocka_unit_test_setup_teardown(
            test_flexray__engine_cycle__empty_frame_config, s, t),
        cmocka_unit_test_setup_teardown(
            test_flexray__engine_cycle__with_frame_config, s, t),
        cmocka_unit_test_setup_teardown(test_flexray__engine_cycle__wrap, s, t),
        cmocka_unit_test_setup_teardown(
            test_flexray__engine_cycle__shift, s, t),
        cmocka_unit_test_setup_teardown(
            test_flexray__engine_txrx__frames, s, t),
    };

    return cmocka_run_group_tests_name("PDU FLEXRAY ENGINE", tests, NULL, NULL);
}
