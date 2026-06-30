// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/codec/ab/codec.h>


#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define BUFFER_LEN    1024  // Initial buffer size, will grow as needed.
#define CHECK_RC(call)                                                         \
    do {                                                                       \
        rc = (call);                                                           \
        if (rc < 0 && rc != -ENOMSG) {                                         \
            printf("Error: call:%s rc=%d\n", #call, rc);                       \
            return rc;                                                         \
        }                                                                      \
    } while (0)

extern int simbus(
    void* nodes, size_t node_size, size_t nc_offset, size_t count);
extern bool                       trace_off;
extern NCodecPduFlexrayConfig     fray_config;
extern NCodecPduFlexrayLpduConfig fray_frames[];
extern size_t                     frame_count(void);


typedef struct {
    const char*         name;
    const char*         mimetype;
    const char*         greeting;
    size_t              tx_frame_index;
    NCodecStreamVTable* stream;
    NCODEC*             nc;
} Node;

static Node node_table[] = {
    {
        .name = "ECU 1",
        .mimetype = "application/x-automotive-bus; "
                    "interface=stream;type=pdu;schema=fbs;"
                    "ecu_id=1;vcn=2;model=flexray;name=fray;loopback=1",
        .greeting = "Hello World from ECU 1",
        .tx_frame_index = 0,
    },
    {
        .name = "ECU 2",
        .mimetype = "application/x-automotive-bus; "
                    "interface=stream;type=pdu;schema=fbs;"
                    "ecu_id=2;vcn=2;model=flexray;name=fray;loopback=1",
        .greeting = "Hello World from ECU 2",
        .tx_frame_index = 1,
    },
    {
        .name = "ECU 3",
        .mimetype = "application/x-automotive-bus; "
                    "interface=stream;type=pdu;schema=fbs;"
                    "ecu_id=3;vcn=2;model=flexray;name=fray;loopback=1",
        .greeting = "Hello World from ECU 3",
        .tx_frame_index = 2,
    },
};


int setup(Node* node)
{
    int rc = 0;
    node->stream = ncodec_buffer_stream_create(BUFFER_LEN);
    node->nc = ncodec_open(node->mimetype, node->stream);

    /* Push FlexRay Config Tables. */
    NCodecPduFlexrayLpduConfig* frames =
        calloc(frame_count(), sizeof(NCodecPduFlexrayLpduConfig));
    for (size_t i = 0; i < frame_count(); i++) {
        frames[i] = fray_frames[i];
        if (node->tx_frame_index == i) {
            frames[i].direction = NCodecPduFlexrayDirectionTx;
            frames[i].transmit_mode = NCodecPduFlexrayTransmitModeContinuous;
        }
    }
    NCodecPduFlexrayConfig config = fray_config;
    config.frame_config.table = frames;
    config.frame_config.count = frame_count();
    ncodec_write(node->nc, &(struct NCodecPdu){
        .transport_type = NCodecPduTransportTypeFlexray,
        .transport.flexray = {
            .metadata_type = NCodecPduFlexrayMetadataTypeConfig,
            .metadata.config = config,
        },
    });
    free(frames);

    /* Push FlexRay L-PDUs. */
    for (size_t i = 0; i < frame_count(); i++) {
        if (node->tx_frame_index == i) {
            /* This nodes Tx frame. */
            CHECK_RC(ncodec_write(node->nc,
                &(struct NCodecPdu){ .id = fray_frames[i].slot_id,
                    .payload = (uint8_t*)node->greeting,
                    .payload_len = strlen(node->greeting) + 1,
                    .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray = {
                        .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                        .metadata.lpdu = {
                            .frame_config_index = i,
                            .status = NCodecPduFlexrayLpduStatusNotTransmitted,
                        },
                    }
                }));
        } else {
            /* Otherwise an Rx frame. */
            CHECK_RC(ncodec_write(node->nc,
                &(struct NCodecPdu){ .id = fray_frames[i].slot_id,
                    .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray = {
                        .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                        .metadata.lpdu = {
                            .frame_config_index = i,
                            .status = NCodecPduFlexrayLpduStatusNotReceived,
                        },
                    }
                }));
        }
    }

    CHECK_RC(ncodec_flush(node->nc)); /* Call after writing PDUs. */
    return 0;
}


int step(Node* node, int s)
{
    int rc = 0;

    /* Read PDUs from the NCodec. */
    while (1) {
        NCodecPdu msg = {};
        CHECK_RC(ncodec_read(node->nc, &msg));
        if (rc == -ENOMSG) break;
        if (msg.transport_type != NCodecPduTransportTypeFlexray) continue;
        if (msg.transport.flexray.metadata_type !=
            NCodecPduFlexrayMetadataTypeLpdu)
            continue;
        if (msg.transport.flexray.metadata.lpdu.status ==
            NCodecPduFlexrayLpduStatusReceived) {
            printf("%s (@%0.4f) -> Message is: %s\n", node->name, s * 0.0005,
                (char*)msg.payload);
        }
    }
    CHECK_RC(ncodec_truncate(node->nc)); /* Always call _once_ per step. */
    CHECK_RC(ncodec_flush(node->nc)); /* Typically call after writing PDUs. */

    return 0;
}

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    int rc = 0;
    trace_off = true;

    /* Setup the nodes. */
    for (size_t i = 0; i < ARRAY_SIZE(node_table); i++) {
        Node* n = &node_table[i];
        setup(n);
    }
    simbus(node_table, sizeof(node_table[0]), offsetof(Node, nc),
        ARRAY_SIZE(node_table));

    /* Complete a Co-Simulation run (20 * 0.5 = 10mS). */
    for (int s = 0; s < 20; s++) {
        for (size_t i = 0; i < ARRAY_SIZE(node_table); i++) {
            Node* n = &node_table[i];
            if ((rc = step(n, s)) != 0) break;
        }
        simbus(node_table, sizeof(node_table[0]), offsetof(Node, nc),
            ARRAY_SIZE(node_table));
    }

    /* Close the NCodec. */
    for (size_t i = 0; i < ARRAY_SIZE(node_table); i++) {
        Node* n = &node_table[i];
        ncodec_close(n->nc);
    }

    return 0;
}
