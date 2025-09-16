// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_CODEC_AB_FLEXRAY_FLEXRAY_H_
#define DSE_NCODEC_CODEC_AB_FLEXRAY_FLEXRAY_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/schema/abs/stream/pdu_builder.h>

typedef struct FlexrayNodeState {
    NCodecPduFlexrayNodeIdentifier node_ident;

    /* Per Node/NCodec instance. */
    NCodecPduFlexrayPocState         poc_state;
    NCodecPduFlexrayTransceiverState tcvr_state;
} FlexrayNodeState;


typedef struct FlexrayState {
    Vector node_state; /* FlexrayNodeState objects. */
    Vector vcs_node;   /* Virtual coldstart nodes. */

    /* The resultant bus_condition. */
    NCodecPduFlexrayTransceiverState bus_condition;
} FlexrayState;


typedef struct FlexrayEngine {
    NCodecPduFlexrayNodeIdentifier node_ident;
    bool                           inhibit_null_frames;

    double sim_step_size;

    uint32_t microtick_per_cycle;
    uint32_t macrotick_per_cycle;

    uint32_t static_slot_length_mt;
    uint32_t static_slot_count;
    uint32_t minislot_length_mt;
    uint32_t minislot_count;
    uint32_t static_slot_payload_length;

    uint32_t macro2micro;
    uint32_t microtick_ns;
    uint32_t macrotick_ns;
    uint32_t offset_static_mt;
    uint32_t offset_dynamic_mt;
    uint32_t offset_network_mt;
    uint32_t pos_mt;
    uint32_t pos_slot;
    uint8_t  pos_cycle;

    uint32_t step_budget_ut;
    uint32_t step_budget_mt;
    uint32_t bits_per_minislot;

    Vector slot_map;
    Vector txrx_list;
    Vector config_list; /* Storage for NCodecPduFlexrayLpduConfig tables. */
} FlexrayEngine;


typedef struct FlexrayLpdu {
    /* Status and config. */
    NCodecPduFlexrayNodeIdentifier node_ident;
    NCodecPduFlexrayLpduConfig     lpdu_config;

    /* Payload associated with this LPDU. */
    uint8_t* payload;

    /* Cycle of the last Tx/Rx for this LPDU. */
    uint8_t  cycle;
    uint16_t macrotick;

    /* Indicate if this LPDU represents a NULL frame. */
    bool null_frame;
} FlexrayLpdu;


typedef struct FlexrayBusModel {
    NCodecPduFlexrayNodeIdentifier node_ident;
    size_t                         vcn_count;
    bool                           power_on;

    FlexrayEngine engine;
    FlexrayState  state;  // FIXME: array for chA chB
} FlexrayBusModel;


int  process_config(NCodecPduFlexrayConfig* config, FlexrayEngine* engine);
int  calculate_budget(FlexrayEngine* engine, double step_size);
int  consume_slot(FlexrayEngine* engine);
void release_config(FlexrayEngine* engine);
int  shift_cycle(FlexrayEngine* engine, uint32_t mt, uint8_t cycle, bool force);
int  set_lpdu(FlexrayEngine* engine, uint64_t node_id, uint32_t slot_id,
     uint32_t frame_config_index, NCodecPduFlexrayLpduStatus status,
     const uint8_t* payload, size_t payload_len);

int process_poc_command(
    FlexrayNodeState* state, NCodecPduFlexrayPocCommand command);

void register_node_state(FlexrayState* state,
    NCodecPduFlexrayNodeIdentifier nid, bool pwr_on, bool pwr_off);
void register_vcn_node_state(
    FlexrayState* state, NCodecPduFlexrayNodeIdentifier nid);
void release_state(FlexrayState* state);
void push_node_state(FlexrayState* state, NCodecPduFlexrayNodeIdentifier nid,
    NCodecPduFlexrayPocCommand command);
void calculate_bus_condition(FlexrayState* state);
FlexrayNodeState get_node_state(
    FlexrayState* state, NCodecPduFlexrayNodeIdentifier nid);
void set_node_power(
    FlexrayState* state, NCodecPduFlexrayNodeIdentifier nid, bool power_on);
void set_poc_state(FlexrayState* state, NCodecPduFlexrayNodeIdentifier nid,
    NCodecPduFlexrayPocState poc_state);

const char* tcvr_state_string(unsigned int state);
const char* poc_state_string(unsigned int state);

/* fbs.c */
#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Stream_Pdu, x)
void decode_flexray_metadata(
    ns(Pdu_table_t) pdu, NCodecPdu* _pdu, Vector* free_list);
uint32_t emit_flexray_metadata(flatcc_builder_t* B, NCodecPdu* _pdu);
#undef ns


#endif  // DSE_NCODEC_CODEC_AB_FLEXRAY_FLEXRAY_H_
