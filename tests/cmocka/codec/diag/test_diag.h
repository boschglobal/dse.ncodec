// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef TESTS_CMOCKA_CODEC_DIAG_TEST_DIAG_H_
#define TESTS_CMOCKA_CODEC_DIAG_TEST_DIAG_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <dse/ncodec/codec.h>


#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define BUFFER_LEN    1024

#define MIMETYPE(t)                                                            \
    "application/x-automotive-bus; "                                           \
    "interface=diagnostic;type=" t ";schema=fbs"

#define assert_string_or_null(a, b)                                            \
    do {                                                                       \
        if ((a) == NULL) {                                                     \
            assert_null(b);                                                    \
        } else {                                                               \
            assert_non_null(b);                                                \
            assert_string_equal(a, b);                                         \
        }                                                                      \
    } while (0)

#define assert_vector_or_null(a, a_len, b, b_len)                              \
    do {                                                                       \
        assert_true(((a) == NULL) == ((b) == NULL));                           \
        assert_int_equal(a_len, b_len);                                        \
        if (a_len) assert_memory_equal(a, b, (a_len) * sizeof((a)[0]));        \
    } while (0)


typedef struct Mock {
    NCODEC* nc;
} Mock;


/* test_codec.c */
extern NCODEC*        ncodec_open(const char* mime_type, NSTREAM* stream);
extern int            diag_test_setup(void** state, const char* mime_type);
extern int            diag_test_teardown(void** state);
extern const uint8_t* stream_buffer(NCODEC* nc, size_t* length);
extern void           stream_append(NCODEC* dst, NCODEC* src);


#endif  // TESTS_CMOCKA_CODEC_DIAG_TEST_DIAG_H_
