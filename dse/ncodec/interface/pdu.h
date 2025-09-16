// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_INTERFACE_PDU_H_
#define DSE_NCODEC_INTERFACE_PDU_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/** NCODEC API - PDU/Stream
    =======================

    Types relating to the implementation of the Stream/PDU interface of
    the NCodec API.

    The root type is `NCodecPdu` which may be substituted for the
   `NCodecMessage` type when calling NCodec API methods (e.g. `ncodec_write()`).
*/


/** PDU : CAN Message/Frame Interface
    ---------------------------------
*/

typedef enum NCodecPduCanFrameFormat {
    NCodecPduCanFrameFormatBase = 0,
    NCodecPduCanFrameFormatExtended = 1,
    NCodecPduCanFrameFormatFdBase = 2,
    NCodecPduCanFrameFormatFdExtended = 3,
} NCodecPduCanFrameFormat;

typedef enum NCodecPduCanFrameType {
    NCodecPduCanFrameTypeData = 0,
    NCodecPduCanFrameTypeRemote = 1,
    NCodecPduCanFrameTypeError = 2,
    NCodecPduCanFrameTypeOverload = 3,
} NCodecPduCanFrameType;

typedef struct NCodecPduCanMessageMetadata {
    NCodecPduCanFrameFormat frame_format;
    NCodecPduCanFrameType   frame_type;
    uint32_t                interface_id;
    uint32_t                network_id;
} NCodecPduCanMessageMetadata;


/** PDU : IP Message/Frame Interface
    --------------------------------
*/

typedef enum {
    NCodecPduIpProtocolNone = 0,
    NCodecPduIpProtocolTcp = 6,
    NCodecPduIpProtocolUdp = 17,
} NCodecPduIpProtocol;

typedef enum {
    NCodecPduIpAddrNone = 0,
    NCodecPduIpAddrIPv4 = 1,
    NCodecPduIpAddrIPv6 = 2,
} NCodecPduIpAddr;

typedef enum {
    NCodecPduSoAdNone = 0,
    NCodecPduSoAdDoIP = 1,
    NCodecPduSoAdSomeIP = 2,
} NCodecPduSoAd;

typedef struct NCodecPduIpAddrV4 {
    uint32_t src_addr;
    uint32_t dst_addr;
} NCodecPduIpAddrV4;

typedef struct NCodecPduIpAddrV6 {
    uint16_t src_addr[8];
    uint16_t dst_addr[8];
} NCodecPduIpAddrV6;


typedef struct NCodecPduDoIpAdapter {
    uint8_t  protocol_version;
    uint16_t payload_type;
} NCodecPduDoIpAdapter;

typedef struct NCodecPduSomeIpAdapter {
    uint32_t message_id;
    uint32_t length;
    uint32_t request_id;
    uint8_t  protocol_version;
    uint8_t  interface_version;
    uint8_t  message_type;
    uint8_t  return_code;
} NCodecPduSomeIpAdapter;


typedef struct NCodecPduIpMessageMetadata {
    uint64_t eth_dst_mac;
    uint64_t eth_src_mac;
    uint16_t eth_ethertype;
    uint8_t  eth_tci_pcp;
    uint8_t  eth_tci_dei;
    uint16_t eth_tci_vid;

    NCodecPduIpProtocol ip_protocol;
    NCodecPduIpAddr     ip_addr_type;
    union {
        struct {
        } none;
        NCodecPduIpAddrV4 ip_v4;
        NCodecPduIpAddrV6 ip_v6;
    } ip_addr;
    uint16_t ip_src_port;
    uint16_t ip_dst_port;

    NCodecPduSoAd so_ad_type;
    union {
        struct {
        } none;
        NCodecPduDoIpAdapter   do_ip;
        NCodecPduSomeIpAdapter some_ip;
    } so_ad;
} NCodecPduIpMessageMetadata;


/** PDU : Struct Message Interface
    ------------------------------
*/

typedef struct NCodecPduStructMetadata {
    const char* type_name;
    const char* var_name;
    const char* encoding;
    uint16_t    attribute_aligned;
    bool        attribute_packed;
    const char* platform_arch;
    const char* platform_os;
    const char* platform_abi;
} NCodecPduStructMetadata;


/** PDU : FlexRay Message/Frame Interface
    -------------------------------------
*/

#define MAX_VCN 2

typedef struct NCodecPduFlexrayNodeIdentifier {
    union {
        uint64_t node_id;
        struct {
            uint16_t ecu_id;
            uint16_t cc_id;
            uint32_t swc_id;
        } node;
    };
} NCodecPduFlexrayNodeIdentifier;

typedef enum {
    NCodecPduFlexrayBitrateNone = 0, /* No Config. */
    NCodecPduFlexrayBitrate10 = 1,
    NCodecPduFlexrayBitrate5 = 2,
    NCodecPduFlexrayBitrate2_5 = 3,
} NCodecPduFlexrayBitrate;

typedef enum {
    NCodecPduFlexrayMicroTickNone = 0,
    NCodecPduFlexrayMicroTickNs10 = 25,
    NCodecPduFlexrayMicroTickNs5 = 25,
    NCodecPduFlexrayMicroTickNs2_5 = 50,
} NCodecPduFlexrayMicroTickNs;

static const uint8_t flexray_microtick_ns[] = {
    [NCodecPduFlexrayBitrateNone] = NCodecPduFlexrayMicroTickNone,
    [NCodecPduFlexrayBitrate10] = NCodecPduFlexrayMicroTickNs10,
    [NCodecPduFlexrayBitrate5] = NCodecPduFlexrayMicroTickNs5,
    [NCodecPduFlexrayBitrate2_5] = NCodecPduFlexrayMicroTickNs2_5,
};

static const uint16_t flexray_bittime_ns[] = {
    [NCodecPduFlexrayBitrateNone] = 1, /* Not used, safe value. */
    [NCodecPduFlexrayBitrate10] = 100,
    [NCodecPduFlexrayBitrate5] = 200,
    [NCodecPduFlexrayBitrate2_5] = 400,
};

typedef enum {
    NCodecPduFlexrayDirectionNone = 0,
    NCodecPduFlexrayDirectionRx = 1,
    NCodecPduFlexrayDirectionTx = 2,
} NCodecPduFlexrayDirection;

typedef enum {
    NCodecPduFlexrayTransmitModeNone = 0,
    NCodecPduFlexrayTransmitModeContinuous = 1,
    NCodecPduFlexrayTransmitModeSingleShot = 2,
} NCodecPduFlexrayTransmitMode;

typedef enum {
    NCodecPduFlexrayChannelNone = 0,
    NCodecPduFlexrayChannelA = 1,
    NCodecPduFlexrayChannelB = 2,
    NCodecPduFlexrayChannelAB = 3,
} NCodecPduFlexrayChannel;

typedef enum {
    NCodecPduFlexrayChannelStatusA = 0,
    NCodecPduFlexrayChannelStatusB = 1,
    NCodecPduFlexrayChannelStatusSize = 2,
} NCodecPduFlexrayChannelStatus;

typedef enum {
    NCodecPduFlexrayTransceiverStateNone = 0,
    NCodecPduFlexrayTransceiverStateNoPower = 1,
    NCodecPduFlexrayTransceiverStateNoConnection = 2,
    NCodecPduFlexrayTransceiverStateNoSignal = 3,
    NCodecPduFlexrayTransceiverStateCAS = 4,
    NCodecPduFlexrayTransceiverStateWUP = 5,
    NCodecPduFlexrayTransceiverStateFrameSync = 6,
    NCodecPduFlexrayTransceiverStateFrameError = 7,
} NCodecPduFlexrayTransceiverState;

typedef enum {
    NCodecPduFlexrayPocStateDefaultConfig = 0, /* WUP detection only. */
    NCodecPduFlexrayPocStateConfig = 1,
    NCodecPduFlexrayPocStateReady = 2,
    NCodecPduFlexrayPocStateWakeup = 3,
    NCodecPduFlexrayPocStateStartup = 4,
    NCodecPduFlexrayPocStateNormalActive = 5,  /* Synchronized, active. */
    NCodecPduFlexrayPocStateNormalPassive = 6, /* Synchronize failed. */
    NCodecPduFlexrayPocStateHalt = 7,
    NCodecPduFlexrayPocStateFreeze = 8,
    NCodecPduFlexrayPocStateUndefined = 9,
} NCodecPduFlexrayPocState;

typedef enum {
    NCodecPduFlexrayCommandNone = 0,
    NCodecPduFlexrayCommandConfig = 1,
    NCodecPduFlexrayCommandReady = 2,
    NCodecPduFlexrayCommandWakeup = 3,
    NCodecPduFlexrayCommandRun = 4,
    NCodecPduFlexrayCommandAllSlots = 5,
    NCodecPduFlexrayCommandHalt = 6,
    NCodecPduFlexrayCommandFreeze = 7,
    NCodecPduFlexrayCommandAllowColdstart = 8,
    NCodecPduFlexrayCommandNop = 9,
} NCodecPduFlexrayPocCommand;

typedef enum {
    NCodecPduFlexrayLpduStatusNone = 0,
    NCodecPduFlexrayLpduStatusTransmitted = 1,
    NCodecPduFlexrayLpduStatusNotTransmitted = 2,
    NCodecPduFlexrayLpduStatusReceived = 3,
    NCodecPduFlexrayLpduStatusNotReceived = 4,
} NCodecPduFlexrayLpduStatus;

typedef enum {
    NCodecPduFlexrayLpduConfigSet = 0,
    NCodecPduFlexrayLpduConfigFrameTableSet = 1,
    NCodecPduFlexrayLpduConfigFrameTableMerge = 2,
    NCodecPduFlexrayLpduConfigFrameTableDelete = 3,
} NCodecPduFlexrayConfigOp;

typedef struct NCodecPduFlexrayLpdu {
    /* Header (id/payload in NCodecPdu). */
    uint8_t cycle; /* 0..63 */

    /* Config index. */
    uint16_t frame_config_index;  // TODO: test coverage on this.

    /* Header Indicators. */
    bool null_frame;
    bool sync_frame;
    bool startup_frame;
    bool payload_preamble;

    /* Status update (to/from NCodecPduFlexrayLpduConfig). */
    NCodecPduFlexrayLpduStatus status;

    /* Testing support. */
    uint16_t macrotick;
} NCodecPduFlexrayLpdu;

typedef struct NCodecPduFlexrayLpduConfig {
    /* Communication Cycle parameters. */
    uint16_t slot_id;          /* 1..2047 */
    uint8_t  payload_length;   /* 0..254 */
    uint8_t  cycle_repetition; /* 0..63 */
    uint8_t  base_cycle;       /* 0..63 */

    /* Indexes. */
    struct {
        uint16_t frame_table;
        uint16_t lpdu_table; /* Controller internal only! */
    } index;

    /* Operational Fields. */
    NCodecPduFlexrayDirection    direction;
    NCodecPduFlexrayChannel      channel;
    NCodecPduFlexrayTransmitMode transmit_mode;
    NCodecPduFlexrayLpduStatus   status;
    bool                         inhibit_null;
} NCodecPduFlexrayLpduConfig;


typedef struct NCodecPduFlexrayConfig {
    /* Node Config items (Codec internal). */
    NCodecPduFlexrayNodeIdentifier node_ident;
    NCodecPduFlexrayNodeIdentifier vcn[MAX_VCN];
    size_t                         vcn_count;
    NCodecPduFlexrayPocState       initial_poc_state_cha;
    NCodecPduFlexrayPocState       initial_poc_state_chb;
    bool                           inhibit_null_frames;

    /* Config update operation. */
    NCodecPduFlexrayConfigOp operation;

    /* Communication Cycle Config. */
    uint16_t macrotick_per_cycle;        /* 10..16000 MT */
    uint32_t microtick_per_cycle;        /* 640..640000 uT */
    uint16_t network_idle_start;         /* 7..15997 MT */
    uint16_t static_slot_length;         /* 4..659 MT */
    uint16_t static_slot_count;          /* 2..1023 */
    uint8_t  minislot_length;            /* 2..63 MT */
    uint16_t minislot_count;             /* 0..7986 */
    uint32_t static_slot_payload_length; /* 0..254 */

    NCodecPduFlexrayBitrate bit_rate;
    NCodecPduFlexrayChannel channel_enable;

    /* Codestart & Sync Config. */
    bool           coldstart_node;
    bool           sync_node;
    uint8_t        coldstart_attempts;    /* 2..31 */
    uint8_t        wakeup_channel_select; /* 0=A, 1=B */
    bool           single_slot_enabled;   /* If true then set false by command
                                             NCodecPduFlexrayCommandAllSlots. */
    uint16_t       key_slot_id;
    const uint8_t* key_slot_payload;
    size_t         key_slot_payload_len;
    NCodecPduFlexrayLpdu* key_slot_lpdu;
    // TODO wakeup pattern(33)? symbol
    // TX_Idle(180)/TX_low(60)/Rx_Idle(40)/Rx_Low(40)

    /* Frame Config. */
    struct {
        size_t                      count;
        NCodecPduFlexrayLpduConfig* table;
    } frame_config;
} NCodecPduFlexrayConfig;

typedef struct NCodecPduFlexrayStatus {
    /* Communication Cycle. */
    uint16_t macrotick;
    uint8_t  cycle;

    /* Channel Status ([0] == CH_A, [1] == CH_B). */
    struct {
        NCodecPduFlexrayTransceiverState tcvr_state;
        NCodecPduFlexrayPocState         poc_state;
        /* Command interface (from controller). */
        NCodecPduFlexrayPocCommand       poc_command;
    } channel[NCodecPduFlexrayChannelStatusSize];
} NCodecPduFlexrayStatus;

typedef enum {
    NCodecPduFlexrayMetadataTypeNone = 0,
    NCodecPduFlexrayMetadataTypeConfig = 1,
    NCodecPduFlexrayMetadataTypeStatus = 2,
    NCodecPduFlexrayMetadataTypeLpdu = 3,
} NCodecPduFlexrayMetadataType;

typedef struct NCodecPduFlexrayTransport {
    NCodecPduFlexrayNodeIdentifier node_ident;
    NCodecPduFlexrayMetadataType   metadata_type;
    union {
        struct {
        } none;
        NCodecPduFlexrayLpdu   lpdu;
        NCodecPduFlexrayConfig config;
        NCodecPduFlexrayStatus status;
    } metadata;
} NCodecPduFlexrayTransport;

typedef enum {
    NCodecPduTransportTypeNone = 0,
    NCodecPduTransportTypeCan = 1,
    NCodecPduTransportTypeIp = 2,
    NCodecPduTransportTypeStruct = 3,
    NCodecPduTransportTypeFlexray = 4,
} NCodecPduTransportType;

typedef struct NCodecPdu {
    uint32_t       id;
    const uint8_t* payload;
    size_t         payload_len;

    /* Sender identifying properties (optional), default values are taken
       from the stream MIME Type parameters. */
    uint16_t ecu_id;
    uint32_t swc_id;

    /* Transport Metadata. */
    // TODO: Update types here for consistency (i.e. ...Transport).
    NCodecPduTransportType transport_type;
    union {
        struct {
        } none;
        NCodecPduCanMessageMetadata can_message;
        NCodecPduIpMessageMetadata  ip_message;
        NCodecPduStructMetadata     struct_object;
        NCodecPduFlexrayTransport
            flexray;  // TODO: should the schema table names change ?
    } transport;

    /* Simulation Metadata. */
    double simulation_time;
    double pdu_time;
} NCodecPdu;

#endif  // DSE_NCODEC_INTERFACE_PDU_H_
