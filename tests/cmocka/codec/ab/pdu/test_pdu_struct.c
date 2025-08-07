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


void test_pdu_transport_struct(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";
    const char* type_name = "foo";
    const char* var_name = "bar";
    const char* encoding = "foobar";
    uint16_t    attribute_aligned = 16;
    bool        attribute_packed = true;
    const char* platform_arch = "amd64";
    const char* platform_os = "linux";
    const char* platform_abi = "abc";

    // Write and flush a message.
    ncodec_truncate(nc);
    rc = ncodec_write(nc, &(struct NCodecPdu){
        .id = 42,
        .payload = (uint8_t*)greeting,
        .payload_len = strlen(greeting),
        .transport_type = NCodecPduTransportTypeStruct,
        .transport.struct_object = {
            .type_name = type_name,
            .var_name = var_name,
            .encoding = encoding,
            .attribute_aligned = attribute_aligned,
            .attribute_packed = attribute_packed,
            .platform_arch = platform_arch,
            .platform_os = platform_os,
            .platform_abi = platform_abi,
        },
    });
    assert_int_equal(rc, strlen(greeting));
    size_t len = ncodec_flush(nc);
    assert_int_equal(len, 0xce);

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

    // Check the transport.
    assert_string_equal(pdu.transport.struct_object.type_name, type_name);
    assert_string_equal(pdu.transport.struct_object.var_name, var_name);
    assert_string_equal(pdu.transport.struct_object.encoding, encoding);
    assert_int_equal(
        pdu.transport.struct_object.attribute_aligned, attribute_aligned);
    assert_int_equal(
        pdu.transport.struct_object.attribute_packed, attribute_packed);
    assert_string_equal(
        pdu.transport.struct_object.platform_arch, platform_arch);
    assert_string_equal(pdu.transport.struct_object.platform_os, platform_os);
    assert_string_equal(pdu.transport.struct_object.platform_abi, platform_abi);
}


int run_pdu_struct_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_pdu_transport_struct, s, t),
    };

    return cmocka_run_group_tests_name("PDU STRUCT", tests, NULL, NULL);
}
