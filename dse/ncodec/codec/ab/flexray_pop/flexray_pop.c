// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <dse/logger.h>
#include <dse/ncodec/stream/stream.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/codec/ab/flexray_pop/flexray_pop.h>


#define UNUSED(x) ((void)x)


typedef NCodecPduFlexrayNodeIdentifier NodeIdentifier;

typedef struct VectorPduRouteItem {
    NodeIdentifier node_ident;
    Vector         pdu_list; /* NCodecPdu */
} VectorPduRouteItem;


static int __node_ident_compar(const void* left, const void* right)
{
    NodeIdentifier l = ((VectorPduRouteItem*)left)->node_ident;
    NodeIdentifier r = ((VectorPduRouteItem*)right)->node_ident;

    if (l.node_id < r.node_id) return -1;
    if (l.node_id > r.node_id) return 1;
    return 0;
}

static int __pdu_status_first(const void* left, const void* right)
{
    NCodecPdu* l = (NCodecPdu*)left;
    NCodecPdu* r = (NCodecPdu*)right;

    if (l->transport.flexray.metadata_type ==
        r->transport.flexray.metadata_type)
        return 0;
    if (l->transport.flexray.metadata_type ==
        NCodecPduFlexrayMetadataTypeStatus)
        return -1;
    return 1;
}

static void __pdu_payload_destory(void* item, void* data)
{
    UNUSED(data);

    NCodecPdu* pdu = item;
    free((uint8_t*)pdu->payload);
    pdu->payload = NULL;
}

static void __pdu_list_clear(void* item, void* data)
{
    UNUSED(data);
    VectorPduRouteItem* pdu_route = item;

    // vector_clear(&pdu_route->pdu_list, __pdu_payload_destory, NULL);
    vector_clear(&pdu_route->pdu_list, NULL, NULL);
}

static void __pdu_list_destory(void* item, void* data)
{
    UNUSED(data);
    VectorPduRouteItem* pdu_route = item;

    vector_clear(&pdu_route->pdu_list, __pdu_payload_destory, NULL);
    vector_reset(&pdu_route->pdu_list);
}

static void __pdu_router_clear(FlexrayPopBusModel* m)
{
    for (size_t i = 0; i < vector_len(&m->pdu_router); i++) {
        void* item = vector_at(&m->pdu_router, i, NULL);
        __pdu_list_clear(item, NULL);
    }
}

static void __pdu_router_destroy(FlexrayPopBusModel* m)
{
    vector_clear(&m->pdu_router, __pdu_list_destory, NULL);
    vector_reset(&m->pdu_router);
}

static VectorPduRouteItem* __pdu_router_ensure_route(
    FlexrayPopBusModel* m, NodeIdentifier node_ident)
{
    VectorPduRouteItem* route = vector_find(&m->pdu_router,
        &(VectorPduRouteItem){ .node_ident = node_ident }, 0, NULL);
    if (route == NULL) {
        vector_push(&m->pdu_router,
            &(VectorPduRouteItem){
                .node_ident = node_ident,
                .pdu_list =
                    vector_make(sizeof(NCodecPdu), 0, __pdu_status_first),
            });
        vector_sort(&m->pdu_router);
        route = vector_find(&m->pdu_router,
            &(VectorPduRouteItem){ .node_ident = node_ident }, 0, NULL);
    }
    assert(route);
    return (route);
}

static void __pdu_router_push(FlexrayPopBusModel* m, NodeIdentifier node_ident,
    NCodecPdu* pdu, const char* msg)
{
    /* Create the origin & destination routes (the reverse/return route). */
    NCodecPduFlexrayNodeIdentifier orig_node_ident =
        pdu->transport.flexray.node_ident;
    VectorPduRouteItem* orig_pdu_route =
        __pdu_router_ensure_route(m, orig_node_ident);
    VectorPduRouteItem* pdu_route = __pdu_router_ensure_route(m, node_ident);

    /* Push the PDU to the destination route. */
    log_info("POP:Route: (%u:%u:%u) -[%s]-> (%u:%u:%u)",
        orig_node_ident.node.ecu_id, orig_node_ident.node.cc_id,
        orig_node_ident.node.swc_id, msg, node_ident.node.ecu_id,
        node_ident.node.cc_id, node_ident.node.swc_id);
    vector_push(&pdu_route->pdu_list, pdu);  // FIXME: deep copy?
}


bool flexray_pop_bus_model_consume(ABCodecBusModel* bm, NCodecPdu* pdu)
{
    FlexrayPopBusModel* m = (FlexrayPopBusModel*)bm->model;

    if (pdu->transport_type != NCodecPduTransportTypeFlexray) return false;

    /* Determine the intended node_ident for this message.*/
    NCodecPduFlexrayNodeIdentifier node_ident =
        pdu->transport.flexray.node_ident;
    NCodecPduFlexrayNodeIdentifier pop_node_ident =
        pdu->transport.flexray.pop_node_ident;

    /* Consume the message. */
    switch (pdu->transport.flexray.metadata_type) {
    case (NCodecPduFlexrayMetadataTypeNone):
        /* No metadata content to decode. */
        break;
    case (NCodecPduFlexrayMetadataTypeConfig):
        log_debug("FlexRay%s: Consume: (%u:%u:%u/%u:%u:%u) Config", m->log_id,
            node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id, pop_node_ident.node.ecu_id,
            pop_node_ident.node.cc_id, pop_node_ident.node.swc_id);
        if (node_ident.node_id != 0) {
            /* Node -> : route to PoP . */
            __pdu_router_push(
                m, (NodeIdentifier){ .node_id = 0 }, pdu, "Config");
        } else {
            /* PoP -> : extract config and discard. */
// FIXME: step_size is a parameter.
#define SIM_STEP_SIZE 0.0005
            NCodecPduFlexrayConfig* config =
                &pdu->transport.flexray.metadata.config;
            m->config.bit_rate = config->bit_rate;
            m->config.microtick_per_cycle = config->microtick_per_cycle;
            m->config.macrotick_per_cycle = config->macrotick_per_cycle;
            m->config.static_slot_length_mt = config->static_slot_length;
            m->config.static_slot_count = config->static_slot_count;

            if (m->config.bit_rate != NCodecPduFlexrayBitrate10) {
                log_notice("Bit rate not supported (%u)", m->config.bit_rate);
                break;
            }

            if (m->config.microtick_per_cycle == 0 ||
                m->config.macrotick_per_cycle == 0 ||
                m->config.static_slot_length_mt == 0 ||
                m->config.static_slot_count == 0) {
                log_notice("Partial configuration of PoP, macrotick estimation "
                           "not available.");
                break;
            }

            uint32_t macro2micro =
                m->config.microtick_per_cycle / m->config.macrotick_per_cycle;
            uint32_t microtick_ns = flexray_microtick_ns[m->config.bit_rate];
            uint32_t step_budget_ut =
                (SIM_STEP_SIZE * 1000000000) / microtick_ns;
            m->config.step_budget_mt = step_budget_ut / macro2micro;

            log_info(
                "PoP step width in Macrotick: %u", m->config.step_budget_mt);
        }
        break;
    case (NCodecPduFlexrayMetadataTypeStatus):
        log_debug("FlexRay%s: Consume: (%u:%u:%u/%u:%u:%u) Status", m->log_id,
            node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id, pop_node_ident.node.ecu_id,
            pop_node_ident.node.cc_id, pop_node_ident.node.swc_id);
        if (node_ident.node_id == 0) {
            if (pop_node_ident.node_id == 0) {
                NCodecPduFlexrayStatus* status =
                    &pdu->transport.flexray.metadata.status;
                /* PoP -> : terminate on Bus Model. */
                /* Force status if Bus Condition is not FrameSync*/
                if (status->channel[0].tcvr_state !=
                    NCodecPduFlexrayTransceiverStateFrameSync) {
                    // TODO: Consider WUP signal on tcvr?
                    m->status.running = false;
                    m->status.pos_cycle = 0;
                    m->status.pos_mt = 0;
                    log_debug("FlexRay%s: PoP Status: pos_cycle=%u pos_mt=%u",
                        m->log_id, m->status.pos_cycle, m->status.pos_mt);
                    break;
                }
                /* Set Macrotick. */
                if (status->macrotick != 0) {
                    /* Always take the provided Macrotick. */
                    m->status.pos_mt = status->macrotick;
                } else {
                    m->status.pos_mt += m->config.step_budget_mt;
                    if (m->status.pos_mt > m->config.macrotick_per_cycle) {
                        m->status.pos_mt = m->config.macrotick_per_cycle;
                    }
                }
                /* Evaluate the cycle and correct macrotick. */
                if (status->cycle != m->status.pos_cycle ||
                    m->status.running == false) {
                    m->status.pos_cycle = status->cycle;
                    if (status->macrotick == 0) {
                        /* Only adjust if microtick not provided. */
                        m->status.pos_mt = 0;
                    }
                    m->status.running = true;
                }
                log_debug("FlexRay%s: PoP Status: pos_cycle=%u pos_mt=%u",
                    m->log_id, m->status.pos_cycle, m->status.pos_mt);
            } else {
                /* PoP -> : route to node. */
                __pdu_router_push(m, pop_node_ident, pdu, "Status");
            }
        } else {
            /* Node -> : route to PoP. */
            __pdu_router_push(
                m, (NodeIdentifier){ .node_id = 0 }, pdu, "Status");
        }
        break;
    case (NCodecPduFlexrayMetadataTypeLpdu):
        log_debug("FlexRay%s: Consume: (%u:%u:%u/%u:%u:%u) LPDU %04x index=%u, "
                  "len=%u, "
                  "status=%d",
            m->log_id, node_ident.node.ecu_id, node_ident.node.cc_id,
            node_ident.node.swc_id, pop_node_ident.node.ecu_id,
            pop_node_ident.node.cc_id, pop_node_ident.node.swc_id, pdu->id,
            pdu->transport.flexray.metadata.lpdu.frame_config_index,
            pdu->payload_len, pdu->transport.flexray.metadata.lpdu.status);
        if (node_ident.node_id != 0) {
            /* Node -> : route to PoP . */
            __pdu_router_push(m, (NodeIdentifier){ .node_id = 0 }, pdu, "LPDU");
        } else if (pop_node_ident.node_id != 0) {
            /* PoP -> : route to node. */
            __pdu_router_push(m, pop_node_ident, pdu, "LPDU");
        }

        /* Extract Macrotick from Static LPDUs. */
        NCodecPduFlexrayLpdu* lpdu = &pdu->transport.flexray.metadata.lpdu;
        if (pop_node_ident.node_id != 0 &&
            lpdu->status == NCodecPduFlexrayLpduStatusTransmitted) {
            /* Intercept Tx LPDU from PoP to node. */
            if (pdu->id > m->config.static_slot_count) {
                /* In the dynamic part, ensure Macrotick is pushed to at least
                the start of the dynamic part. */
                uint32_t start_dynamic_mt = (m->config.static_slot_count + 1) *
                                            m->config.static_slot_length_mt;
                if (start_dynamic_mt > m->status.pos_mt) {
                    m->status.pos_mt = start_dynamic_mt;
                    log_debug("FlexRay%s: PoP Status: pos_cycle=%u pos_mt=%u",
                        m->log_id, m->status.pos_cycle, m->status.pos_mt);
                }
            } else {
                /* In the static part, calculate the Macrotick at the trailing
                edge of this LPDU. */
                uint32_t lpdu_mt =
                    (pdu->id + 1) * m->config.static_slot_length_mt;
                if (lpdu_mt > m->status.pos_mt) {
                    /* Push the Macrotick to the trailing edge. */
                    m->status.pos_mt = lpdu_mt;
                    log_debug("FlexRay%s: PoP Status: pos_cycle=%u pos_mt=%u",
                        m->log_id, m->status.pos_cycle, m->status.pos_mt);
                } else {
                    if (m->status.pos_mt >
                        (lpdu_mt + m->config.step_budget_mt)) {
                        /* Macrotick is too far advanced, retard. */
                        m->status.pos_mt = lpdu_mt;
                        log_debug(
                            "FlexRay%s: PoP Status: pos_cycle=%u pos_mt=%u",
                            m->log_id, m->status.pos_cycle, m->status.pos_mt);
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    return true;
}


void flexray_pop_bus_model_progress(ABCodecBusModel* bm)
{
    FlexrayPopBusModel* m = (FlexrayPopBusModel*)bm->model;

    log_debug("FlexRay%s: Progress: ", m->log_id);

    /* Each route should have at least a Status Message. */
    for (size_t i = 0; i < vector_len(&m->pdu_router); i++) {
        VectorPduRouteItem* pdu_route = vector_at(&m->pdu_router, i, NULL);

        if (pdu_route->node_ident.node_id == 0) continue;


        /* Each node should have Status PDU first. */
        NCodecPdu* pdu = vector_at(&pdu_route->pdu_list, 0, NULL);
        if (pdu != NULL && pdu->transport.flexray.metadata_type ==
                               NCodecPduFlexrayMetadataTypeStatus) {
            /* Status is first. This should be the normal path. */
            pdu->transport.flexray.metadata.status.cycle = m->status.pos_cycle;
            pdu->transport.flexray.metadata.status.macrotick = m->status.pos_mt;
            continue;
        }
        vector_sort(&pdu_route->pdu_list);
        pdu = vector_at(&pdu_route->pdu_list, 0, NULL);
        if (pdu != NULL && pdu->transport.flexray.metadata_type ==
                               NCodecPduFlexrayMetadataTypeStatus) {
            /* Status is first. */
            pdu->transport.flexray.metadata.status.cycle = m->status.pos_cycle;
            pdu->transport.flexray.metadata.status.macrotick = m->status.pos_mt;
            continue;
        } else {
            /* There is no status, add one. */
            vector_push(&pdu_route->pdu_list,
                &(NCodecPdu){
                    .transport_type = NCodecPduTransportTypeFlexray,
                    .transport.flexray.metadata_type =
                        NCodecPduFlexrayMetadataTypeStatus,
                    .transport.flexray.node_ident = pdu_route->node_ident,
                    .transport.flexray.metadata.status.cycle =
                        m->status.pos_cycle,
                    .transport.flexray.metadata.status.macrotick =
                        m->status.pos_mt,
                    .transport.flexray.metadata.status.channel[0].tcvr_state =
                        NCodecPduFlexrayTransceiverStateNoConnection,
                });
            vector_sort(&pdu_route->pdu_list);
        }
    }

    /* Encode the route list for _this_ node. */
    VectorPduRouteItem* node_pdu_route = vector_find(&m->pdu_router,
        &(VectorPduRouteItem){ .node_ident = m->node_ident }, 0, NULL);
    assert(node_pdu_route);
    for (size_t i = 0; i < vector_len(&node_pdu_route->pdu_list); i++) {
        NCodecPdu* pdu = vector_at(&node_pdu_route->pdu_list, i, NULL);
        ncodec_write((NCODEC*)bm->nc, pdu);
    }


    __pdu_router_clear(m);
}


void flexray_pop_bus_model_close(ABCodecBusModel* bm)
{
    FlexrayPopBusModel* m = (FlexrayPopBusModel*)bm->model;

    log_debug("FlexRay%s: Close: ", m->log_id);

    __pdu_router_destroy(m);
}


void flexray_pop_bus_model_create(ABCodecInstance* nc)
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

    /* Create the Bus Model object. */
    FlexrayPopBusModel* bm = calloc(1, sizeof(FlexrayPopBusModel));
    bm->node_ident.node.ecu_id = nc->ecu_id;
    bm->node_ident.node.cc_id = nc->cc_id;
    bm->node_ident.node.swc_id = nc->swc_id;
    snprintf(bm->log_id, FLEXRAY_LOG_ID_LEN, "(%u:%u:%u)",
        bm->node_ident.node.ecu_id, bm->node_ident.node.cc_id,
        bm->node_ident.node.swc_id);
    log_debug("FlexRay%s: Create: ", bm->log_id);

    bm->pdu_router =
        vector_make(sizeof(VectorPduRouteItem), 0, __node_ident_compar);
    __pdu_router_ensure_route(bm, bm->node_ident);


    /* Install and configure the Bus Model VTable. */
    nc->reader.bus_model.model = bm;
    nc->reader.bus_model.vtable.consume = flexray_pop_bus_model_consume;
    nc->reader.bus_model.vtable.progress = flexray_pop_bus_model_progress;
    nc->reader.bus_model.vtable.close = flexray_pop_bus_model_close;
}
