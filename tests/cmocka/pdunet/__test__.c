// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dse/testing.h>
#include <dse/log.h>
#include <dse/clib/util/yaml.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/pdunet/network/network.h>
#include <dse/pdunet/pdunet.h>
#include "mock.h"  // NOLINT(build/include_subdir,build/include_order)


#define BUFFER_LEN 1024
#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "swc_id=4;ecu_id=5;loopback=1"


DseLog dlog = { .level = LOG_QUIET, dse_log2console };

NCODEC* ncodec_open(const char* mime_type, NSTREAM* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc) {
        NCodecInstance* _nc = (NCodecInstance*)nc;
        _nc->stream = stream;
    }
    return nc;
}

PdunetMock* __create_mock(const char* y)
{
    PdunetMock*  mock = calloc(1, sizeof(PdunetMock));
    NSTREAM*     stream = ncodec_buffer_stream_create(BUFFER_LEN);
    YamlDocList* dl = dse_yaml_load_file(&dlog, y, NULL);

    // dlog.level = LOG_DEBUG;

    *mock = (PdunetMock){
        .step_size = 0.0005,
        .dl = dl,
        .doc = dse_yaml_find_doc_in_doclist(dl, "Network", NULL, NULL, 0),
        .L = luaL_newstate(),
        .ncodec = (void*)ncodec_open(MIMETYPE, stream),
    };
    luaL_openlibs(mock->L);
    return mock;
}

extern int run_pdunet_network_tests(void);
extern int run_pdunet_sigmap_tests(void);
extern int run_pdunet_ncodec_tests(void);
extern int run_pdunet_schedule_tests(void);


int main()
{
    int rc = 0;
    rc |= run_pdunet_network_tests();
    rc |= run_pdunet_sigmap_tests();
    rc |= run_pdunet_ncodec_tests();
    rc |= run_pdunet_schedule_tests();
    return rc;
}
