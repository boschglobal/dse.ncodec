// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_CODEC_AB_CODEC_H_
#define DSE_NCODEC_CODEC_AB_CODEC_H_

#include <stdbool.h>
#include <stdint.h>
#include <dse/ncodec/schema/abs/stream/flatbuffers_common_reader.h>
#include <dse/ncodec/schema/abs/stream/flatbuffers_common_builder.h>
#include <dse/ncodec/codec.h>


/* Declare an extension to the NCodecInstance type. */
typedef struct ABCodecInstance {
    NCodecInstance c;

    /* Codec selectors: from MIMEtype. */
    char* interface;
    char* type;
    char* bus;
    char* schema;

    /* Parameters: from MIMEtype or calls to ncodec_config(). */
    /* String representation (supporting ncodec_stat()). */
    char*   bus_id_str;
    char*   node_id_str;
    char*   interface_id_str;
    char*   swc_id_str;
    char*   ecu_id_str;
    char*   cc_id_str; /* Communication Controller. */
    /* Internal representation. */
    uint8_t bus_id;
    uint8_t node_id;
    uint8_t interface_id;
    uint8_t swc_id;
    uint8_t ecu_id;
    uint8_t cc_id;

    /* Flatbuffer resources. */
    flatcc_builder_t fbs_builder;
    bool             fbs_builder_initalized;
    bool             fbs_stream_initalized;

    /* Message parsing state. */
    uint8_t* msg_ptr;
    size_t   msg_len;

    /* Frame parsing state. */
    const flatbuffers_uoffset_t* vector;
    size_t                       vector_idx;
    size_t                       vector_len;
} ABCodecInstance;


#endif  // DSE_NCODEC_CODEC_AB_CODEC_H_
