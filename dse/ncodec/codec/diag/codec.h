// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_CODEC_DIAG_CODEC_H_
#define DSE_NCODEC_CODEC_DIAG_CODEC_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/schema/abs/diagnostic/flatbuffers_common_reader.h>
#include <dse/ncodec/schema/abs/diagnostic/flatbuffers_common_builder.h>
#include <dse/ncodec/codec.h>


/* Declare an extension to the NCodecInstance type. */
typedef struct DiagCodecInstance {
    NCodecInstance c;

    /* Codec selectors: from MIMEtype. */
    char* interface;
    char* type;
    char* schema;

    /* Parameters: from MIMEtype or calls to ncodec_config(). */
    /* String representation (supporting ncodec_stat()). */
    char*   node_id_str;
    char*   swc_id_str;
    char*   ecu_id_str;
    char*   name;         /* Optional name of node. */
    char*   loopback_str; /* Disable filter sender==receiver. */
    /* Internal representation. */
    uint8_t node_id;
    uint8_t swc_id;
    uint8_t ecu_id;
    bool    loopback;

    /* Flatbuffer resources. */
    flatcc_builder_t fbs_builder;
    bool             fbs_builder_initalized;

    /* Free list (free called on truncate). */
    Vector free_list; /* void* references */

    /* Trace Log interface (NCodecTraceVTable). */
    NCodecTraceLogLevel log_level;
} DiagCodecInstance;


/* codec.c */
DLL_PRIVATE NCODEC* diag_codec_create(const char* mime_type);


/* Log interface. */
#define DIAG_CODEC_LOG_BUFFER_SIZE 512
static inline void __diag_trace_log(void* nc, NCodecTraceLogLevel level,
    const char* file, int line, const char* format, ...)
{
    int         errno_save = errno;
    va_list     args;
    static char buffer[DIAG_CODEC_LOG_BUFFER_SIZE];

    if (((NCodecInstance*)nc)->trace.log == NULL) return;

    va_start(args, format);
    int pos = vsnprintf(buffer, DIAG_CODEC_LOG_BUFFER_SIZE, format, args);
    va_end(args);
    if (level != NCODEC_LOG_NOTICE && pos >= 0) {
        snprintf(buffer + pos, DIAG_CODEC_LOG_BUFFER_SIZE - pos, " (%s:%0d)",
            file, line);
    }
    errno = errno_save;

    ((NCodecInstance*)nc)->trace.log(nc, level, buffer);
}

#undef log_trace
#define log_trace(nc, ...)                                                     \
    do {                                                                       \
        if (((DiagCodecInstance*)nc)->log_level <= NCODEC_LOG_TRACE)           \
            __diag_trace_log(                                                  \
                nc, NCODEC_LOG_TRACE, __func__, __LINE__, __VA_ARGS__);        \
    } while (0)

#undef log_debug
#define log_debug(nc, ...)                                                     \
    do {                                                                       \
        if (((DiagCodecInstance*)nc)->log_level <= NCODEC_LOG_DEBUG)           \
            __diag_trace_log(                                                  \
                nc, NCODEC_LOG_DEBUG, __func__, __LINE__, __VA_ARGS__);        \
    } while (0)

#undef log_info
#define log_info(nc, ...)                                                      \
    do {                                                                       \
        if (((DiagCodecInstance*)nc)->log_level <= NCODEC_LOG_INFO)            \
            __diag_trace_log(                                                  \
                nc, NCODEC_LOG_INFO, __func__, __LINE__, __VA_ARGS__);         \
    } while (0)

#undef log_simbus
#define log_simbus(nc, ...)                                                    \
    do {                                                                       \
        if (((DiagCodecInstance*)nc)->log_level <= NCODEC_LOG_SIMBUS)          \
            __diag_trace_log(                                                  \
                nc, NCODEC_LOG_SIMBUS, __func__, __LINE__, __VA_ARGS__);       \
    } while (0)

#undef log_notice
#define log_notice(nc, ...)                                                    \
    do {                                                                       \
        __diag_trace_log(                                                      \
            nc, NCODEC_LOG_NOTICE, __func__, __LINE__, __VA_ARGS__);           \
    } while (0)

#undef log_error
#define log_error(nc, ...)                                                     \
    do {                                                                       \
        if (((DiagCodecInstance*)nc)->log_level <= NCODEC_LOG_ERROR)           \
            __diag_trace_log(                                                  \
                nc, NCODEC_LOG_ERROR, __func__, __LINE__, __VA_ARGS__);        \
    } while (0)

#undef log_fatal
#define log_fatal(nc, ...)                                                     \
    do {                                                                       \
        __diag_trace_log(                                                      \
            nc, NCODEC_LOG_FATAL, __func__, __LINE__, __VA_ARGS__);            \
    } while (0)


#endif  // DSE_NCODEC_CODEC_DIAG_CODEC_H_
