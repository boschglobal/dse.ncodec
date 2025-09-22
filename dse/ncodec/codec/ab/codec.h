// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_CODEC_AB_CODEC_H_
#define DSE_NCODEC_CODEC_AB_CODEC_H_

#include <stdbool.h>
#include <stdint.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/schema/abs/stream/flatbuffers_common_reader.h>
#include <dse/ncodec/schema/abs/stream/flatbuffers_common_builder.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>


typedef struct ABCodecInstance ABCodecInstance;
typedef struct ABCodecBusModel ABCodecBusModel;

// typedef struct {} BUSMODEL;
typedef bool (*NCodecBusModelConsume)(ABCodecBusModel* bm, NCodecPdu* pdu);
typedef void (*NCodecBusModelProgress)(ABCodecBusModel* bm);
typedef void (*NCodecBusModelClose)(ABCodecBusModel* bm);

typedef void BUSMODEL;

typedef struct ABCodecBusModel {
    BUSMODEL*        model;
    ABCodecInstance* nc; /* Stream (via NC). */
    struct {
        NCodecBusModelConsume  consume;
        NCodecBusModelProgress progress;
        NCodecBusModelClose    close;
    } vtable;
} ABCodecBusModel;


// Stream(buffer) -> Message -> Vector -> PDU
typedef struct ABCodecReader {
    /* Reader stage. */
    struct {
        bool ncodec_consumed;
        bool model_produced;
        bool model_consumed;
    } stage;
    /* Parsing state. */
    struct {
        /* Stream (via NC) maintains its own state. */
        ABCodecInstance* nc;
        /* Message parsing state. */
        uint8_t*         msg_ptr;
        size_t           msg_len;
        /* Vector parsing state. */
        const uint32_t*  vector;
        size_t           vector_idx;
        size_t           vector_len;
    } state;
    /* Bus model. */
    ABCodecBusModel bus_model;
} ABCodecReader;


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
    char*   cc_id_str;         /* Communication Controller. */
    char*   model;             /* Bus Model. */
    char*   pwr;               /* Initial power state (on|off or not set). */
    char*   vcn_count_str;     /* Count of VCNs. */
    char*   poc_state_cha_str; /* Initial POC state (Channel A). */
    char*   poc_state_chb_str; /* Initial POC state (Channel B). */
    /* Internal representation. */
    uint8_t bus_id;
    uint8_t node_id;
    uint8_t interface_id;
    uint8_t swc_id;
    uint8_t ecu_id;
    uint8_t cc_id;
    uint8_t vcn_count;
    uint8_t poc_state_cha;
    uint8_t poc_state_chb;

    /* Flatbuffer resources. */
    flatcc_builder_t fbs_builder;
    bool             fbs_builder_initalized;
    bool             fbs_stream_initalized;

    /* Reader object. */
    ABCodecReader reader;

    /* Free list (free called on truncate). */
    Vector free_list; /* void* references */
} ABCodecInstance;


#endif  // DSE_NCODEC_CODEC_AB_CODEC_H_
