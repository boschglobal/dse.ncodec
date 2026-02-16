// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/schema/abs/stream/pdu_builder.h>


#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Stream_Pdu, x)


static void _decode_flexray_config(
    ns(FlexrayMetadata_table_t) fr, NCodecPdu* _pdu, Vector* free_list)
{
    NCodecPduFlexrayConfig* c = &_pdu->transport.flexray.metadata.config;
    _pdu->transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeConfig;

    ns(FlexrayConfig_table_t) fc_msg =
        (ns(FlexrayConfig_table_t))ns(FlexrayMetadata_metadata(fr));

    /* Force the config node_ident. */
    c->node_ident = _pdu->transport.flexray.node_ident;

    flatbuffers_string_t name = ns(FlexrayConfig_node_name(fc_msg));
    if (flatbuffers_string_len(name) > 0) {
        strncpy(c->node_name, name, NCODEC_PDU_NODE_NAME_LEN - 1);
    }
    c->vcn_count =
        ns(FlexrayNodeIdentifier_vec_len(ns(FlexrayConfig_vcn(fc_msg))));
    // FIXME: decode this properly.
    c->vcn[0] = _pdu->transport.flexray.node_ident;
    c->vcn[1] = _pdu->transport.flexray.node_ident;
    c->vcn[0].node.swc_id = 1;
    c->vcn[1].node.swc_id = 2;
    c->initial_poc_state_cha = ns(FlexrayConfig_initial_poc_state_cha(fc_msg));
    c->initial_poc_state_chb = ns(FlexrayConfig_initial_poc_state_chb(fc_msg));
    c->inhibit_null_frames = ns(FlexrayConfig_inhibit_null_frames(fc_msg));
    c->macrotick_per_cycle = ns(FlexrayConfig_macrotick_per_cycle(fc_msg));
    c->microtick_per_cycle = ns(FlexrayConfig_microtick_per_cycle(fc_msg));
    c->network_idle_start = ns(FlexrayConfig_network_idle_start(fc_msg));
    c->static_slot_length = ns(FlexrayConfig_static_slot_length(fc_msg));
    c->static_slot_count = ns(FlexrayConfig_static_slot_count(fc_msg));
    c->minislot_length = ns(FlexrayConfig_minislot_length(fc_msg));
    c->minislot_count = ns(FlexrayConfig_minislot_count(fc_msg));
    c->static_slot_payload_length =
        ns(FlexrayConfig_static_slot_payload_length(fc_msg));

    c->bit_rate = ns(FlexrayConfig_bit_rate(fc_msg));
    c->channel_enable = ns(FlexrayConfig_channel_enable(fc_msg));

    c->coldstart_node = ns(FlexrayConfig_coldstart_node(fc_msg));
    c->sync_node = ns(FlexrayConfig_sync_node(fc_msg));
    c->coldstart_attempts = ns(FlexrayConfig_coldstart_attempts(fc_msg));
    c->wakeup_channel_select = ns(FlexrayConfig_wakeup_channel_select(fc_msg));
    c->single_slot_enabled = ns(FlexrayConfig_single_slot_enabled(fc_msg));
    c->key_slot_id = ns(FlexrayConfig_key_slot_id(fc_msg));

    c->frame_config.count =
        ns(FlexrayLpduConfig_vec_len(ns(FlexrayConfig_frame_table(fc_msg))));
    if (c->frame_config.count) {
        c->frame_config.table =
            calloc(c->frame_config.count, sizeof(NCodecPduFlexrayLpduConfig));
        if (free_list->capacity == 0) {
            *free_list = vector_make(sizeof(void*), 0, NULL);
        }
        vector_push(free_list, &c->frame_config.table);
        for (size_t i = 0; i < c->frame_config.count; i++) {
            NCodecPduFlexrayLpduConfig* lc = &c->frame_config.table[i];
            ns(FlexrayLpduConfig_table_t) lc_table =
                ns(FlexrayLpduConfig_vec_at(
                    ns(FlexrayConfig_frame_table(fc_msg)), i));

            lc->slot_id = ns(FlexrayLpduConfig_slot_id(lc_table));
            lc->payload_length = ns(FlexrayLpduConfig_payload_length(lc_table));
            lc->cycle_repetition =
                ns(FlexrayLpduConfig_cycle_repetition(lc_table));
            lc->base_cycle = ns(FlexrayLpduConfig_base_cycle(lc_table));
            lc->index.frame_table =
                ns(FlexrayLpduConfig_frame_table_index(lc_table));
            lc->index.lpdu_table =
                ns(FlexrayLpduConfig_lpdu_table_index(lc_table));
            lc->direction = ns(FlexrayLpduConfig_direction(lc_table));
            lc->channel = ns(FlexrayLpduConfig_channel(lc_table));
            lc->transmit_mode = ns(FlexrayLpduConfig_transmit_mode(lc_table));
            lc->status = ns(FlexrayLpduConfig_status(lc_table));
            lc->inhibit_null = ns(FlexrayLpduConfig_inhibit_null(lc_table));

            /* Correct cycle_repetition if repetition is 0. */
            if (lc->cycle_repetition == 0) {
                /* 0 or 1 possible, push to 1 to avoid divide-by-0 case. */
                lc->cycle_repetition = 1;
            }
            /* Correct base_cycle if repetition is 1. */
            if (lc->cycle_repetition == 1) {
                lc->base_cycle = 0;
            }
        }
    }
}

static void _decode_flexray_status(
    ns(FlexrayMetadata_table_t) fr, NCodecPdu* _pdu)
{
    NCodecPduFlexrayStatus* s = &_pdu->transport.flexray.metadata.status;
    _pdu->transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeStatus;

    ns(FlexrayStatus_table_t) fs_msg =
        (ns(FlexrayStatus_table_t))ns(FlexrayMetadata_metadata(fr));

    s->macrotick = ns(FlexrayStatus_macrotick(fs_msg));
    s->cycle = ns(FlexrayStatus_cycle(fs_msg));

    s->channel[0].tcvr_state = ns(FlexrayStatus_tcvr_state_cha(fs_msg));
    s->channel[0].poc_state = ns(FlexrayStatus_poc_state_cha(fs_msg));
    s->channel[0].poc_command = ns(FlexrayStatus_poc_command_cha(fs_msg));

    s->channel[1].tcvr_state = ns(FlexrayStatus_tcvr_state_chb(fs_msg));
    s->channel[1].poc_state = ns(FlexrayStatus_poc_state_chb(fs_msg));
    s->channel[1].poc_command = ns(FlexrayStatus_poc_command_chb(fs_msg));
}

static void _decode_flexray_lpdu(
    ns(FlexrayMetadata_table_t) fr, NCodecPdu* _pdu)
{
    NCodecPduFlexrayLpdu* l = &_pdu->transport.flexray.metadata.lpdu;
    _pdu->transport.flexray.metadata_type = NCodecPduFlexrayMetadataTypeLpdu;

    ns(FlexrayLpdu_table_t) fl_msg =
        (ns(FlexrayLpdu_table_t))ns(FlexrayMetadata_metadata(fr));

    l->cycle = ns(FlexrayLpdu_cycle(fl_msg));
    l->macrotick = ns(FlexrayLpdu_macrotick(fl_msg));
    l->frame_config_index = ns(FlexrayLpdu_frame_config_index(fl_msg));
    l->null_frame = ns(FlexrayLpdu_null_frame(fl_msg));
    l->sync_frame = ns(FlexrayLpdu_sync_frame(fl_msg));
    l->startup_frame = ns(FlexrayLpdu_startup_frame(fl_msg));
    l->payload_preamble = ns(FlexrayLpdu_payload_preamble(fl_msg));
    l->status = ns(FlexrayLpdu_status(fl_msg));
}


void decode_flexray_metadata(
    ns(Pdu_table_t) pdu, NCodecPdu* _pdu, Vector* free_list)
{
    NCodecPduFlexrayTransport* fr = &_pdu->transport.flexray;
    _pdu->transport_type = NCodecPduTransportTypeFlexray;

    ns(FlexrayMetadata_table_t) fr_msg =
        (ns(FlexrayMetadata_table_t))ns(Pdu_transport(pdu));
    fr->node_ident.node.ecu_id = ns(FlexrayMetadata_node_ident(fr_msg))->ecu_id;
    fr->node_ident.node.cc_id = ns(FlexrayMetadata_node_ident(fr_msg))->cc_id;
    fr->node_ident.node.swc_id = ns(FlexrayMetadata_node_ident(fr_msg))->swc_id;
    fr->pop_node_ident.node.ecu_id =
        ns(FlexrayMetadata_pop_node_ident(fr_msg))->ecu_id;
    fr->pop_node_ident.node.cc_id =
        ns(FlexrayMetadata_pop_node_ident(fr_msg))->cc_id;
    fr->pop_node_ident.node.swc_id =
        ns(FlexrayMetadata_pop_node_ident(fr_msg))->swc_id;
    ns(FlexrayMetadataType_union_type_t) metadata_type =
        ns(FlexrayMetadata_metadata_type(fr_msg));
    switch (metadata_type) {
    case ns(FlexrayMetadataType_Config):
        _decode_flexray_config(fr_msg, _pdu, free_list);
        break;
    case ns(FlexrayMetadataType_Status):
        _decode_flexray_status(fr_msg, _pdu);
        break;
    case ns(FlexrayMetadataType_Lpdu):
        _decode_flexray_lpdu(fr_msg, _pdu);
        break;
    }
}


static uint32_t _emit_flexray_config(flatcc_builder_t* B, NCodecPdu* _pdu)
{
    NCodecPduFlexrayConfig* c = &_pdu->transport.flexray.metadata.config;
    ns(FlexrayConfig_start(B));

    if (c->vcn_count > 0) {
        ns(FlexrayConfig_vcn_start(B));
        for (size_t i = 0; i < c->vcn_count && i < NCODEC_PDU_MAX_VCN; i++) {
            ns(FlexrayConfig_vcn_push_create(B, c->vcn[i].node.ecu_id,
                c->vcn[i].node.cc_id, c->vcn[i].node.swc_id));
        }
        ns(FlexrayConfig_vcn_end(B));
    }
    if (strlen(c->node_name)) {
        flatbuffers_string_vec_ref_t _node_name =
            flatbuffers_string_create_strn(
                B, c->node_name, NCODEC_PDU_NODE_NAME_LEN);
        ns(FlexrayConfig_node_name_add(B, _node_name));
    }
    ns(FlexrayConfig_initial_poc_state_cha_add(B, c->initial_poc_state_cha));
    ns(FlexrayConfig_initial_poc_state_chb_add(B, c->initial_poc_state_chb));
    ns(FlexrayConfig_inhibit_null_frames_add(B, c->inhibit_null_frames));
    ns(FlexrayConfig_macrotick_per_cycle_add(B, c->macrotick_per_cycle));
    ns(FlexrayConfig_microtick_per_cycle_add(B, c->microtick_per_cycle));
    ns(FlexrayConfig_network_idle_start_add(B, c->network_idle_start));
    ns(FlexrayConfig_static_slot_length_add(B, c->static_slot_length));
    ns(FlexrayConfig_static_slot_count_add(B, c->static_slot_count));
    ns(FlexrayConfig_minislot_length_add(B, c->minislot_length));
    ns(FlexrayConfig_minislot_count_add(B, c->minislot_count));
    ns(FlexrayConfig_static_slot_payload_length_add(
        B, c->static_slot_payload_length));

    ns(FlexrayConfig_bit_rate_add(B, c->bit_rate));
    ns(FlexrayConfig_channel_enable_add(B, c->channel_enable));

    ns(FlexrayConfig_coldstart_node_add(B, c->coldstart_node));
    ns(FlexrayConfig_sync_node_add(B, c->sync_node));
    ns(FlexrayConfig_coldstart_attempts_add(B, c->coldstart_attempts));
    ns(FlexrayConfig_wakeup_channel_select_add(B, c->wakeup_channel_select));
    ns(FlexrayConfig_single_slot_enabled_add(B, c->single_slot_enabled));
    ns(FlexrayConfig_key_slot_id_add(B, c->key_slot_id));

    ns(FlexrayConfig_config_op_add(B, c->operation));
    if (c->frame_config.count > 0) {
        ns(FlexrayConfig_frame_table_start(B));
        for (size_t i = 0; i < c->frame_config.count; i++) {
            NCodecPduFlexrayLpduConfig* lc = &c->frame_config.table[i];
            ns(FlexrayConfig_frame_table_push_start(B));
            ns(FlexrayLpduConfig_slot_id_add(B, lc->slot_id));
            ns(FlexrayLpduConfig_payload_length_add(B, lc->payload_length));
            ns(FlexrayLpduConfig_cycle_repetition_add(B, lc->cycle_repetition));
            ns(FlexrayLpduConfig_base_cycle_add(B, lc->base_cycle));
            ns(FlexrayLpduConfig_frame_table_index_add(
                B, lc->index.frame_table));
            ns(FlexrayLpduConfig_lpdu_table_index_add(B, lc->index.lpdu_table));
            ns(FlexrayLpduConfig_direction_add(B, lc->direction));
            ns(FlexrayLpduConfig_channel_add(B, lc->channel));
            ns(FlexrayLpduConfig_transmit_mode_add(B, lc->transmit_mode));
            ns(FlexrayLpduConfig_status_add(B, lc->status));
            ns(FlexrayLpduConfig_inhibit_null_add(B, lc->inhibit_null));
            ns(FlexrayConfig_frame_table_push_end(B));
        }
        ns(FlexrayConfig_frame_table_end(B));
    }

    return ns(FlexrayConfig_end(B));
}

static uint32_t _emit_flexray_status(flatcc_builder_t* B, NCodecPdu* _pdu)
{
    NCodecPduFlexrayStatus* s = &_pdu->transport.flexray.metadata.status;
    ns(FlexrayStatus_start(B));

    ns(FlexrayStatus_macrotick_add(B, s->macrotick));
    ns(FlexrayStatus_cycle_add(B, s->cycle));

    ns(FlexrayStatus_tcvr_state_cha_add(B, s->channel[0].tcvr_state));
    ns(FlexrayStatus_poc_state_cha_add(B, s->channel[0].poc_state));
    ns(FlexrayStatus_poc_command_cha_add(B, s->channel[0].poc_command));

    ns(FlexrayStatus_tcvr_state_chb_add(B, s->channel[1].tcvr_state));
    ns(FlexrayStatus_poc_state_chb_add(B, s->channel[1].poc_state));
    ns(FlexrayStatus_poc_command_chb_add(B, s->channel[1].poc_command));

    return ns(FlexrayStatus_end(B));
}

static uint32_t _emit_flexray_lpdu(flatcc_builder_t* B, NCodecPdu* _pdu)
{
    NCodecPduFlexrayLpdu* l = &_pdu->transport.flexray.metadata.lpdu;
    ns(FlexrayLpdu_start(B));

    ns(FlexrayLpdu_cycle_add(B, l->cycle));
    ns(FlexrayLpdu_macrotick_add(B, l->macrotick));
    ns(FlexrayLpdu_frame_config_index_add(B, l->frame_config_index));
    ns(FlexrayLpdu_null_frame_add(B, l->null_frame));
    ns(FlexrayLpdu_sync_frame_add(B, l->sync_frame));
    ns(FlexrayLpdu_startup_frame_add(B, l->startup_frame));
    ns(FlexrayLpdu_payload_preamble_add(B, l->payload_preamble));
    ns(FlexrayLpdu_status_add(B, l->status));

    return ns(FlexrayLpdu_end(B));
}


uint32_t emit_flexray_metadata(flatcc_builder_t* B, NCodecPdu* _pdu)
{
    NCodecPduFlexrayTransport* fr = &_pdu->transport.flexray;
    ns(FlexrayConfig_ref_t) config = 0;
    ns(FlexrayStatus_ref_t) status = 0;
    ns(FlexrayLpdu_ref_t) lpdu = 0;

    switch (fr->metadata_type) {
    case NCodecPduFlexrayMetadataTypeConfig:
        config = _emit_flexray_config(B, _pdu);
        break;
    case NCodecPduFlexrayMetadataTypeStatus:
        status = _emit_flexray_status(B, _pdu);
        break;
    case NCodecPduFlexrayMetadataTypeLpdu:
        lpdu = _emit_flexray_lpdu(B, _pdu);
        break;
    default:
        break;
    }

    ns(FlexrayMetadata_start(B));
    ns(FlexrayMetadata_node_ident_create(B, fr->node_ident.node.ecu_id,
        fr->node_ident.node.cc_id, fr->node_ident.node.swc_id));
    ns(FlexrayMetadata_pop_node_ident_create(B, fr->pop_node_ident.node.ecu_id,
        fr->pop_node_ident.node.cc_id, fr->pop_node_ident.node.swc_id));
    if (config) {
        ns(FlexrayMetadata_metadata_Config_add(B, config));
    } else if (status) {
        ns(FlexrayMetadata_metadata_Status_add(B, status));
    } else if (lpdu) {
        ns(FlexrayMetadata_metadata_Lpdu_add(B, lpdu));
    }
    return ns(FlexrayMetadata_end(B));
}
