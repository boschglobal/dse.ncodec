// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef TESTS_CMOCKA_PDUNET_MOCK_H_
#define TESTS_CMOCKA_PDUNET_MOCK_H_

#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dse/ncodec/codec/ab/codec.h>
#undef log_trace
#undef log_debug
#undef log_info
#undef log_simbus
#undef log_notice
#undef log_error
#undef log_fatal
#include <dse/testing.h>
#include <dse/clib/util/yaml.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/pdunet/network/network.h>
#include <dse/pdunet/pdunet.h>


typedef struct PdunetMock {
    PduNetwork*     net;
    NCODEC*         ncodec;
    ABCodecBusModel bm_save;
    lua_State*      L;
    YamlDocList*    dl;
    double          step_size;
    void*           doc;
    struct {
        const char*  name;
        size_t       count;
        const char** signal;
        double*      scalar;
    } sv;
} PdunetMock;

extern PdunetMock* __create_mock(const char* y);

#endif  // TESTS_CMOCKA_PDUNET_MOCK_H_
