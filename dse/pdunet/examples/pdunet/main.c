// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <errno.h>
#include <dse/ncodec/codec.h>

int run_pdunet_example(const char* y);

NCODEC* ncodec_open(const char* mime_type, NSTREAM* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc) {
        NCodecInstance* _nc = (NCodecInstance*)nc;
        _nc->stream = stream;
    }
    return nc;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <network.yaml>\n", argv[0]);
        return 1;
    }
    return run_pdunet_example(argv[1]);
}
