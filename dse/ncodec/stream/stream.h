// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_STREAM_STREAM_H_
#define DSE_NCODEC_STREAM_STREAM_H_

#include <dse/platform.h>


/* buffer.c */
DLL_PUBLIC void* ncodec_buffer_stream_create(size_t buffer_size);

/* ascii85.c */
DLL_PUBLIC char* ncodec_ascii85_encode(const char* source, size_t source_len);
DLL_PUBLIC char* ncodec_ascii85_decode(const char* source, size_t* len);


#endif  // DSE_NCODEC_STREAM_STREAM_H_
