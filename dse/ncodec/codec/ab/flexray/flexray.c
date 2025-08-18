// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <dse/logger.h>
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
        log_debug("FlexRay: Consume: (%u:%u:%u) Config", node_ident.node.ecu_id,
            node_ident.node.cc_id, node_ident.node.swc_id);
        /* Ensure the Config has the node_ident of the PDU. */
        pdu->transport.flexray.metadata.config.node_ident = node_ident;
        process_config(&pdu->transport.flexray.metadata.config, &m->engine);
        for (size_t i = 0;
             i < pdu->transport.flexray.metadata.config.vcn_count &&
             i < MAX_VCN;
             i++) {
            register_vcs_node_state(
                &m->state, pdu->transport.flexray.metadata.config.vcn[i]);
        }
        register_node_state(&m->state, node_ident, true,
            false);  // TODO: map correct power state.
        break;
    case (NCodecPduFlexrayMetadataTypeStatus):
        log_debug("FlexRay: Consume: (%u:%u:%u) Status", node_ident.node.ecu_id,
            node_ident.node.cc_id, node_ident.node.swc_id);
        // TODO: state needs to be an array for CHA CHB
        push_node_state(&m->state, node_ident,
            pdu->transport.flexray.metadata.status.channel[0].poc_command);

        // TODO: set the node power, does this need a specific command in the
        // Status message? set_node_power(state, checks[i].node, true);
        // shift_cycle(&m->engine, 0, 0, true); // TODO: FR sync from bridge.
        break;
    case (NCodecPduFlexrayMetadataTypeLpdu):
        log_debug("FlexRay: Consume: (%u:%u:%u) LPDU %04x (len=%u)",
            node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id, pdu->id, pdu->payload_len);
        // TODO: Need also the config index. Currently based on slot_id
        // TODO: but needs to also have config_index, or actually use
        // TODO: config_index for the lookup (rather than slot_id).
        // TODO: *** NOT URGENT ***, Needed for CHA + B support.
        set_payload(&m->engine, node_ident.node_id, pdu->id, 0, pdu->payload,
            pdu->payload_len);
        break;
    default:
        log_error("Unexpected FlexRay metadata type (%d)",
            pdu->transport.flexray.metadata_type);
    }

    return true;
}

void Flexray_bus_model_progress(ABCodecBusModel* bm)
{
    FlexrayBusModel*               m = (FlexrayBusModel*)bm->model;
    NCodecPduFlexrayNodeIdentifier node_ident = m->node_ident;

    log_debug("FlexRay: Progress: Bus Condition");
    calculate_bus_condition(&m->state);

    if (m->state.bus_condition == NCodecPduFlexrayTransceiverStateFrameSync) {
        log_debug("FlexRay: Progress: Calculate and Consume slots");
// TODO: use simulation time from pdu.simulation_time
#define SIM_STEP_SIZE 0.0005
        calculate_budget(&m->engine, SIM_STEP_SIZE);
        for (; consume_slot(&m->engine) == 0;) {
        }
    }

    log_debug("FlexRay: Progress: Status");
    FlexrayNodeState ns = get_node_state(&m->state, node_ident);
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

    log_debug("FlexRay: Progress: LPDUs");
    for (size_t i = 0; i < vector_len(&m->engine.txrx_list); i++) {
        FlexrayLpdu* lpdu = NULL;
        vector_at(&m->engine.txrx_list, i, &lpdu);
        log_debug(
            "FlexRay: Progress: LPDU %04x (len=%u) frame_index=%u status=%u",
            lpdu->lpdu_config.slot_id, lpdu->lpdu_config.payload_length,
            lpdu->lpdu_config.index.frame_table, lpdu->lpdu_config.status);
        ncodec_write((NCODEC*)bm->nc,
            &(NCodecPdu){ .ecu_id = m->node_ident.node.ecu_id,
                .swc_id = m->node_ident.node.swc_id,
                .id = lpdu->lpdu_config.slot_id,
                .payload_len = lpdu->lpdu_config.payload_length,
                .payload = lpdu->payload,
                .transport_type = NCodecPduTransportTypeFlexray,
                .transport.flexray = { .node_ident = m->node_ident,
                    .metadata_type = NCodecPduFlexrayMetadataTypeLpdu,
                    .metadata.lpdu = {
                        .cycle = lpdu->cycle,
                        .frame_config_index =
                            lpdu->lpdu_config.index.frame_table,
                        .status = lpdu->lpdu_config.status,
                    } } });
    }
}

void flexray_bus_model_close(ABCodecBusModel* bm)
{
    FlexrayBusModel* m = (FlexrayBusModel*)bm->model;
    release_state(&m->state);
    release_config(&m->engine);
}

void flexray_bus_model_create(ABCodecInstance* nc)
{
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
    FlexrayBusModel* bm = calloc(1, sizeof(FlexrayBusModel));
    bm->node_ident.node.ecu_id = nc->ecu_id;
    bm->node_ident.node.cc_id = nc->cc_id;
    bm->node_ident.node.swc_id = nc->swc_id;
    bm->engine.node_ident = bm->node_ident;

    bm->vcn_count = nc->vcn_count;
    bm->power_on = true;
    if (nc->pwr && strcmp(nc->pwr, "off")) {
        bm->power_on = false;
    }
    nc->reader.bus_model.model = bm;

    /* Configure the Bus Model VTable. */
    nc->reader.bus_model.vtable.consume = flexray_bus_model_consume;
    nc->reader.bus_model.vtable.progress = Flexray_bus_model_progress;
    nc->reader.bus_model.vtable.close = flexray_bus_model_close;
}
