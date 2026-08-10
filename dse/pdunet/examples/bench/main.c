// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dse/ncodec/codec.h>

typedef struct BenchArgs {
    double      step_size;
    double      end_time;
    const char* network;
} BenchArgs;

int run_pdunet_benchmark(const BenchArgs* args);

NCODEC* ncodec_open(const char* mime_type, NSTREAM* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc) {
        NCodecInstance* _nc = (NCodecInstance*)nc;
        _nc->stream = stream;
    }
    return nc;
}

static void usage(const char* prog)
{
    fprintf(stderr,
        "usage: %s --step-size=<seconds> --end-time=<seconds> "
        "--network=<network.yaml>\n"
        "\n"
        "example:\n"
        "  %s --step-size=0.0005 --end-time=10 --network=network.yaml\n",
        prog, prog);
}

static int parse_double_arg(const char* value, double* out)
{
    char* end = NULL;
    errno = 0;

    double v = strtod(value, &end);
    if (errno != 0 || end == value || *end != '\0') {
        return -1;
    }

    *out = v;
    return 0;
}

static int parse_args(int argc, char** argv, BenchArgs* args)
{
    args->step_size = 0.0005;
    args->end_time = 1.0;
    args->network = NULL;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--step-size=", 12) == 0) {
            if (parse_double_arg(argv[i] + 12, &args->step_size) != 0) {
                fprintf(
                    stderr, "invalid --step-size value: %s\n", argv[i] + 12);
                return -1;
            }
        } else if (strncmp(argv[i], "--end-time=", 11) == 0) {
            if (parse_double_arg(argv[i] + 11, &args->end_time) != 0) {
                fprintf(stderr, "invalid --end-time value: %s\n", argv[i] + 11);
                return -1;
            }
        } else if (strncmp(argv[i], "--network=", 10) == 0) {
            args->network = argv[i] + 10;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            exit(0);
        } else {
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
            return -1;
        }
    }

    if (args->step_size <= 0.0) {
        fprintf(stderr, "--step-size must be > 0\n");
        return -1;
    }

    if (args->end_time <= 0.0) {
        fprintf(stderr, "--end-time must be > 0\n");
        return -1;
    }

    if (args->network == NULL || args->network[0] == '\0') {
        fprintf(stderr, "--network is required\n");
        return -1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    BenchArgs args;

    if (parse_args(argc, argv, &args) != 0) {
        usage(argv[0]);
        return 1;
    }

    return run_pdunet_benchmark(&args);
}
