// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_CODEC_H_
#define DSE_NCODEC_CODEC_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>


/* DLL Interface visibility. */
#if defined _WIN32 || defined __CYGWIN__
#ifdef DLL_BUILD
#define DLL_PUBLIC __declspec(dllexport)
#else
#define DLL_PUBLIC __declspec(dllimport)
#endif /* DLL_BUILD */
#define DLL_PRIVATE
#else /* Linux */
#define DLL_PUBLIC  __attribute__((visibility("default")))
#define DLL_PRIVATE __attribute__((visibility("hidden")))
#endif /* _WIN32 || defined __CYGWIN__ */


/**
Network Codec
=============

A Network Codec has two interfaces: a Codec Interface which is used to
encode/decode message from a Model/Device (connected to a Network), and a
Stream Interface which is used to exchange the encoded messages with other
Model/Devices connected to the same Network.

The Network Codec API (codec.h & codec.c) provides the framework
for implementing both the Codec Interface and the Stream Interface.
A typical realisation of this scheme would be:

* Stream Implementation - provided by the Model Environment.
* Network Codec Implementation - provided by a Codec vendor.
* Model - using the Network Codec API to configure and use a Network, with
  the assistance of both the Network Codec and Stream implementations.


Component Diagram
-----------------
<div hidden>

```
@startuml ncodec-component

title Network Codec

package "Model Environment" {
    interface "Signal Interface" as Sif
    component "Model" as foo {
                component "Model" as Model
                interface "CodecVTable" as Cvt
                component "Codec" as Codec
        interface "StreamVTable" as Svt
    }
    component "Stream" as Stream
    interface "Binary Interface" as Bif
    component "Codec Lib" as lib
}

Sif )-down- Model
Model -right-( Cvt
Cvt -right- Codec
Codec -right-( Svt
Svt -right- Stream
Stream -down-( Bif

lib .up.> Codec

Model --> lib :ncodec_open()

center footer Dynamic Simulation Environment

@enduml
```

</div>

![](ncodec-component.png)


Example
-------

{{< readfile file="examples/ncodec_api.c" code="true" lang="c" >}}

*/

typedef void NCODEC;


/* Stream Interface */

#define NCODEC_EOF true

typedef enum NCodecStreamSeekOperation {
    NCODEC_SEEK_SET = 0,
    NCODEC_SEEK_CUR,
    NCODEC_SEEK_END,
    NCODEC_SEEK_RESET,
} NCodecStreamSeekOperation;

typedef enum NCodecStreamPosOperation {
    NCODEC_POS_UPDATE = 0,
    NCODEC_POS_NC,
} NCodecStreamPosOperation;

typedef size_t (*NCodecStreamRead)(
    NCODEC* nc, uint8_t** data, size_t* len, int32_t pos_op);
typedef size_t (*NCodecStreamWrite)(NCODEC* nc, uint8_t* data, size_t len);
typedef int64_t (*NCodecStreamSeek)(NCODEC* nc, size_t pos, int32_t op);
typedef int64_t (*NCodecStreamTell)(NCODEC* nc);
typedef int32_t (*NCodecStreamEof)(NCODEC* nc);
typedef int32_t (*NCodecStreamClose)(NCODEC* nc);

typedef struct NCodecStreamVTable {
    NCodecStreamRead  read;
    NCodecStreamWrite write;
    NCodecStreamSeek  seek;
    NCodecStreamTell  tell;
    NCodecStreamEof   eof;
    NCodecStreamClose close;
} NCodecStreamVTable;


/** CODEC Interface */

typedef struct NCodecConfigItem {
    const char* name;
    const char* value;
} NCodecConfigItem;

typedef void NCodecMessage; /* Generic message container. */


typedef int32_t NCodecLoad(const char* filename, const char* hint);
typedef NCODEC* NCodecOpen(const char* mime_type, NCodecStreamVTable* stream);
typedef NCODEC* NCodecCreate(const char* mime_type);

typedef int32_t (*NCodecConfig)(NCODEC* nc, NCodecConfigItem item);
typedef NCodecConfigItem (*NCodecStat)(NCODEC* nc, int32_t* index);
typedef int32_t (*NCodecWrite)(NCODEC* nc, NCodecMessage* msg);
typedef int32_t (*NCodecRead)(NCODEC* nc, NCodecMessage* msg);
typedef int32_t (*NCodecFlush)(NCODEC* nc);
typedef int32_t (*NCodecTruncate)(NCODEC* nc);
typedef void (*NCodecClose)(NCODEC* nc);

typedef struct NCodecVTable {
    NCodecConfig   config;
    NCodecStat     stat;
    NCodecWrite    write;
    NCodecRead     read;
    NCodecFlush    flush;
    NCodecTruncate truncate;
    NCodecClose    close;
} NCodecVTable;


typedef enum NCodecTraceLogLevel {
    NCODEC_LOG_TRACE = 0,
    NCODEC_LOG_DEBUG,
    NCODEC_LOG_CODEC,   /* Intermediate debug messages. */
    NCODEC_LOG_INFO,
    NCODEC_LOG_NOTICE, /* Application level messages. */
    NCODEC_LOG_QUIET,  /* Suppress all logs, excluding errors. */
    NCODEC_LOG_ERROR,  /* May print errno, if set. */
    NCODEC_LOG_FATAL,  /* Will print errno and then call exit(). */
} NCodecTraceLogLevel;

typedef void (*NCodecTraceWrite)(NCODEC* nc, NCodecMessage* msg);
typedef void (*NCodecTraceRead)(NCODEC* nc, NCodecMessage* msg);
typedef void (*NCodecTraceLog)(NCODEC* nc, NCodecTraceLogLevel level,
    const char* msg);

typedef struct NCodecTraceVTable {
    NCodecTraceWrite write;
    NCodecTraceRead  read;
    NCodecTraceLog   log;
} NCodecTraceVTable;


typedef struct NCodecInstance {
    const char*         mime_type;
    NCodecVTable        codec;
    NCodecStreamVTable* stream;
    /* Trace interface (optional). */
    NCodecTraceVTable   trace;
    /* Private reference data from API user (optional). */
    void* private;
} NCodecInstance;


/* Implemented by Codec. */
DLL_PUBLIC NCodecCreate ncodec_create;

/* Implemented by integrator. */
DLL_PUBLIC int32_t ncodec_load(const char* filename, const char* hint);
DLL_PUBLIC NCODEC* ncodec_open(
    const char* mime_type, NCodecStreamVTable* stream);

/* Provided by codec.c (in this package). */
DLL_PUBLIC void             ncodec_config(NCODEC* nc, NCodecConfigItem item);
DLL_PUBLIC NCodecConfigItem ncodec_stat(NCODEC* nc, int32_t* index);
DLL_PUBLIC int32_t          ncodec_write(NCODEC* nc, NCodecMessage* msg);
DLL_PUBLIC int32_t          ncodec_read(NCODEC* nc, NCodecMessage* msg);
DLL_PUBLIC int32_t          ncodec_flush(NCODEC* nc);
DLL_PUBLIC int32_t          ncodec_truncate(NCODEC* nc);
DLL_PUBLIC void             ncodec_close(NCODEC* nc);
DLL_PUBLIC int64_t          ncodec_seek(NCODEC* nc, size_t pos, int32_t op);
DLL_PUBLIC int64_t          ncodec_tell(NCODEC* nc);

#endif  // DSE_NCODEC_CODEC_H_
