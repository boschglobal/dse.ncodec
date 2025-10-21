// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdio.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>


bool flexray_bus_model_consume(ABCodecBusModel* bm, NCodecPdu* pdu)
{
    if (pdu->transport_type != NCodecPduTransportTypeFlexray) return false;

    FlexrayBusModel*               m = (FlexrayBusModel*)bm->model;
    NCodecPduFlexrayNodeIdentifier node_ident =
        pdu->transport.flexray.node_ident;

    switch (pdu->transport.flexray.metadata_type) {
    case (NCodecPduFlexrayMetadataTypeNone):
        /* No metadata content to decode. */
        break;
    case (NCodecPduFlexrayMetadataTypeConfig):
        log_debug(bm->log_nc, "FlexRay%s: Consume: (%u:%u:%u) Config",
            m->log_id, node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id);
        /* Ensure the Config has the node_ident of the PDU. */
        pdu->transport.flexray.metadata.config.node_ident = node_ident;
        process_config(m, pdu);
        log_debug(bm->log_nc, "Configure %d VCN (nid (%d:%d:%d))",
            pdu->transport.flexray.metadata.config.vcn_count,
            node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id);
        for (size_t i = 0;
             i < pdu->transport.flexray.metadata.config.vcn_count &&
             i < NCODEC_PDU_MAX_VCN;
             i++) {
            register_vcn_node_state(
                m, pdu->transport.flexray.metadata.config.vcn[i]);
        }
        register_node_state(m, node_ident, true, false);
        // TODO: map correct power state.
        set_poc_state(m, node_ident,
            pdu->transport.flexray.metadata.config.initial_poc_state_cha);
        break;
    case (NCodecPduFlexrayMetadataTypeStatus):
        log_debug(bm->log_nc, "FlexRay%s: Consume: (%u:%u:%u) Status",
            m->log_id, node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id);
        // TODO: state needs to be an array for CHA CHB
        push_node_state(m, node_ident,
            pdu->transport.flexray.metadata.status.channel[0].poc_command);

        // TODO: set the node power, does this need a specific command in the
        // Status message? set_node_power(state, checks[i].node, true);
        // shift_cycle(m, 0, 0, true); // TODO: FR sync from bridge.
        break;
    case (NCodecPduFlexrayMetadataTypeLpdu):
        log_info(bm->log_nc,
            "FlexRay%s: Consume: (%u:%u:%u) LPDU %04x index=%u, len=%u, "
            "status=%d",
            m->log_id, node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id, pdu->id,
            pdu->transport.flexray.metadata.lpdu.frame_config_index,
            pdu->payload_len, pdu->transport.flexray.metadata.lpdu.status);
        set_lpdu(m, node_ident.node_id, pdu->id,
            pdu->transport.flexray.metadata.lpdu.frame_config_index,
            pdu->transport.flexray.metadata.lpdu.status, pdu->payload,
            pdu->payload_len);
        break;
    default:
        log_error(bm->log_nc, "Unexpected FlexRay metadata type (%d)",
            pdu->transport.flexray.metadata_type);
    }

    return true;
}

void flexray_bus_model_progress(ABCodecBusModel* bm)
{
    FlexrayBusModel*               m = (FlexrayBusModel*)bm->model;
    NCodecPduFlexrayNodeIdentifier node_ident = m->node_ident;

    calculate_bus_condition(m);
    log_trace(bm->log_nc, "FlexRay%s: Progress: Bus Condition=%s", m->log_id,
        tcvr_state_string(m->state.bus_condition));

    if (m->state.bus_condition == NCodecPduFlexrayTransceiverStateFrameSync) {
// TODO: use simulation time from pdu.simulation_time
#define SIM_STEP_SIZE 0.0005
        int rc = calculate_budget(m, SIM_STEP_SIZE);
        if (rc == 0) {
            log_trace(bm->log_nc,
                "FlexRay%s: Progress: Pos (cycle=%u, slot=%u, mt=%u) "
                "Budget (mt=%u, ut=%u)",
                m->log_id, m->engine.pos_cycle, m->engine.pos_slot,
                m->engine.pos_mt, m->engine.step_budget_mt,
                m->engine.step_budget_ut);
            for (; consume_slot(m) == 0;) {
                log_trace(bm->log_nc,
                    "FlexRay%s: Progress: Pos (cycle=%u, slot=%u, mt=%u) "
                    "Budget (mt=%u, ut=%u)",
                    m->log_id, m->engine.pos_cycle, m->engine.pos_slot,
                    m->engine.pos_mt, m->engine.step_budget_mt,
                    m->engine.step_budget_ut);
            }
        } else {
            log_error(bm->log_nc, "Call to calculate_budget() returned %d", rc);
        }
    }

    FlexrayNodeState ns = get_node_state(m, node_ident);
    log_trace(bm->log_nc,
        "FlexRay%s: Progress (%u:%u:%u): poc=%u, tcvr=%u, cycle=%u, "
        "slot=%u, mt=%u (MT=%u, UT=%u, LPDU=%u)",
        m->log_id, node_ident.node.ecu_id, node_ident.node.cc_id,
        node_ident.node.swc_id, ns.poc_state, ns.tcvr_state,
        m->engine.pos_cycle, m->engine.pos_slot, m->engine.pos_mt,
        m->engine.step_budget_mt, m->engine.step_budget_ut,
        vector_len(&m->engine.txrx_list));
    log_trace(bm->log_nc,
        "FlexRay%s: Progress (%u:%u:%u): Status : poc_state=%s(%u), "
        "tcvr_state=%s(%u)",
        m->log_id, node_ident.node.ecu_id, node_ident.node.cc_id,
        node_ident.node.swc_id, poc_state_string(ns.poc_state), ns.poc_state,
        tcvr_state_string(ns.tcvr_state), ns.tcvr_state);
    ncodec_write((NCODEC*)bm->nc,
        &(NCodecPdu){ .ecu_id = m->node_ident.node.ecu_id,
            .swc_id = m->node_ident.node.swc_id,
            .transport_type = NCodecPduTransportTypeFlexray,
            .transport.flexray = { .node_ident = m->node_ident,
                .metadata_type = NCodecPduFlexrayMetadataTypeStatus,
                .metadata.status = {
                    .cycle = m->engine.pos_cycle,
                    .macrotick = m->engine.pos_mt,
                    .channel[0].poc_state = ns.poc_state,
                    .channel[0].tcvr_state = ns.tcvr_state,
                } } });

    for (size_t i = 0; i < vector_len(&m->engine.txrx_list); i++) {
        FlexrayLpdu* lpdu = NULL;
        vector_at(&m->engine.txrx_list, i, &lpdu);
        const uint8_t* payload = NULL;
        uint8_t        payload_len = 0;
        if (lpdu->lpdu_config.direction == NCodecPduFlexrayDirectionRx &&
            lpdu->null_frame == false) {
            payload = lpdu->payload;
            payload_len = lpdu->lpdu_config.payload_length;
        }
        NCodecPduFlexrayLpduStatus status = NCodecPduFlexrayLpduStatusNone;
        switch (lpdu->lpdu_config.status) {
        case NCodecPduFlexrayLpduStatusTransmitted:
        case NCodecPduFlexrayLpduStatusNotTransmitted:
            status = NCodecPduFlexrayLpduStatusTransmitted;
            break;
        case NCodecPduFlexrayLpduStatusReceived:
        case NCodecPduFlexrayLpduStatusNotReceived:
            if (lpdu->null_frame) {
                status = NCodecPduFlexrayLpduStatusNotReceived;
            } else {
                status = NCodecPduFlexrayLpduStatusReceived;
            }
            break;
        default:
            continue;
        }
        log_info(bm->log_nc,
            "FlexRay%s: Progress: (%u:%u:%u) LPDU %04x (len=%u) frame_index=%u "
            "status=%u, null=%u",
            m->log_id, node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id, lpdu->lpdu_config.slot_id, payload_len,
            lpdu->lpdu_config.index.frame_table, status, lpdu->null_frame);
        ncodec_write((NCODEC*)bm->nc,
            &(NCodecPdu){ .ecu_id = m->node_ident.node.ecu_id,
                .swc_id = m->node_ident.node.swc_id,
                .id = lpdu->lpdu_config.slot_id,
                .payload_len = payload_len,
                .payload = payload,
                .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = { .node_ident = m->node_ident,
                    .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                    .metadata.lpdu = {
                        .cycle = lpdu->cycle,
                        .macrotick = lpdu->macrotick,
                        .frame_config_index =
                            lpdu->lpdu_config.index.frame_table,
                        .status = status,
                        .null_frame = lpdu->null_frame,
                    } } });
    }
}

void flexray_bus_model_close(ABCodecBusModel* bm)
{
    FlexrayBusModel* m = (FlexrayBusModel*)bm->model;
    release_state(m);
    release_config(m);
}

void flexray_bus_model_create(ABCodecInstance* nc)
{
    /* Install the logging interface. */
    nc->reader.bus_model.log_nc = nc;

    /* Shallow copy the nc object. */
    ABCodecInstance* nc_copy = calloc(1, sizeof(ABCodecInstance));
    *nc_copy = *nc;
    nc_copy->c.stream = NULL;
    nc_copy->c.trace = (NCodecTraceVTable){ 0 };
    nc_copy->c.private = NULL;
    nc_copy->model = NULL;
    nc_copy->fbs_builder = (flatcc_builder_t){ 0 };
    nc_copy->fbs_builder_initalized = false;
    nc_copy->fbs_stream_initalized = false;
    nc_copy->reader = (ABCodecReader){ 0 };

/* Rebuild various objects in the model NC. */
#define BUFFER_LEN 1024
    flatcc_builder_init(&nc_copy->fbs_builder);
    nc_copy->fbs_builder.buffer_flags |= flatcc_builder_with_size;
    nc_copy->fbs_stream_initalized = false;
    nc_copy->fbs_builder_initalized = true;
    nc_copy->c.stream = ncodec_buffer_stream_create(BUFFER_LEN);

    /* Install the duplicated NC object. */
    nc->reader.bus_model.nc = nc_copy;

    /* Install the Bus Model object. */
    FlexrayBusModel* m = calloc(1, sizeof(FlexrayBusModel));
    m->log_nc = nc->reader.bus_model.log_nc;
    m->node_ident.node.ecu_id = nc->ecu_id;
    m->node_ident.node.cc_id = nc->cc_id;
    m->node_ident.node.swc_id = nc->swc_id;
    m->engine.node_ident = m->node_ident;
    snprintf(m->log_id, FLEXRAY_LOG_ID_LEN, "(%u:%u:%u)",
        m->node_ident.node.ecu_id, m->node_ident.node.cc_id,
        m->node_ident.node.swc_id);
    m->engine.log_id = m->log_id;
    m->vcn_count = nc->vcn_count;
    m->power_on = true;
    if (nc->pwr && strcmp(nc->pwr, "off")) {
        m->power_on = false;
    }
    nc->reader.bus_model.model = m;

    /* Configure the Bus Model VTable. */
    nc->reader.bus_model.vtable.consume = flexray_bus_model_consume;
    nc->reader.bus_model.vtable.progress = flexray_bus_model_progress;
    nc->reader.bus_model.vtable.close = flexray_bus_model_close;
}
