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


typedef struct FlexRayState {
    NCodecPduFlexrayPocState poc_state;
} FlexRayState;


typedef struct FlexRayEngine {
    NCodecPduFlexrayNodeIdentifier node_ident;

    double sim_step_size;

    uint32_t microtick_per_cycle;
    uint32_t macrotick_per_cycle;

    uint32_t static_slot_length_mt;
    uint32_t static_slot_count;
    uint32_t minislot_length_mt;
    uint32_t minislot_count;
    uint32_t static_slot_payload_length;

    // transmit_mode / channels_enable

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

    struct {
        NCodecPduFlexrayLpdu* table;
        uint32_t              count;
    } frame_config;

    Vector slot_map;
    Vector txrx_list;
} FlexRayEngine;


typedef struct FlexRayLpdu {
    /* Status and config. */
    NCodecPduFlexrayNodeIdentifier node_ident;
    NCodecPduFlexrayLpduConfig     lpdu_config;

    /* Payload associated with this LPDU. */
    uint8_t* payload;
} FlexRayLpdu;


int  process_config(NCodecPduFlexrayConfig* config, FlexRayEngine* engine);
int  calculate_budget(FlexRayEngine* engine, double step_size);
int  consume_slot(FlexRayEngine* engine);
void release_config(FlexRayEngine* engine);
int  shift_cycle(FlexRayEngine* engine, uint32_t mt, uint8_t cycle, bool force);
int  set_payload(FlexRayEngine* engine, uint64_t node_id, uint32_t slot_id,
     NCodecPduFlexrayLpduStatus status, uint8_t* payload, size_t payload_len);
int process_poc_command(FlexRayState* state, NCodecPduFlexrayPocCommand command);

#endif  // DSE_NCODEC_CODEC_AB_FLEXRAY_FLEXRAY_H_
