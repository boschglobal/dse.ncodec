// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <math.h>
#include <dse/logger.h>
#include <dse/ncodec/codec/ab/vector.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>


#define UNUSED(x) ((void)x)
#define MAX_CYCLE 64 /* 0.. 63 */


typedef struct VectorSlotMapItem {
    uint32_t slot_id;
    Vector   lpdus; /* FlexrayLpdu */
} VectorSlotMapItem;

int VectorSlotMapItemCompar(const void* left, const void* right)
{
    const VectorSlotMapItem* l = left;
    const VectorSlotMapItem* r = right;
    if (l->slot_id < r->slot_id) return -1;
    if (l->slot_id > r->slot_id) return 1;
    return 0;
}


static inline int __merge_uint32(uint32_t* param, uint32_t v)
{
    if (*param != 0 && *param != v) {
        log_error("FlexRay config mismatch");
        return -EBADE;
    } else {
        *param = v;
        return 0;
    }
}

int process_config(NCodecPdu* pdu, FlexrayEngine* engine)
{
    if (pdu == NULL || engine == NULL) return -EINVAL;
    assert(pdu->transport_type == NCodecPduTransportTypeFlexray);
    assert(pdu->transport.flexray.metadata_type ==
           NCodecPduFlexrayMetadataTypeConfig);
    NCodecPduFlexrayConfig* config = &pdu->transport.flexray.metadata.config;
    if (config->bit_rate == NCodecPduFlexrayBitrateNone) {
        log_error("FlexRay%s: Config: no bitrate", engine->log_id);
        return -EINVAL;
    }
    if (config->bit_rate > NCodecPduFlexrayBitrate2_5) {
        log_error("FlexRay%s: Config: bitrate not supported: %u",
            engine->log_id, config->bit_rate);
        return -EINVAL;
    }

    int rc = 0;
    rc |= __merge_uint32(
        &engine->microtick_per_cycle, config->microtick_per_cycle);
    rc |= __merge_uint32(
        &engine->macrotick_per_cycle, config->macrotick_per_cycle);
    rc |= __merge_uint32(
        &engine->static_slot_length_mt, config->static_slot_length);
    rc |= __merge_uint32(&engine->static_slot_count, config->static_slot_count);
    rc |= __merge_uint32(&engine->minislot_length_mt, config->minislot_length);
    rc |= __merge_uint32(&engine->minislot_count, config->minislot_count);
    rc |= __merge_uint32(&engine->static_slot_payload_length,
        config->static_slot_payload_length);
    rc |= __merge_uint32(
        &engine->microtick_ns, flexray_microtick_ns[config->bit_rate]);
    rc |= __merge_uint32(&engine->macro2micro,
        engine->microtick_per_cycle / engine->macrotick_per_cycle);
    rc |= __merge_uint32(&engine->macrotick_ns,
        engine->macro2micro * flexray_microtick_ns[config->bit_rate]);
    rc |= __merge_uint32(&engine->offset_static_mt, 0);
    rc |= __merge_uint32(&engine->offset_dynamic_mt,
        engine->static_slot_length_mt * engine->static_slot_count);
    rc |=
        __merge_uint32(&engine->offset_network_mt, config->network_idle_start);
    if (rc != 0) return rc;

    if (engine->pos_slot == 0) {
        /* Slot counts from 1... */
        engine->pos_slot = 1;
    }
    engine->bits_per_minislot = engine->minislot_length_mt *
                                engine->macrotick_ns /
                                flexray_bittime_ns[config->bit_rate];

    /* Configure the Slot Map. */
    NCodecPduFlexrayLpduConfig* frame_config_table = NULL;
    if (config->frame_config.count) {
        frame_config_table = calloc(
            config->frame_config.count, sizeof(NCodecPduFlexrayLpduConfig));
        memcpy(frame_config_table, config->frame_config.table,
            config->frame_config.count * sizeof(NCodecPduFlexrayLpduConfig));
    }
    if (engine->slot_map.capacity == 0) {
        engine->slot_map =
            vector_make(sizeof(VectorSlotMapItem), 0, VectorSlotMapItemCompar);
    }
    for (size_t i = 0; i < config->frame_config.count; i++) {
        uint16_t           slot_id = frame_config_table[i].slot_id;
        /* Find the Slot Map entry. */
        VectorSlotMapItem* slot_map_item = NULL;
        slot_map_item = vector_find(&engine->slot_map,
            &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);
        if (slot_map_item == NULL) {
            vector_push(&engine->slot_map,
                &(VectorSlotMapItem){ .slot_id = slot_id,
                    .lpdus = vector_make(sizeof(FlexrayLpdu), 0, NULL) });
            vector_sort(&engine->slot_map);
            slot_map_item = vector_find(&engine->slot_map,
                &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);

            /* Continue if vector operations failed. */
            if (slot_map_item == NULL) continue;
        }

        /* Add the LPDU config to the Slot Map. */
        vector_push(&slot_map_item->lpdus,
            &(FlexrayLpdu){ .node_ident = config->node_ident,
                .lpdu_config = frame_config_table[i] });
    }

    /* Configure TXRX Inform List (hold references to LPDUs). */
    if (engine->txrx_list.capacity == 0) {
        engine->txrx_list = vector_make(sizeof(FlexrayLpdu*), 0, NULL);
    }

    /* Configure Config List. */
    if (engine->config_list.capacity == 0) {
        engine->config_list =
            vector_make(sizeof(NCodecPduFlexrayLpduConfig*), 0, NULL);
    }
    if (frame_config_table != NULL) {
        vector_push(&engine->config_list, &frame_config_table);
    }

    /* Additional options. */
    engine->inhibit_null_frames = config->inhibit_null_frames;

    /* Configuration complete. */
    NCodecPduFlexrayNodeIdentifier node_ident = engine->node_ident;
    NCodecPduFlexrayNodeIdentifier cnode_ident = config->node_ident;
    log_info("FlexRay%s: Engine: ==== Configuration for Node:(%d:%d:%d) ====",
        engine->log_id, cnode_ident.node.ecu_id, cnode_ident.node.cc_id,
        cnode_ident.node.swc_id);
    log_info("FlexRay%s: Engine: sim_step_size=%f", engine->log_id,
        engine->sim_step_size);

    log_info("FlexRay%s: Engine: microtick_per_cycle=%u", engine->log_id,
        engine->microtick_per_cycle);
    log_info("FlexRay%s: Engine: macrotick_per_cycle=%u", engine->log_id,
        engine->macrotick_per_cycle);

    log_info("FlexRay%s: Engine: static_slot_length_mt=%u", engine->log_id,
        engine->static_slot_length_mt);
    log_info("FlexRay%s: Engine: static_slot_count=%u", engine->log_id,
        engine->static_slot_count);
    log_info("FlexRay%s: Engine: minislot_length_mt=%u", engine->log_id,
        engine->minislot_length_mt);
    log_info("FlexRay%s: Engine: minislot_count=%u", engine->log_id,
        engine->minislot_count);
    log_info("FlexRay%s: Engine: static_slot_payload_length=%u", engine->log_id,
        engine->static_slot_payload_length);

    log_info("FlexRay%s: Engine: macro2micro=%u", engine->log_id,
        engine->macro2micro);
    log_info("FlexRay%s: Engine: microtick_ns=%u", engine->log_id,
        engine->microtick_ns);
    log_info("FlexRay%s: Engine: macrotick_ns=%u", engine->log_id,
        engine->macrotick_ns);
    log_info("FlexRay%s: Engine: offset_static_mt=%u", engine->log_id,
        engine->offset_static_mt);
    log_info("FlexRay%s: Engine: offset_dynamic_mt=%u", engine->log_id,
        engine->offset_dynamic_mt);
    log_info("FlexRay%s: Engine: offset_network_mt=%u", engine->log_id,
        engine->offset_network_mt);
    log_info("FlexRay%s: Engine: inhibit_null_frames=%u", engine->log_id,
        engine->inhibit_null_frames);
    log_info("FlexRay%s: Engine: Frame Table: count=%u", engine->log_id,
        config->frame_config.count);
    for (size_t i = 0; i < config->frame_config.count; i++) {
        log_info("FlexRay%s: Engine: Frame Table: [%u] LPDU %04x "
                 "base=%u, rep=%u, dir=%u, tx_mode=%u, inhibit_null=%d",
            engine->log_id, i, config->frame_config.table[i].slot_id,
            config->frame_config.table[i].base_cycle,
            config->frame_config.table[i].cycle_repetition,
            config->frame_config.table[i].direction,
            config->frame_config.table[i].transmit_mode,
            config->frame_config.table[i].inhibit_null);
    }

    return 0;
}

int calculate_budget(FlexrayEngine* engine, double step_size)
{
    if (engine == NULL) return -EINVAL;
    if (engine->macrotick_ns == 0) {
        log_error("engine->macrotick_ns not configured", engine->log_id);
        return -EINVAL;
    }
    if (engine->macro2micro == 0) {
        log_error("engine->macro2micro not configured", engine->log_id);
        return -EINVAL;
    }

    if (step_size <= 0.0) {
        if (engine->sim_step_size <= 0.0) {
            return -EBADE;
        }
        step_size = engine->sim_step_size;
    }
    engine->step_budget_ut += (step_size * 1000000000) / engine->microtick_ns;
    engine->step_budget_mt = engine->step_budget_ut / engine->macro2micro;

    /* Clear the TxRx list from previous step. */
    vector_clear(&engine->txrx_list, NULL, NULL);
    return 0;
}

static void process_slot(FlexrayEngine* engine)
{
    VectorSlotMapItem* slot_map_item = vector_find(&engine->slot_map,
        &(VectorSlotMapItem){ .slot_id = engine->pos_slot }, 0, NULL);
    if (slot_map_item == NULL) {
        /* No configured slot. */
        return;
    }

    log_debug("FlexRay%s: Process slot: %u (cycle=%u, mt=%u) len=%u",
        engine->log_id, engine->pos_slot, engine->pos_cycle, engine->pos_mt,
        vector_len(&slot_map_item->lpdus));

    /* Search for Tx and Rx LPDUs in this slot. */
    FlexrayLpdu* tx_lpdu = NULL;
    FlexrayLpdu* rx_lpdu = NULL;
    bool         tx_null_frame = false;
    for (size_t i = 0; i < vector_len(&slot_map_item->lpdus); i++) {
        FlexrayLpdu* lpdu_item = vector_at(&slot_map_item->lpdus, i, NULL);
        if (lpdu_item->lpdu_config.direction == NCodecPduFlexrayDirectionTx) {
            /* TX LPDU. */
            if (engine->pos_mt < engine->offset_dynamic_mt) {
                /* Static Part. */
                if (lpdu_item->lpdu_config.cycle_repetition == 0) continue;
                if (engine->pos_cycle %
                        lpdu_item->lpdu_config.cycle_repetition ==
                    lpdu_item->lpdu_config.base_cycle) {
                    tx_lpdu = lpdu_item;
                    log_debug("FlexRay%s:   Tx LPDU Identified (static): "
                              "index=%u, base=%u, repeat=%u, status=%u",
                        engine->log_id,
                        lpdu_item->lpdu_config.index.frame_table,
                        lpdu_item->lpdu_config.base_cycle,
                        lpdu_item->lpdu_config.cycle_repetition,
                        lpdu_item->lpdu_config.status);
                    /* Determine if the Tx will represent a NULL Frame. */
                    if (tx_lpdu->lpdu_config.status ==
                            NCodecPduFlexrayLpduStatusNone ||
                        tx_lpdu->lpdu_config.status ==
                            NCodecPduFlexrayLpduStatusTransmitted) {
                        tx_null_frame = true;
                    }
                }
            } else if (engine->pos_mt < engine->offset_network_mt) {
                /* Dynamic Part. */
                log_debug(
                    "FlexRay%s:   Tx LPDU Identified (dynamic): index=%u, "
                    "status=%u",
                    engine->log_id, lpdu_item->lpdu_config.index.frame_table,
                    lpdu_item->lpdu_config.status);
                tx_lpdu = lpdu_item;
            }
        }
    }
    if (tx_lpdu == NULL) return;

    /* Perform the Tx (-> Rx). */
    if (tx_lpdu->lpdu_config.status ==
            NCodecPduFlexrayLpduStatusNotTransmitted &&
        tx_null_frame == false) {
        /* Process the TX. */
        if (tx_lpdu->lpdu_config.transmit_mode !=
            NCodecPduFlexrayTransmitModeContinuous) {
            tx_lpdu->lpdu_config.status = NCodecPduFlexrayLpduStatusTransmitted;
        }
        if (tx_lpdu->payload == NULL) {
            log_debug("FlexRay%s:   LPDU %04x (len=%u) no payload available",
                engine->log_id, tx_lpdu->lpdu_config.slot_id,
                tx_lpdu->lpdu_config.payload_length);
        }
        tx_lpdu->cycle = engine->pos_cycle;
        tx_lpdu->macrotick = engine->pos_mt;
        if (tx_lpdu->node_ident.node_id == engine->node_ident.node_id) {
            vector_push(&engine->txrx_list, &tx_lpdu);
        }
    }

    /* And the associated RX, if identified. */
    for (size_t i = 0; i < vector_len(&slot_map_item->lpdus); i++) {
        FlexrayLpdu* rx_lpdu = NULL;
        FlexrayLpdu* lpdu_item = vector_at(&slot_map_item->lpdus, i, NULL);
        /* Identify Rx LPDU, only for this node. */
        if (lpdu_item->lpdu_config.direction == NCodecPduFlexrayDirectionRx) {
            /* Check configured LPDU status. */
            if (lpdu_item->lpdu_config.status !=
                    NCodecPduFlexrayLpduStatusNotReceived &&
                lpdu_item->lpdu_config.status !=
                    NCodecPduFlexrayLpduStatusReceived) {
                continue;
            }
            /* Check configured on the NCodec for this node.*/
            if (lpdu_item->node_ident.node_id == engine->node_ident.node_id) {
                if (engine->pos_mt < engine->offset_dynamic_mt) {
                    /* Static Part. */
                    if (lpdu_item->lpdu_config.cycle_repetition == 0) continue;
                    if (engine->pos_cycle %
                            lpdu_item->lpdu_config.cycle_repetition ==
                        lpdu_item->lpdu_config.base_cycle) {
                        rx_lpdu = lpdu_item;
                        log_debug("FlexRay%s:   Rx LPDU Identified (static): "
                                  "index=%u, base=%u, repeat=%u, status=%u",
                            engine->log_id,
                            lpdu_item->lpdu_config.index.frame_table,
                            lpdu_item->lpdu_config.base_cycle,
                            lpdu_item->lpdu_config.cycle_repetition,
                            lpdu_item->lpdu_config.status);
                    }
                } else if (engine->pos_mt < engine->offset_network_mt) {
                    /* Dynamic Part. */
                    log_debug("FlexRay%s:   Rx LPDU Identified (dynamic): "
                              "index=%u, status=%u",
                        engine->log_id,
                        lpdu_item->lpdu_config.index.frame_table,
                        lpdu_item->lpdu_config.status);
                    rx_lpdu = lpdu_item;
                }
            }
        }
        /* Perform the Rx for this LPDU (on this node). */
        if (rx_lpdu != NULL) {
            if (tx_null_frame) {
                if (rx_lpdu->lpdu_config.inhibit_null == false &&
                    engine->inhibit_null_frames == false) {
                    rx_lpdu->cycle = engine->pos_cycle;
                    rx_lpdu->macrotick = engine->pos_mt;
                    rx_lpdu->null_frame = true;
                    log_debug("FlexRay%s:   LPDU %04x: Rx <- NULL",
                        engine->log_id, rx_lpdu->lpdu_config.slot_id);
                    vector_push(&engine->txrx_list, &rx_lpdu);
                }
            } else {
                rx_lpdu->lpdu_config.status =
                    NCodecPduFlexrayLpduStatusReceived;
                if (rx_lpdu->payload == NULL) {
                    rx_lpdu->payload = calloc(
                        rx_lpdu->lpdu_config.payload_length, sizeof(uint8_t));
                }
                if (tx_lpdu->payload) {
                    size_t len = rx_lpdu->lpdu_config.payload_length;
                    if (len > tx_lpdu->lpdu_config.payload_length) {
                        len = tx_lpdu->lpdu_config.payload_length;
                    }
                    log_debug(
                        "FlexRay%s:   LPDU %04x: Rx <- Tx: payload_length=%u",
                        engine->log_id, tx_lpdu->lpdu_config.slot_id, len);
                    memset(rx_lpdu->payload + len, 0,
                        rx_lpdu->lpdu_config.payload_length - len);
                    memcpy(rx_lpdu->payload, tx_lpdu->payload, len);
                }
                rx_lpdu->cycle = engine->pos_cycle;
                rx_lpdu->macrotick = engine->pos_mt;
                rx_lpdu->null_frame = false;
                vector_push(&engine->txrx_list, &rx_lpdu);
            }
        }
    }
}

int consume_slot(FlexrayEngine* engine)
{
    if (engine->pos_mt < engine->offset_dynamic_mt) {
        /* In static part of cycle. */
        uint32_t need_mt = engine->static_slot_length_mt;
        uint32_t need_ut = need_mt * engine->macro2micro;
        if (need_ut > engine->step_budget_ut) {
            /* Not enough budget. */
            return 1;
        } else {
            /* Consume the slot. */
            process_slot(engine);
            engine->step_budget_ut -= need_ut;
            engine->step_budget_mt -= need_mt;
            engine->pos_slot += 1;
            engine->pos_mt += need_mt;
            return 0;
        }
    } else if (engine->pos_mt < engine->offset_network_mt) {
        /* In dynamic part of cycle. */
        uint32_t           need_mt = engine->minislot_length_mt;
        bool               pending_tx = false;
        VectorSlotMapItem* slot_map_item = vector_find(&engine->slot_map,
            &(VectorSlotMapItem){ .slot_id = engine->pos_slot }, 0, NULL);
        if (slot_map_item != NULL) {
            for (size_t i = 0; i < vector_len(&slot_map_item->lpdus); i++) {
                FlexrayLpdu* lpdu_item =
                    vector_at(&slot_map_item->lpdus, i, NULL);
                if (lpdu_item->lpdu_config.direction ==
                        NCodecPduFlexrayDirectionTx &&
                    lpdu_item->lpdu_config.status ==
                        NCodecPduFlexrayLpduStatusNotTransmitted) {
                    /* Pending TX LPDU, calculate transmission MT (round up). */
                    pending_tx = true;
                    unsigned int mini_slot_count =
                        (40 + (lpdu_item->lpdu_config.payload_length * 8) +
                            engine->bits_per_minislot - 1) /
                        engine->bits_per_minislot;
                    need_mt = mini_slot_count * engine->minislot_length_mt;
                }
            }
        }
        if (need_mt + engine->pos_mt > engine->macrotick_per_cycle) {
            log_info("FlexRay engine configuration exceeds cycle length: "
                     "need_mt=%u, pos_mt=%u, cycle_mt=%u",
                need_mt, engine->pos_mt, engine->macrotick_per_cycle);
            need_mt = engine->macrotick_per_cycle - engine->pos_mt;
        }
        uint32_t need_ut = need_mt * engine->macro2micro;
        if (need_ut > engine->step_budget_ut) {
            /* Not enough budget. */
            return 1;
        } else {
            /* Consume the slot. */
            if (pending_tx) {
                process_slot(engine);
            }
            engine->step_budget_ut -= need_ut;
            engine->step_budget_mt -= need_mt;
            engine->pos_slot += 1;
            engine->pos_mt += need_mt;
            return 0;
        }
    } else {
        /* At the end of the cycle.*/
        uint32_t remaining_ut = 0;
        if ((engine->pos_mt * engine->macro2micro) <
            engine->microtick_per_cycle) {
            remaining_ut = engine->microtick_per_cycle -
                           (engine->pos_mt * engine->macro2micro);
        } else {
            log_info("FlexRay engine configuration exceeds cycle length: "
                     "pos_mt=%u, cycle_mt=%u",
                engine->pos_mt, engine->macrotick_per_cycle);
        }
        if (remaining_ut > engine->step_budget_ut) {
            /* Not enough budget. */
            return 1;
        } else {
            /* Consume the slot remainder. */
            engine->step_budget_ut -= remaining_ut;
            /* Cycle complete, reset the pos markers. */
            engine->pos_slot = 1;
            engine->pos_mt = 0;
            engine->pos_cycle = (engine->pos_cycle + 1) % MAX_CYCLE;
            return 0;
        }
    }
    return 0;
}

static void __flexray_lpdu_destroy(void* item, void* data)
{
    UNUSED(data);

    FlexrayLpdu* lpdu = item;
    free(lpdu->payload);
    lpdu->payload = NULL;
}

static void __flexray_config_destroy(void* item, void* data)
{
    UNUSED(data);

    NCodecPduFlexrayLpduConfig** config = item;
    free(*config);
}

void release_config(FlexrayEngine* engine)
{
    for (size_t i = 0; i < engine->slot_map.length; i++) {
        VectorSlotMapItem slot_item;
        if (vector_at(&engine->slot_map, i, &slot_item)) {
            vector_clear(&slot_item.lpdus, __flexray_lpdu_destroy, NULL);
            vector_reset(&slot_item.lpdus);
        }
    }
    vector_reset(&engine->slot_map);
    vector_reset(&engine->txrx_list);
    vector_clear(&engine->config_list, __flexray_config_destroy, NULL);
    vector_reset(&engine->config_list);
}

int shift_cycle(FlexrayEngine* engine, uint32_t mt, uint8_t cycle, bool force)
{
    if (mt < engine->offset_dynamic_mt) {
        /* In static part of cycle. */

        /* Set the fundamental sync properties. */
        engine->pos_mt = mt;
        engine->pos_cycle = cycle % MAX_CYCLE;
        /* Adjust derivate properties. */
        engine->pos_slot = engine->pos_mt / engine->static_slot_length_mt + 1;
        /* No budget is carried over. */
        engine->step_budget_ut = 0;
        engine->step_budget_mt = 0;

        return 0;
    } else if (force) {
        /* Unit testing support, shift on basis of no TX in dynamic part. */
        engine->pos_mt = mt;
        engine->pos_cycle = cycle % MAX_CYCLE;
        /* Adjust derivate properties. */
        engine->pos_slot = ((engine->pos_mt - engine->offset_dynamic_mt) /
                               engine->minislot_length_mt) +
                           engine->static_slot_count + 1;
        /* No budget is carried over. */
        engine->step_budget_ut = 0;
        engine->step_budget_mt = 0;
        return 0;
    } else {
        /* In dynamic part of cycle, shift is not possible. */
        return 1;
    }
}

int set_lpdu(FlexrayEngine* engine, uint64_t node_id, uint32_t slot_id,
    uint32_t frame_config_index, NCodecPduFlexrayLpduStatus status,
    const uint8_t* payload, size_t payload_len)
{
    /* Search for the LPDU. */
    VectorSlotMapItem* slot_map_item = NULL;
    slot_map_item = vector_find(
        &engine->slot_map, &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);
    if (slot_map_item == NULL) {
        /* No configured slot. */
        log_debug("No configured slot_map!");
        return -EINVAL;
    }
    FlexrayLpdu* lpdu = NULL;
    for (size_t i = 0; i < vector_len(&slot_map_item->lpdus); i++) {
        FlexrayLpdu* lpdu_item = vector_at(&slot_map_item->lpdus, i, NULL);
        if (lpdu_item->node_ident.node_id == node_id &&
            lpdu_item->lpdu_config.index.frame_table == frame_config_index) {
            lpdu = lpdu_item;
            break;
        }
    }
    if (lpdu == NULL) {
        log_debug("No LPDU found in slot_map!");
        return -EINVAL;
    }

    lpdu->lpdu_config.status = status;

    if (lpdu->lpdu_config.direction == NCodecPduFlexrayDirectionTx) {
        /* Set the LPDU payload. */
        if (lpdu->payload == NULL) {
            lpdu->payload =
                calloc(lpdu->lpdu_config.payload_length, sizeof(uint8_t));
        }
        size_t len = lpdu->lpdu_config.payload_length;
        if (len > payload_len) {
            len = payload_len;
        }
        memset(lpdu->payload + len, 0, lpdu->lpdu_config.payload_length - len);
        memcpy(lpdu->payload, payload, len);
    }

    return 0;
}
