// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <dse/clib/collections/vector.h>
#include <dse/clib/util/yaml.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/pdunet/pdunet.h>
#include <dse/pdunet/network/network.h>


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BUFFER_LEN    4096
#define MIMETYPE                                                               \
    "application/x-automotive-bus;"                                            \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "ecu_id=5;loopback=1"


extern void* pdunet_schema_object_enumerator(PdunetSchemaObject* object,
    const char* path, uint32_t* index, PdunetSchemaObjectGenerator generator);

typedef struct BenchArgs {
    double      step_size;
    double      end_time;
    const char* network;
} BenchArgs;

NCODEC* ncodec_open(const char* mime_type, NSTREAM* stream);

static double monotonic_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);
}

static void* object_match_handler(void* object)
{
    return object;  // YamlNode
}

int run_pdunet_benchmark(const BenchArgs* args)
{
    int          rc = 0;
    YamlDocList* dl = NULL;
    void*        doc = NULL;
    NSTREAM*     stream = NULL;
    NCODEC*      nc = NULL;
    PduNetwork*  net = NULL;

    printf("\n");
    printf("Parse Network\n");
    printf("-------------\n");

    /* Load Network YAML. */
    dl = dse_yaml_load_file(NULL, args->network, NULL);
    if (dl == NULL) {
        fprintf(stderr, "failed to load network YAML: %s\n", args->network);
        return 1;
    }

    doc = dse_yaml_find_doc_in_doclist(dl, "Network", NULL, NULL, 0);
    if (doc == NULL) {
        fprintf(
            stderr, "failed to find Network document in: %s\n", args->network);
        return 2;
    }

    /* Create the Signal Vectors. */
    Vector             signal_names = vector_make(sizeof(const char*), 0, NULL);
    uint32_t           pdu_index = 0;
    PdunetSchemaObject object = {
        .doc = doc,
    };
    do {
        YamlNode* n = pdunet_schema_object_enumerator(
            &object, "spec/pdus", &pdu_index, object_match_handler);
        if (n == NULL) break;
        YamlNode* sigs = dse_yaml_find_node(n, "signals");
        if (sigs) {
            for (uint32_t i = 0; i < hashlist_length(&sigs->sequence); i++) {
                YamlNode*   sig = hashlist_at(&sigs->sequence, i);
                const char* sig_name = NULL;
                dse_yaml_get_string(sig, "signal", &sig_name);
                if (sig_name == NULL) continue;
                vector_push(&signal_names, &sig_name);
            }
        }
    } while (1);
    Vector signal_values =
        vector_make(sizeof(const double), vector_len(&signal_names), NULL);

    /* Create NCodec. */
    stream = ncodec_buffer_stream_create(BUFFER_LEN);
    nc = ncodec_open(MIMETYPE, stream);
    if (nc == NULL) {
        fprintf(stderr, "failed to create ncodec\n");
        return 3;
    }

    /* Create and map PDUNet using benchmark step size. */
    net = pdunet_create(nc, doc, args->step_size, NULL, NULL);
    if (net == NULL) {
        fprintf(stderr, "failed to create pdunet\n");
        rc = 4;
        goto cleanup;
    }

    const char** names = signal_names.items;
    double*      values = signal_values.items;
    rc = pdunet_map_signals(
        net, "PDUNet", vector_len(&signal_names), names, values);
    if (rc != 0) {
        fprintf(stderr, "failed to map signals: rc=%d\n", rc);
        rc = 5;
        goto cleanup;
    }

    /* Benchmark simulation loop. */
    printf("\n");
    printf("Run Benchmark\n");
    printf("-------------\n");

    uint64_t     steps = 0;
    double       runtime_model_time = 0.0;
    double       step_time_correction = 0.0;
    const double stop_time = args->end_time;
    const double step_epsilon = args->step_size * 1e-9;

    const double wall_start = monotonic_seconds();

    double model_current_time;
    double model_stop_time;
    do {
        /* Determine times. */
        model_current_time = runtime_model_time;
        model_stop_time = stop_time;

        if (model_current_time >= model_stop_time) {
            break;
        }

        /*
         * Use the simulation step size.
         * Increment via Kahan summation.
         */
        double y = args->step_size - step_time_correction;
        double t = model_current_time + y;
        double next_step_time_correction = (t - model_current_time) - y;
        model_stop_time = t;

        /* Model stop time past benchmark stop time. */
        if (model_stop_time > stop_time + step_epsilon) {
            break;
        }

        /* Step accepted, now commit the correction. */
        step_time_correction = next_step_time_correction;

        /*
         * Keep signal updates deterministic.
         * This can later be replaced with benchmark-specific traffic patterns.
         */
        values[0] = 50.0 + model_current_time;
        values[1] = 1.2;

        pdunet_tx(net, NULL, NULL, NULL, model_current_time);
        pdunet_rx(net, NULL, NULL, NULL);

        runtime_model_time = model_stop_time;
        steps++;
    } while (model_stop_time + step_epsilon < stop_time);

    const double wall_end = monotonic_seconds();
    const double wall_elapsed = wall_end - wall_start;
    const double simulated_elapsed = runtime_model_time;
    const double real_time_factor =
        wall_elapsed > 0.0 ? simulated_elapsed / wall_elapsed : 0.0;

    printf("PDUNet benchmark result\n");
    printf("  network:           %s\n", args->network);
    printf("  signals:           %zu\n", vector_len(&signal_names));
    printf("  step_size:         %.9f s\n", args->step_size);
    printf("  requested_end:     %.9f s\n", args->end_time);
    printf("  simulated_time:    %.9f s\n", simulated_elapsed);
    printf("  steps:             %zu\n", steps);
    printf("  wall_time:         %.9f s\n", wall_elapsed);
    printf("  real_time_factor:  %.3f x\n", real_time_factor);

    rc = 0;

cleanup:
    if (net != NULL) {
        pdunet_destroy(net);
    }
    if (nc != NULL) {
        ncodec_close(nc);
    }

    /*
     * If the YAML utility exposes a doc-list destroy/free function elsewhere
     * in the repo, call it here as well. The existing example does not yet do
     * that cleanup.
     */

    return rc;
}
