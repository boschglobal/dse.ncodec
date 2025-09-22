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


void test_pdu_transport_ip__eth(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";

    // Write and flush a message.
    ncodec_truncate(nc);
    rc = ncodec_write(nc, &(struct NCodecPdu){
        .id = 42,
        .payload = (uint8_t*)greeting,
        .payload_len = strlen(greeting),
        .transport_type = NCodecPduTransportTypeIp,
        .transport.ip_message = {
            .eth_dst_mac = 0x0000123456789ABC,
            .eth_src_mac = 0x0000CBA987654321,
            .eth_ethertype = 1,
            .eth_tci_pcp = 2,
            .eth_tci_dei = 3,
            .eth_tci_vid = 4,
        },
    });
    assert_int_equal(rc, strlen(greeting));
    size_t len = ncodec_flush(nc);
    assert_int_equal(len, 0x8e);

    // Seek to the start, keeping the content, modify the node_id.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
    buffer[BUF_SWCID_OFFSET + 4] = 0x22;
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
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeIp);
    assert_int_equal(pdu.transport.ip_message.eth_dst_mac, 0x0000123456789ABC);
    assert_int_equal(pdu.transport.ip_message.eth_src_mac, 0x0000CBA987654321);
    assert_int_equal(pdu.transport.ip_message.eth_ethertype, 1);
    assert_int_equal(pdu.transport.ip_message.eth_tci_pcp, 2);
    assert_int_equal(pdu.transport.ip_message.eth_tci_dei, 3);
    assert_int_equal(pdu.transport.ip_message.eth_tci_vid, 4);
}

void test_pdu_transport_ip__ip(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";

    // Write and flush a message.
    ncodec_truncate(nc);
    rc = ncodec_write(nc, &(struct NCodecPdu){
                            .id = 42,
                            .swc_id = 44,
                            .payload = (uint8_t*)greeting,
                            .payload_len = strlen(greeting),
                            .transport_type = NCodecPduTransportTypeIp,
                            .transport.ip_message = {
                                .ip_protocol = NCodecPduIpProtocolTcp,
                                .ip_addr_type = NCodecPduIpAddrIPv4,
                                .ip_addr.ip_v4.src_addr = 1001,
                                .ip_addr.ip_v4.dst_addr = 2002,
                                .ip_src_port = 3003,
                                .ip_dst_port = 4004,
                            },
                        });
    assert_int_equal(rc, strlen(greeting));

    rc = ncodec_write(nc, &(struct NCodecPdu){
                            .id = 42,
                            .swc_id = 44,
                            .payload = (uint8_t*)greeting,
                            .payload_len = strlen(greeting),
                            .transport_type = NCodecPduTransportTypeIp,
                            .transport.ip_message = {
                                .ip_protocol = NCodecPduIpProtocolUdp,
                                .ip_addr_type = NCodecPduIpAddrIPv6,
                                .ip_addr.ip_v6 = {
                                    .src_addr = {1, 2, 3, 4, 5, 6, 7, 8},
                                    .dst_addr = {2, 2, 4, 4, 6, 6, 8, 8},
                                },
                                .ip_src_port = 4003,
                                .ip_dst_port = 3004,
                            },
                        });
    assert_int_equal(rc, strlen(greeting));

    size_t len = ncodec_flush(nc);
    // assert_int_equal(len, 0x8e);

    // Seek to the start, keeping the content, modify the node_id.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
    if (0) {
        for (uint32_t i = 0; i < buffer_len; i += 8)
            printf("%p: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                &(buffer[i + 0]), buffer[i + 0], buffer[i + 1], buffer[i + 2],
                buffer[i + 3], buffer[i + 4], buffer[i + 5], buffer[i + 6],
                buffer[i + 7]);
    }

    NCodecPdu pdu = {};

    // Read the next message (first)
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, strlen(greeting));
    assert_int_equal(pdu.payload_len, strlen(greeting));
    assert_non_null(pdu.payload);
    assert_memory_equal(pdu.payload, greeting, strlen(greeting));
    assert_int_equal(pdu.swc_id, 44);
    assert_int_equal(pdu.ecu_id, 5);
    // Check the transport.
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeIp);
    assert_int_equal(
        pdu.transport.ip_message.ip_protocol, NCodecPduIpProtocolTcp);
    assert_int_equal(
        pdu.transport.ip_message.ip_addr_type, NCodecPduIpAddrIPv4);
    assert_int_equal(pdu.transport.ip_message.ip_addr.ip_v4.src_addr, 1001);
    assert_int_equal(pdu.transport.ip_message.ip_addr.ip_v4.dst_addr, 2002);
    assert_int_equal(pdu.transport.ip_message.ip_src_port, 3003);
    assert_int_equal(pdu.transport.ip_message.ip_dst_port, 4004);

    // Read the message back (second).
    len = ncodec_read(nc, &pdu);
    assert_int_equal(len, strlen(greeting));
    assert_int_equal(pdu.payload_len, strlen(greeting));
    assert_non_null(pdu.payload);
    assert_memory_equal(pdu.payload, greeting, strlen(greeting));
    assert_int_equal(pdu.swc_id, 44);
    assert_int_equal(pdu.ecu_id, 5);
    // Check the transport.
    uint16_t src[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint16_t dst[] = { 2, 2, 4, 4, 6, 6, 8, 8 };
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeIp);
    assert_int_equal(
        pdu.transport.ip_message.ip_protocol, NCodecPduIpProtocolUdp);
    assert_int_equal(
        pdu.transport.ip_message.ip_addr_type, NCodecPduIpAddrIPv6);
    assert_memory_equal(
        pdu.transport.ip_message.ip_addr.ip_v6.src_addr, src, 16);
    assert_memory_equal(
        pdu.transport.ip_message.ip_addr.ip_v6.dst_addr, dst, 16);
    assert_int_equal(pdu.transport.ip_message.ip_src_port, 4003);
    assert_int_equal(pdu.transport.ip_message.ip_dst_port, 3004);
}

void test_pdu_transport_ip__module_do_ad(void** state)
{
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";

    // Write and flush a message.
    ncodec_truncate(nc);
    rc = ncodec_write(nc, &(struct NCodecPdu){
                            .id = 42,
                            .swc_id = 24,
                            .payload = (uint8_t*)greeting,
                            .payload_len = strlen(greeting),
                            .transport_type = NCodecPduTransportTypeIp,
                            .transport.ip_message = {
                                .so_ad_type = NCodecPduSoAdDoIP,
                                .so_ad.do_ip = {
                                    .protocol_version = 4,
                                    .payload_type = 6,
                                },
                            },
                        });
    assert_int_equal(rc, strlen(greeting));
    size_t len = ncodec_flush(nc);

    // Seek to the start, keeping the content, modify the node_id.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
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
    assert_int_equal(pdu.swc_id, 24);  // Note this value was modified.
    assert_int_equal(pdu.ecu_id, 5);

    // Check the transport.
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeIp);
    assert_int_equal(pdu.transport.ip_message.so_ad_type, NCodecPduSoAdDoIP);
    assert_int_equal(pdu.transport.ip_message.so_ad.do_ip.protocol_version, 4);
    assert_int_equal(pdu.transport.ip_message.so_ad.do_ip.payload_type, 6);
}

void test_pdu_transport_ip__module_some_ip(void** state)
{
    // some_ip:SomeIpMetadata;
    Mock*   mock = *state;
    NCODEC* nc = mock->nc;
    int     rc;

    const char* greeting = "Hello World";

    // Write and flush a message.
    ncodec_truncate(nc);
    rc = ncodec_write(nc, &(struct NCodecPdu){
                            .id = 42,
                            .swc_id = 24,
                            .payload = (uint8_t*)greeting,
                            .payload_len = strlen(greeting),
                            .transport_type = NCodecPduTransportTypeIp,
                            .transport.ip_message = {
                                .so_ad_type = NCodecPduSoAdSomeIP,
                                .so_ad.some_ip = {
                                    .message_id = 10,
                                    .length = 11,
                                    .request_id = 12,
                                    .protocol_version = 13,
                                    .interface_version = 14,
                                    .message_type = 15,
                                    .return_code = 16,
                                },
                            },
                        });
    assert_int_equal(rc, strlen(greeting));
    size_t len = ncodec_flush(nc);

    // Seek to the start, keeping the content, modify the node_id.
    ncodec_seek(nc, 0, NCODEC_SEEK_SET);
    uint8_t* buffer;
    size_t   buffer_len;
    stream_read(nc, &buffer, &buffer_len, NCODEC_POS_NC);
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
    assert_int_equal(pdu.swc_id, 24);  // Note this value was modified.
    assert_int_equal(pdu.ecu_id, 5);

    // Check the transport.
    assert_int_equal(pdu.transport_type, NCodecPduTransportTypeIp);
    assert_int_equal(pdu.transport.ip_message.so_ad_type, NCodecPduSoAdSomeIP);
    assert_int_equal(pdu.transport.ip_message.so_ad.some_ip.message_id, 10);
    assert_int_equal(pdu.transport.ip_message.so_ad.some_ip.length, 11);
    assert_int_equal(pdu.transport.ip_message.so_ad.some_ip.request_id, 12);
    assert_int_equal(
        pdu.transport.ip_message.so_ad.some_ip.protocol_version, 13);
    assert_int_equal(
        pdu.transport.ip_message.so_ad.some_ip.interface_version, 14);
    assert_int_equal(pdu.transport.ip_message.so_ad.some_ip.message_type, 15);
    assert_int_equal(pdu.transport.ip_message.so_ad.some_ip.return_code, 16);
}


int run_pdu_ip_tests(void)
{
    void* s = test_setup;
    void* t = test_teardown;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_pdu_transport_ip__eth, s, t),
        cmocka_unit_test_setup_teardown(test_pdu_transport_ip__ip, s, t),
        cmocka_unit_test_setup_teardown(
            test_pdu_transport_ip__module_do_ad, s, t),
        cmocka_unit_test_setup_teardown(
            test_pdu_transport_ip__module_some_ip, s, t),
    };

    return cmocka_run_group_tests_name("PDU IP", tests, NULL, NULL);
}
