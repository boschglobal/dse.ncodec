// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_NCODEC_CODEC_AB_FLEXRAY_POP_FLEXRAY_POP_H_
#define DSE_NCODEC_CODEC_AB_FLEXRAY_POP_FLEXRAY_POP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <dse/ncodec/codec/ab/vector.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/schema/abs/stream/pdu_builder.h>

#define FLEXRAY_LOG_ID_LEN 20


typedef struct FlexrayPopBusModel {
    NCodecPduFlexrayNodeIdentifier node_ident;

    Vector pdu_router; /* NCodecPdu: indexed by node_ident. */
    char   log_id[FLEXRAY_LOG_ID_LEN];

    struct {
        /* Configuration items. */
        NCodecPduFlexrayBitrate bit_rate;
        uint32_t                microtick_per_cycle;
        uint32_t                macrotick_per_cycle;
        uint32_t                static_slot_length_mt;
        uint32_t                static_slot_count;

        /* Calculated configuration. */
        uint32_t step_budget_mt;
    } config;

    struct {
        /* Observed status. */
        NCodecPduFlexrayTransceiverState bus_condition;

        uint8_t cycle;
        bool    running; /* Set on the initial transition to FrameSync. */

        /* Calculated state. */
        uint32_t pos_mt;
        uint32_t pos_cycle;
    } status;

    /* Logging interface, duplicated from ABCodecBusModel. */
    ABCodecInstance* log_nc;
} FlexrayPopBusModel;


#endif  // DSE_NCODEC_CODEC_AB_FLEXRAY_POP_FLEXRAY_POP_H_
