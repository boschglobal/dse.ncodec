// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <errno.h>
#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/interface/pdu.h>

#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define BUFFER_LEN    1024


extern NCodecConfigItem codec_stat(NCODEC* nc, int* index);
extern NCODEC*          ncodec_create(const char* mime_type);
extern int32_t stream_read(NCODEC* nc, uint8_t** data, size_t* len, int pos_op);

NCODEC* ncodec_open(const char* mime_type, NCodecStreamVTable* stream)
{
    NCODEC* nc = ncodec_create(mime_type);
    if (nc) {
        NCodecInstance* _nc = (NCodecInstance*)nc;
        _nc->stream = stream;
    }
    return nc;
}

typedef struct Mock {
    NCODEC* nc;
} Mock;


#define MIMETYPE                                                               \
    "application/x-automotive-bus; "                                           \
    "interface=stream;type=pdu;schema=fbs;"                                    \
    "swc_id=4;ecu_id=5"
#define BUF_SWCID_OFFSET 40


static int test_setup(void** state)
{
    Mock* mock = calloc(1, sizeof(Mock));
    assert_non_null(mock);

    NCodecStreamVTable* stream = ncodec_buffer_stream_create(BUFFER_LEN);
    mock->nc = (void*)ncodec_open(MIMETYPE, stream);
    assert_non_null(mock->nc);

    *state = mock;
    return 0;
}


static int test_teardown(void** state)
{
    Mock* mock = *state;
    if (mock && mock->nc) ncodec_close((void*)mock->nc);
    if (mock) free(mock);

    return 0;
}


void test_pdu_fbs_no_stream(void** state)
{
    Mock* mock = *state;
    UNUSED(mock);
    int rc;

    const char* greeting = "Hello World";
    uint8_t     payload[BUFFER_LEN];

    NCODEC* nc = (void*)ncodec_create(MIMETYPE);
    assert_non_null(nc);

    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting,
                              .payload_len = strlen(greeting) });
    assert_int_equal(rc, -ENOSR);
    rc = ncodec_flush(nc);
    assert_int_equal(rc, -ENOSR);

    NCodecPdu pdu = {};
    rc = ncodec_read(nc, &pdu);
    assert_int_equal(rc, -ENOSR);
    assert_null(pdu.payload);
    assert_int_equal(pdu.payload_len, 0);

    ncodec_close(nc);
}


void test_pdu_fbs_no_payload(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    rc = ncodec_write(nc, NULL);
    assert_int_equal(rc, -EINVAL);

    rc = ncodec_read(nc, NULL);
    assert_int_equal(rc, -EINVAL);
};


void test_pdu_fbs_flush(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    rc = ncodec_flush(nc);
    assert_int_equal(rc, 0);
}


void test_pdu_fbs_truncate(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";

    // Write to the stream.
    ncodec_seek(nc, 0, NCODEC_SEEK_RESET);
    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting,
                              .payload_len = strlen(greeting) });
    assert_int_equal(rc, strlen(greeting));
    assert_int_equal(0x56, ncodec_flush(nc));
    assert_int_equal(0x56, ncodec_tell(nc));

    // Truncate the stream.
    rc = ncodec_truncate(nc);
    assert_int_equal(rc, 0);
    assert_int_equal(0, ncodec_tell(nc));

    // Flush the stream, and check no payloaded content was retained.
    rc = ncodec_flush(nc);
    assert_int_equal(rc, 0);
    assert_int_equal(0, ncodec_tell(nc));
}


void test_pdu_fbs_read_nomsg(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    ncodec_seek(nc, 0, NCODEC_SEEK_RESET);
    NCodecPdu pdu = {};
    size_t    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, -ENOMSG);
    assert_int_equal(pdu.payload_len, 0);
    assert_null(pdu.payload);
}


void test_pdu_fbs_write(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";

    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting,
                              .payload_len = strlen(greeting) });
    assert_int_equal(rc, strlen(greeting));
    size_t len = ncodec_flush(nc);
    assert_int_equal(len, 0x56);

    // Check the result in the stream.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
    if (0) {
        for (uint32_t i = 0; i < buffer_len; i += 8)
            printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", buffer[i + 0],
                buffer[i + 1], buffer[i + 2], buffer[i + 3], buffer[i + 4],
                buffer[i + 5], buffer[i + 6], buffer[i + 7]);
    }
    assert_int_equal(len, buffer_len);
    assert_memory_equal(buffer + 52, greeting, strlen(greeting));
}


void test_pdu_fbs_readwrite(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";

    // Write and flush a message.
    ncodec_seek(nc, 0, NCODEC_SEEK_RESET);
    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting,
                              .payload_len = strlen(greeting) });
    assert_int_equal(rc, strlen(greeting));
    size_t len = ncodec_flush(nc);
    assert_int_equal(len, 0x56);

    // Seek to the start, keeping the content, modify the node_id.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
    buffer[BUF_SWCID_OFFSET] = 0x22;
    if (0) {
        for (uint32_t i = 0; i < buffer_len; i += 8)
            printf("%p: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                &(buffer[i + 0]), buffer[i + 0], buffer[i + 1], buffer[i + 2],
                buffer[i + 3], buffer[i + 4], buffer[i + 5], buffer[i + 6],
                buffer[i + 7]);
    }

    // Read the message back.
    NCodecPdu pdu = {};
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, strlen(greeting));
    assert_int_equal(pdu.payload_len, strlen(greeting));
    assert_non_null(pdu.payload);
    assert_memory_equal(pdu.payload, greeting, strlen(greeting));
    assert_int_equal(pdu.swc_id, 0x22);  // Note this value was modified.
    assert_int_equal(pdu.ecu_id, 5);
}


void test_pdu_fbs_readwrite_pdus(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting1 = "Hello World";
    const char* greeting2 = "Foo Bar";

    // Write and flush a message.
    ncodec_seek(nc, 0, NCODEC_SEEK_RESET);
    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting1,
                              .payload_len = strlen(greeting1),
                              .swc_id = 42,
                              .ecu_id = 24 });
    assert_int_equal(rc, strlen(greeting1));
    ncodec_seek(nc, 0, NCODEC_SEEK_RESET);
    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting2,
                              .payload_len = strlen(greeting2),
                              .swc_id = 42,
                              .ecu_id = 24 });
    assert_int_equal(rc, strlen(greeting2));
    size_t len = ncodec_flush(nc);
    assert_int_equal(len, 0x7a);

    // Seek to the start, keeping the content, modify the node_id.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
    buffer[BUF_SWCID_OFFSET + 4] = 0x42;
    buffer[BUF_SWCID_OFFSET + 4 + 32] = 0x42;
    if (0) {
        for (uint32_t i = 0; i < buffer_len; i += 8)
            printf("%p: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                &(buffer[i + 0]), buffer[i + 0], buffer[i + 1], buffer[i + 2],
                buffer[i + 3], buffer[i + 4], buffer[i + 5], buffer[i + 6],
                buffer[i + 7]);
    }

    // Read the message back.
    NCodecPdu pdu = {};

    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, strlen(greeting1));
    assert_int_equal(pdu.payload_len, strlen(greeting1));
    assert_non_null(pdu.payload);
    assert_memory_equal(pdu.payload, greeting1, strlen(greeting1));
    assert_int_equal(pdu.swc_id, 0x42);  // Note this value was modified.
    assert_int_equal(pdu.ecu_id, 24);


    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, strlen(greeting2));
    assert_int_equal(pdu.payload_len, strlen(greeting2));
    assert_non_null(pdu.payload);
    assert_memory_equal(pdu.payload, greeting2, strlen(greeting2));
    assert_int_equal(pdu.swc_id, 0x42);  // Note this value was modified.
    assert_int_equal(pdu.ecu_id, 24);
}


void test_pdu_fbs_readwrite_messages(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting1 = "Hello World";
    const char* greeting2 = "Foo Bar";

    // Write and flush a message.
    ncodec_seek(nc, 0, NCODEC_SEEK_RESET);
    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting1,
                              .payload_len = strlen(greeting1),
                              .swc_id = 42,
                              .ecu_id = 24 });
    assert_int_equal(rc, strlen(greeting1));
    size_t len = ncodec_flush(nc);
    assert_int_equal(len, 0x56);

    rc = ncodec_write(nc, &(struct NCodecPdu){ .id = 42,
                              .payload = (uint8_t*)greeting2,
                              .payload_len = strlen(greeting2),
                              .swc_id = 42,
                              .ecu_id = 24 });
    assert_int_equal(rc, strlen(greeting2));
    len = ncodec_flush(nc);
    assert_int_equal(len, 0x52);

    // Seek to the start, keeping the content, modify the node_id.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
    buffer[BUF_SWCID_OFFSET] = 0x42;
    buffer[BUF_SWCID_OFFSET + 0x56] = 0x42;
    if (0) {
        for (uint32_t i = 0; i < buffer_len; i += 8)
            printf("%p: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                &(buffer[i + 0]), buffer[i + 0], buffer[i + 1], buffer[i + 2],
                buffer[i + 3], buffer[i + 4], buffer[i + 5], buffer[i + 6],
                buffer[i + 7]);
    }

    // Read the message back.
    NCodecPdu pdu = {};

    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, strlen(greeting1));
    assert_int_equal(pdu.payload_len, strlen(greeting1));
    assert_non_null(pdu.payload);
    assert_memory_equal(pdu.payload, greeting1, strlen(greeting1));
    assert_int_equal(pdu.swc_id, 0x42);  // Note this value was modified.
    assert_int_equal(pdu.ecu_id, 24);


    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, strlen(greeting2));
    assert_int_equal(pdu.payload_len, strlen(greeting2));
    assert_non_null(pdu.payload);
    assert_memory_equal(pdu.payload, greeting2, strlen(greeting2));
    assert_int_equal(pdu.swc_id, 0x42);  // Note this value was modified.
    assert_int_equal(pdu.ecu_id, 24);
}


int run_pdu_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_pdu_fbs_no_stream, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_no_payload, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_flush, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_read_nomsg, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_write, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_readwrite, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_truncate, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_readwrite_pdus, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_fbs_readwrite_messages, s, t),
    };

    return cmocka_run_group_tests_name("PDU", tests, NULL, NULL);
}
