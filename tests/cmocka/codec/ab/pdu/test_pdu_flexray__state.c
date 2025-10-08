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

#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=1;cc_id=0;swc_id=1;vcn=2;pon=off"


extern NCodecConfigItem codec_stat(NCODEC* nc, int* index);
extern NCODEC*          ncodec_create(const char* mime_type);
extern int32_t stream_read(NCODEC* nc, uint8_t** data, size_t* len, int pos_op);


typedef struct Mock {
    NCODEC* nc;

    /* Object for NodeState tests.*/
    FlexrayNodeState node_state;

    /* */
    FlexrayState flexray_state;
} Mock;


static int test_setup(void** state)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);

    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(MIMETYPE, stream);
    assert_non_null(mock->nc);
    mock->node_state = (FlexrayNodeState){ 0 };

    *state = mock;
    return 0;
}


static int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock && mock->nc) ncodec_close((void*)mock->nc);
    release_state(&mock->flexray_state);
    if (mock) free(mock);

    return 0;
}


typedef struct {
    NCodecPduFlexrayPocCommand       command;
    NCodecPduFlexrayPocState         poc_state;
    NCodecPduFlexrayTransceiverState tcvr_state;
} StateTransition;

typedef struct {
    const char*                      name;
    NCodecPduFlexrayPocState         initial_poc_state;
    NCodecPduFlexrayTransceiverState initial_tcvr_state;
    size_t                           length;
    StateTransition                  transition[20];
} StateSequence;

void test_flexray__node_state_changes(void** _state)
{
    Mock*             mock = *_state;
    FlexrayNodeState* state = &mock->node_state;
    StateSequence checks[] = {
        {
            .name = "DefautConfig --> NormalActive",
            .initial_poc_state = NCodecPduFlexrayPocStateDefaultConfig,
            .initial_tcvr_state = NCodecPduFlexrayTransceiverStateNoSignal,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandConfig,
                    .poc_state = NCodecPduFlexrayPocStateConfig,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateNoSignal,
                },
                {
                    .command = NCodecPduFlexrayCommandReady,
                    .poc_state = NCodecPduFlexrayPocStateReady,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateFrameError,
                },
                {
                    .command = NCodecPduFlexrayCommandRun,
                    .poc_state = NCodecPduFlexrayPocStateNormalActive,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
                },
            },
        },
        {
            .name = "DefautConfig --> Ready --> Config",
            .initial_poc_state = NCodecPduFlexrayPocStateDefaultConfig,
            .initial_tcvr_state = NCodecPduFlexrayTransceiverStateNoSignal,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandConfig,
                    .poc_state = NCodecPduFlexrayPocStateConfig,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateNoSignal,
                },
                {
                    .command = NCodecPduFlexrayCommandReady,
                    .poc_state = NCodecPduFlexrayPocStateReady,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateFrameError,
                },
                {
                    .command = NCodecPduFlexrayCommandConfig,
                    .poc_state = NCodecPduFlexrayPocStateConfig,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateNoSignal,
                },
            },
        },
        {
            .name = "Wakeup --> NormalActive",
            .initial_poc_state = NCodecPduFlexrayPocStateWakeup,
            .initial_tcvr_state = NCodecPduFlexrayTransceiverStateWUP,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandNop,
                    .poc_state = NCodecPduFlexrayPocStateNormalActive,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
                },
            },
        },
        {
            .name = "Startup --> NormalActive",
            .initial_poc_state = NCodecPduFlexrayPocStateStartup,
            .initial_tcvr_state = NCodecPduFlexrayTransceiverStateFrameError,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandNop,
                    .poc_state = NCodecPduFlexrayPocStateNormalActive,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
                },
            },
        },
        {
            .name = "NormalPassive --> NormalActive",
            .initial_poc_state = NCodecPduFlexrayPocStateNormalPassive,
            .initial_tcvr_state = NCodecPduFlexrayTransceiverStateFrameError,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandNop,
                    .poc_state = NCodecPduFlexrayPocStateNormalActive,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
                },
            },
        },
        {
            .name = "NormalActive --> DefaultConfig",
            .initial_poc_state = NCodecPduFlexrayPocStateNormalActive,
            .initial_tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandHalt,
                    .poc_state = NCodecPduFlexrayPocStateHalt,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateNoConnection,
                },
                {
                    .command = NCodecPduFlexrayCommandConfig,
                    .poc_state = NCodecPduFlexrayPocStateDefaultConfig,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateNoSignal,
                },
            },
        },
        {
            .name = "NormalActive --> Freeze",
            .initial_poc_state = NCodecPduFlexrayPocStateNormalActive,
            .initial_tcvr_state = NCodecPduFlexrayTransceiverStateFrameSync,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandFreeze,
                    .poc_state = NCodecPduFlexrayPocStateFreeze,
                    .tcvr_state = NCodecPduFlexrayTransceiverStateNoConnection,
                },
            },
        },
    };

    for (size_t check = 0; check < ARRAY_SIZE(checks); check++) {
        log_info("Check %u: %s", check, checks[check].name);
        *state =
            (FlexrayNodeState){ .poc_state = checks[check].initial_poc_state,
                .tcvr_state = checks[check].initial_tcvr_state };

        for (size_t i = 0;
             checks[check].transition[i].command != NCodecPduFlexrayCommandNone;
             i++) {
            assert_int_equal(0, process_poc_command(state,
                                    checks[check].transition[i].command));
            assert_int_equal(
                state->poc_state, checks[check].transition[i].poc_state);
            assert_int_equal(
                state->tcvr_state, checks[check].transition[i].tcvr_state);
        }
    }
}


typedef struct {
    const char*                    name;
    NCodecPduFlexrayNodeIdentifier vcs_n1;
    NCodecPduFlexrayNodeIdentifier vcs_n2;
    NCodecPduFlexrayNodeIdentifier node;
    NCodecPduFlexrayNodeIdentifier node2;
    size_t                         vcs_node_count;
    struct {
        NCodecPduFlexrayTransceiverState initial_bus_condition;
        NCodecPduFlexrayTransceiverState pre_power;
        NCodecPduFlexrayTransceiverState post_power;
        NCodecPduFlexrayTransceiverState pre_normal_active;
        NCodecPduFlexrayTransceiverState post_normal_active;
        NCodecPduFlexrayTransceiverState post_normal_active_bus_condition;
        NCodecPduFlexrayPocState         post_normal_active_poc_state;
    } condition;
} BusConditionTestCase;

void test_flexray__bus_condition(void** _state)
{
    //__log_level__ = LOG_DEBUG;
    Mock*         mock = *_state;
    FlexrayState* state = &mock->flexray_state;
    BusConditionTestCase checks[] = {
        {
            .name = "Zero VCS Nodes",
            .vcs_n1 = {.node = {0}},
            .vcs_n2 = {.node = {0}},
            .node = {.node = {.ecu_id = 1}},
            .vcs_node_count = 0,
            .condition = {
                .initial_bus_condition = NCodecPduFlexrayTransceiverStateNoSignal,
                .pre_power = NCodecPduFlexrayTransceiverStateNoPower,
                .post_power = NCodecPduFlexrayTransceiverStateNoConnection,
                .pre_normal_active = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active = NCodecPduFlexrayTransceiverStateFrameError,
                .post_normal_active_bus_condition = NCodecPduFlexrayTransceiverStateFrameError,
                .post_normal_active_poc_state = NCodecPduFlexrayPocStateNormalPassive,
            }
        },
        {
            .name = "One VCS Nodes",
            .vcs_n1 = {.node = {.ecu_id = 1, .swc_id = 1}},
            .vcs_n2 = {.node = {0}},
            .node = {.node = {.ecu_id = 1}},
            .vcs_node_count = 1,
            .condition = {
                .initial_bus_condition = NCodecPduFlexrayTransceiverStateFrameError,
                .pre_power = NCodecPduFlexrayTransceiverStateNoPower,
                .post_power = NCodecPduFlexrayTransceiverStateNoConnection,
                .pre_normal_active = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active_bus_condition = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active_poc_state = NCodecPduFlexrayPocStateNormalActive,
            }
        },
        {
            .name = "Two VCS Nodes",
            .vcs_n1 = {.node = {.ecu_id = 1, .swc_id = 1}},
            .vcs_n2 = {.node = {.ecu_id = 1, .swc_id = 2}},
            .node = {.node = {.ecu_id = 1}},
            .vcs_node_count = 2,
            .condition = {
                .initial_bus_condition = NCodecPduFlexrayTransceiverStateFrameSync,
                .pre_power = NCodecPduFlexrayTransceiverStateNoPower,
                .post_power = NCodecPduFlexrayTransceiverStateNoConnection,
                .pre_normal_active = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active_bus_condition = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active_poc_state = NCodecPduFlexrayPocStateNormalActive,
            }
        },
        {
            .name = "Multi Node with 2 VCS Nodes",
            .vcs_n1 = {.node = {.ecu_id = 3, .swc_id = 1}},
            .vcs_n2 = {.node = {.ecu_id = 1, .swc_id = 2}},
            .node = {.node = {.ecu_id = 4}},
            .node2 = {.node = {.ecu_id = 2}},
            .vcs_node_count = 2,
            .condition = {
                .initial_bus_condition = NCodecPduFlexrayTransceiverStateFrameSync,
                .pre_power = NCodecPduFlexrayTransceiverStateNoPower,
                .post_power = NCodecPduFlexrayTransceiverStateNoConnection,
                .pre_normal_active = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active_bus_condition = NCodecPduFlexrayTransceiverStateFrameSync,
                .post_normal_active_poc_state = NCodecPduFlexrayPocStateNormalActive,
            }
        },
    };
    for (size_t i = 0; i < ARRAY_SIZE(checks); i++) {
        log_info("Check %u: %s", i, checks[i].name);
        *state = (FlexrayState){ 0 };
        if (checks[i].vcs_n1.node_id)
            register_vcn_node_state(state, checks[i].vcs_n1);
        if (checks[i].vcs_n2.node_id)
            register_vcn_node_state(state, checks[i].vcs_n2);
        register_node_state(state, checks[i].node, false, true);
        if (checks[i].node2.node_id)
            register_node_state(state, checks[i].node2, false, true);
        assert_int_equal(
            checks[i].vcs_node_count, vector_len(&state->vcs_node));
        if (checks[i].node2.node_id) {
            assert_int_equal(2, vector_len(&state->node_state));
        } else {
            assert_int_equal(1, vector_len(&state->node_state));
        }

        /* Add a node and push to Config state. */
        calculate_bus_condition(state);
        assert_int_equal(
            checks[i].condition.initial_bus_condition, state->bus_condition);

        /* Power-On the transceiver. */
        assert_int_equal(checks[i].condition.pre_power,
            get_node_state(state, checks[i].node).tcvr_state);
        set_node_power(state, checks[i].node, true);
        assert_int_equal(checks[i].condition.post_power,
            get_node_state(state, checks[i].node).tcvr_state);
        if (checks[i].node2.node_id) {
            assert_int_equal(checks[i].condition.pre_power,
                get_node_state(state, checks[i].node2).tcvr_state);
            set_node_power(state, checks[i].node2, true);
            assert_int_equal(checks[i].condition.post_power,
                get_node_state(state, checks[i].node2).tcvr_state);
        }

        /* Push Node to Normal Active. */
        push_node_state(state, checks[i].node, NCodecPduFlexrayCommandConfig);
        push_node_state(state, checks[i].node, NCodecPduFlexrayCommandReady);
        push_node_state(state, checks[i].node, NCodecPduFlexrayCommandRun);
        if (checks[i].node2.node_id) {
            push_node_state(
                state, checks[i].node2, NCodecPduFlexrayCommandConfig);
            push_node_state(
                state, checks[i].node2, NCodecPduFlexrayCommandReady);
            push_node_state(state, checks[i].node2, NCodecPduFlexrayCommandRun);
        }

        assert_int_equal(checks[i].condition.pre_normal_active,
            get_node_state(state, checks[i].node).tcvr_state);
        if (checks[i].node2.node_id) {
            assert_int_equal(checks[i].condition.pre_normal_active,
                get_node_state(state, checks[i].node2).tcvr_state);
        }

        calculate_bus_condition(state);

        assert_int_equal(checks[i].condition.post_normal_active,
            get_node_state(state, checks[i].node).tcvr_state);
        assert_int_equal(checks[i].condition.post_normal_active_bus_condition,
            state->bus_condition);
        assert_int_equal(checks[i].condition.post_normal_active_poc_state,
            get_node_state(state, checks[i].node).poc_state);
        if (checks[i].node2.node_id) {
            assert_int_equal(checks[i].condition.post_normal_active,
                get_node_state(state, checks[i].node2).tcvr_state);
            assert_int_equal(
                checks[i].condition.post_normal_active_bus_condition,
                state->bus_condition);
            assert_int_equal(checks[i].condition.post_normal_active_poc_state,
                get_node_state(state, checks[i].node2).poc_state);
        }

        release_state(state);
    }
}


int run_pdu_flexray_state_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;
#define T cmocka_unit_test_setup_teardown

    const struct CMUnitTest tests[] = {
        T(test_flexray__node_state_changes, s, t),
        T(test_flexray__bus_condition, s, t),
    };

    return cmocka_run_group_tests_name("PDU  FLEXRAY STATE", tests, NULL, NULL);
}
