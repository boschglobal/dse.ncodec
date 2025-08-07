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
    "ecu_id=1;cc_id=0;swc_id=1;vcn=2"


extern NCodecConfigItem codec_stat(NCODEC* nc, int* index);
extern NCODEC*          ncodec_create(const char* mime_type);
extern int32_t stream_read(NCODEC* nc, uint8_t** data, size_t* len, int pos_op);


typedef struct Mock {
    NCODEC*       nc;
    FlexRayState state;
} Mock;


static int test_setup(void** state)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);

    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(MIMETYPE, stream);
    assert_non_null(mock->nc);
    mock->state = (FlexRayState){ 0 };

    *state = mock;
    return 0;
}


static int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock && mock->nc) ncodec_close((void*)mock->nc);
    if (mock) free(mock);

    return 0;
}


typedef struct {
    NCodecPduFlexrayPocCommand command;
    NCodecPduFlexrayPocState target;
} StateTransition;

typedef struct {
    const char* name;
    NCodecPduFlexrayPocState initial_state;
    size_t length;
    StateTransition transition[20];
} StateSequence;

void test_flexray__state_changes(void** _state)
{
    Mock*                       mock = *_state;
    FlexRayState* state = &mock->state;
    StateSequence checks[] = {
        {
            .name = "DefautConfig --> NormalActive",
            .initial_state = NCodecPduFlexrayPocStateDefaultConfig,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandConfig, 
                    .target = NCodecPduFlexrayPocStateConfig,
                },
                {
                    .command = NCodecPduFlexrayCommandReady, 
                    .target = NCodecPduFlexrayPocStateReady,
                },
                {
                    .command = NCodecPduFlexrayCommandRun, 
                    .target = NCodecPduFlexrayPocStateNormalActive,
                },
            },
        },
        {
            .name = "DefautConfig --> Ready --> Config",
            .initial_state = NCodecPduFlexrayPocStateDefaultConfig,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandConfig, 
                    .target = NCodecPduFlexrayPocStateConfig,
                },
                {
                    .command = NCodecPduFlexrayCommandReady, 
                    .target = NCodecPduFlexrayPocStateReady,
                },
                {
                    .command = NCodecPduFlexrayCommandConfig, 
                    .target = NCodecPduFlexrayPocStateConfig,
                },
            },
        },
        {
            .name = "Wakeup --> NormalActive",
            .initial_state = NCodecPduFlexrayPocStateWakeup,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandNop, 
                    .target = NCodecPduFlexrayPocStateNormalActive,
                },
            },
        },
        {
            .name = "Startup --> NormalActive",
            .initial_state = NCodecPduFlexrayPocStateStartup,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandNop, 
                    .target = NCodecPduFlexrayPocStateNormalActive,
                },
            },
        },
        {
            .name = "NormalPassive --> NormalActive",
            .initial_state = NCodecPduFlexrayPocStateNormalPassive,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandNop, 
                    .target = NCodecPduFlexrayPocStateNormalActive,
                },
            },
        },
        {
            .name = "NormalActive --> DefaultConfig",
            .initial_state = NCodecPduFlexrayPocStateNormalActive,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandHalt, 
                    .target = NCodecPduFlexrayPocStateHalt,
                },
                {
                    .command = NCodecPduFlexrayCommandConfig, 
                    .target = NCodecPduFlexrayPocStateDefaultConfig,
                },
            },
        },
        {
            .name = "NormalActive --> Freeze",
            .initial_state = NCodecPduFlexrayPocStateNormalActive,
            .length = 4,
            .transition = {
                {
                    .command = NCodecPduFlexrayCommandFreeze, 
                    .target = NCodecPduFlexrayPocStateFreeze,
                },
            },
        },
    };

    for (size_t check = 0; check < ARRAY_SIZE(checks); check++) {
        log_info("Check %u: %s", check, checks[check].name);
        *state = (FlexRayState){ .poc_state = checks[check].initial_state };

        for (size_t i = 0; checks[check].transition[i].command != NCodecPduFlexrayCommandNone; i++) {
            assert_int_equal(0, process_poc_command(state, checks[check].transition[i].command));
            assert_int_equal(state->poc_state, checks[check].transition[i].target);
        }
    }
}


int run_pdu_flexray_state_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_flexray__state_changes, s, t),
    };

    return cmocka_run_group_tests_name("PDU  FLEXRAY STATE", tests, NULL, NULL);
}

