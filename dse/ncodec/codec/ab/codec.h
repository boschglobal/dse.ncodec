// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_CODEC_AB_CODEC_H_
#define DSE_NCODEC_CODEC_AB_CODEC_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <dse/ncodec/schema/abs/stream/flatbuffers_common_reader.h>
#include <dse/ncodec/schema/abs/stream/flatbuffers_common_builder.h>
#include <dse/ncodec/codec/ab/vector.h>
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
    /* Logging interface. */
    ABCodecInstance* log_nc;
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
    char*   name;              /* Optional name of node. */
    char*   model;             /* Bus Model. */
    char*   mode;              /* Mode (of Bus Model operation). */
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

    /* Trace interface. */
    NCodecTraceLogLevel log_level;
} ABCodecInstance;


/* Log interface. */
#define AB_CODEC_LOG_BUFFER_SIZE 512
static inline void __trace_log(void* nc, NCodecTraceLogLevel level,
    const char* file, int line, const char* format, ...)
{
    int         errno_save = errno;
    va_list     args;
    static char buffer[AB_CODEC_LOG_BUFFER_SIZE];

    if (((NCodecInstance*)nc)->trace.log == NULL) return;

    va_start(args, format);
    int pos = vsnprintf(buffer, AB_CODEC_LOG_BUFFER_SIZE, format, args);
    va_end(args);
    if (level != NCODEC_LOG_NOTICE && pos >= 0) {
        snprintf(buffer + pos, AB_CODEC_LOG_BUFFER_SIZE - pos, " (%s:%0d)",
            file, line);
    }
    errno = errno_save;

    ((NCodecInstance*)nc)->trace.log(nc, level, buffer);
}

#define log_trace(nc, ...)                                                     \
    do {                                                                       \
        if (((ABCodecInstance*)nc)->log_level <= NCODEC_LOG_TRACE)             \
            __trace_log(                                                       \
                nc, NCODEC_LOG_TRACE, __func__, __LINE__, __VA_ARGS__);        \
    } while (0)

#define log_debug(nc, ...)                                                     \
    do {                                                                       \
        if (((ABCodecInstance*)nc)->log_level <= NCODEC_LOG_DEBUG)             \
            __trace_log(                                                       \
                nc, NCODEC_LOG_DEBUG, __func__, __LINE__, __VA_ARGS__);        \
    } while (0)

#define log_info(nc, ...)                                                      \
    do {                                                                       \
        if (((ABCodecInstance*)nc)->log_level <= NCODEC_LOG_INFO)              \
            __trace_log(nc, NCODEC_LOG_INFO, __func__, __LINE__, __VA_ARGS__); \
    } while (0)

#define log_simbus(nc, ...)                                                    \
    do {                                                                       \
        if (((ABCodecInstance*)nc)->log_level <= NCODEC_LOG_SIMBUS)            \
            __trace_log(                                                       \
                nc, NCODEC_LOG_SIMBUS, __func__, __LINE__, __VA_ARGS__);       \
    } while (0)

#define log_notice(nc, ...)                                                    \
    do {                                                                       \
        __trace_log(nc, NCODEC_LOG_NOTICE, __func__, __LINE__, __VA_ARGS__);   \
    } while (0)

#define log_error(nc, ...)                                                     \
    do {                                                                       \
        if (((ABCodecInstance*)nc)->log_level <= NCODEC_LOG_ERROR)             \
            __trace_log(                                                       \
                nc, NCODEC_LOG_ERROR, __func__, __LINE__, __VA_ARGS__);        \
    } while (0)

#define log_fatal(nc, ...)                                                     \
    do {                                                                       \
        __trace_log(nc, NCODEC_LOG_FATAL, __func__, __LINE__, __VA_ARGS__);    \
    } while (0)


#endif  // DSE_NCODEC_CODEC_AB_CODEC_H_
