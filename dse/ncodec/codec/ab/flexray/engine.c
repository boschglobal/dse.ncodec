// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <math.h>
#include <dse/ncodec/codec/ab/codec.h>
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


static inline int __merge_uint32(
    FlexrayBusModel* m, uint32_t* param, uint32_t v)
{
    if (*param != 0 && *param != v) {
        log_error(m->log_nc, "FlexRay config mismatch");
        return -EINVAL;
    } else {
        *param = v;
        return 0;
    }
}

int process_config(FlexrayBusModel* m, NCodecPdu* pdu)
{
    if (pdu == NULL && m == NULL) return -EINVAL;
    assert(pdu->transport_type == NCodecPduTransportTypeFlexray);
    assert(pdu->transport.flexray.metadata_type ==
           NCodecPduFlexrayMetadataTypeConfig);
    NCodecPduFlexrayConfig* config = &pdu->transport.flexray.metadata.config;
    if (config->bit_rate == NCodecPduFlexrayBitrateNone) {
        log_error(m->log_nc, "FlexRay%s: Config: no bitrate", m->engine.log_id);
        return -EINVAL;
    }
    if (config->bit_rate > NCodecPduFlexrayBitrate2_5) {
        log_error(m->log_nc, "FlexRay%s: Config: bitrate not supported: %u",
            m->engine.log_id, config->bit_rate);
        return -EINVAL;
    }

    int rc = 0;
    rc |= __merge_uint32(
        m, &m->engine.microtick_per_cycle, config->microtick_per_cycle);
    rc |= __merge_uint32(
        m, &m->engine.macrotick_per_cycle, config->macrotick_per_cycle);
    rc |= __merge_uint32(
        m, &m->engine.static_slot_length_mt, config->static_slot_length);
    rc |= __merge_uint32(
        m, &m->engine.static_slot_count, config->static_slot_count);
    rc |= __merge_uint32(
        m, &m->engine.minislot_length_mt, config->minislot_length);
    rc |= __merge_uint32(m, &m->engine.minislot_count, config->minislot_count);
    rc |= __merge_uint32(m, &m->engine.static_slot_payload_length,
        config->static_slot_payload_length);
    rc |= __merge_uint32(
        m, &m->engine.microtick_ns, flexray_microtick_ns[config->bit_rate]);
    rc |= __merge_uint32(m, &m->engine.macro2micro,
        m->engine.microtick_per_cycle / m->engine.macrotick_per_cycle);
    rc |= __merge_uint32(m, &m->engine.macrotick_ns,
        m->engine.macro2micro * flexray_microtick_ns[config->bit_rate]);
    rc |= __merge_uint32(m, &m->engine.offset_static_mt, 0);
    rc |= __merge_uint32(m, &m->engine.offset_dynamic_mt,
        m->engine.static_slot_length_mt * m->engine.static_slot_count);
    rc |= __merge_uint32(
        m, &m->engine.offset_network_mt, config->network_idle_start);
    if (rc != 0) return rc;

    if (m->engine.pos_slot == 0) {
        /* Slot counts from 1... */
        m->engine.pos_slot = 1;
    }
    m->engine.bits_per_minislot = m->engine.minislot_length_mt *
                                  m->engine.macrotick_ns /
                                  flexray_bittime_ns[config->bit_rate];

    /* Configure the Slot Map. */
    NCodecPduFlexrayLpduConfig* frame_config_table = NULL;
    if (config->frame_config.count) {
        frame_config_table = calloc(
            config->frame_config.count, sizeof(NCodecPduFlexrayLpduConfig));
        memcpy(frame_config_table, config->frame_config.table,
            config->frame_config.count * sizeof(NCodecPduFlexrayLpduConfig));
    }
    if (m->engine.slot_map.capacity == 0) {
        m->engine.slot_map =
            vector_make(sizeof(VectorSlotMapItem), 0, VectorSlotMapItemCompar);
    }
    for (size_t i = 0; i < config->frame_config.count; i++) {
        uint16_t           slot_id = frame_config_table[i].slot_id;
        /* Find the Slot Map entry. */
        VectorSlotMapItem* slot_map_item = NULL;
        slot_map_item = vector_find(&m->engine.slot_map,
            &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);
        if (slot_map_item == NULL) {
            vector_push(&m->engine.slot_map,
                &(VectorSlotMapItem){ .slot_id = slot_id,
                    .lpdus = vector_make(sizeof(FlexrayLpdu), 0, NULL) });
            vector_sort(&m->engine.slot_map);
            slot_map_item = vector_find(&m->engine.slot_map,
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
    if (m->engine.txrx_list.capacity == 0) {
        m->engine.txrx_list = vector_make(sizeof(FlexrayLpdu*), 0, NULL);
    }

    /* Configure Config List. */
    if (m->engine.config_list.capacity == 0) {
        m->engine.config_list =
            vector_make(sizeof(NCodecPduFlexrayLpduConfig*), 0, NULL);
    }
    if (frame_config_table != NULL) {
        vector_push(&m->engine.config_list, &frame_config_table);
    }

    /* Additional options. */
    m->engine.inhibit_null_frames = config->inhibit_null_frames;

    /* Configuration complete. */
    NCodecPduFlexrayNodeIdentifier node_ident = m->engine.node_ident;
    NCodecPduFlexrayNodeIdentifier cnode_ident = config->node_ident;
    log_info(m->log_nc,
        "FlexRay%s: Engine: ==== Configuration for Node:(%d:%d:%d) ====",
        m->engine.log_id, cnode_ident.node.ecu_id, cnode_ident.node.cc_id,
        cnode_ident.node.swc_id);
    log_info(m->log_nc, "FlexRay%s: Engine: sim_step_size=%f", m->engine.log_id,
        m->engine.sim_step_size);

    log_info(m->log_nc, "FlexRay%s: Engine: microtick_per_cycle=%u",
        m->engine.log_id, m->engine.microtick_per_cycle);
    log_info(m->log_nc, "FlexRay%s: Engine: macrotick_per_cycle=%u",
        m->engine.log_id, m->engine.macrotick_per_cycle);

    log_info(m->log_nc, "FlexRay%s: Engine: static_slot_length_mt=%u",
        m->engine.log_id, m->engine.static_slot_length_mt);
    log_info(m->log_nc, "FlexRay%s: Engine: static_slot_count=%u",
        m->engine.log_id, m->engine.static_slot_count);
    log_info(m->log_nc, "FlexRay%s: Engine: minislot_length_mt=%u",
        m->engine.log_id, m->engine.minislot_length_mt);
    log_info(m->log_nc, "FlexRay%s: Engine: minislot_count=%u",
        m->engine.log_id, m->engine.minislot_count);
    log_info(m->log_nc, "FlexRay%s: Engine: static_slot_payload_length=%u",
        m->engine.log_id, m->engine.static_slot_payload_length);

    log_info(m->log_nc, "FlexRay%s: Engine: macro2micro=%u", m->engine.log_id,
        m->engine.macro2micro);
    log_info(m->log_nc, "FlexRay%s: Engine: microtick_ns=%u", m->engine.log_id,
        m->engine.microtick_ns);
    log_info(m->log_nc, "FlexRay%s: Engine: macrotick_ns=%u", m->engine.log_id,
        m->engine.macrotick_ns);
    log_info(m->log_nc, "FlexRay%s: Engine: offset_static_mt=%u",
        m->engine.log_id, m->engine.offset_static_mt);
    log_info(m->log_nc, "FlexRay%s: Engine: offset_dynamic_mt=%u",
        m->engine.log_id, m->engine.offset_dynamic_mt);
    log_info(m->log_nc, "FlexRay%s: Engine: offset_network_mt=%u",
        m->engine.log_id, m->engine.offset_network_mt);
    log_info(m->log_nc, "FlexRay%s: Engine: inhibit_null_frames=%u",
        m->engine.log_id, m->engine.inhibit_null_frames);
    log_info(m->log_nc, "FlexRay%s: Engine: Frame Table: count=%u",
        m->engine.log_id, config->frame_config.count);
    for (size_t i = 0; i < config->frame_config.count; i++) {
        log_info(m->log_nc,
            "FlexRay%s: Engine: Frame Table: [%u] LPDU %04x "
            "base=%u, rep=%u, dir=%u, tx_mode=%u, inhibit_null=%d",
            m->engine.log_id, i, config->frame_config.table[i].slot_id,
            config->frame_config.table[i].base_cycle,
            config->frame_config.table[i].cycle_repetition,
            config->frame_config.table[i].direction,
            config->frame_config.table[i].transmit_mode,
            config->frame_config.table[i].inhibit_null);
    }

    return 0;
}

int calculate_budget(FlexrayBusModel* m, double step_size)
{
    if (m == NULL) return -EINVAL;
    if (m->engine.macrotick_ns == 0) {
        log_error(m->log_nc, "m->engine.macrotick_ns not configured",
            m->engine.log_id);
        return -EINVAL;
    }
    if (m->engine.macro2micro == 0) {
        log_error(m->log_nc, "m->engine.macro2micro not configured",
            m->engine.log_id);
        return -EINVAL;
    }

    if (step_size <= 0.0) {
        if (m->engine.sim_step_size <= 0.0) {
            return -EINVAL;
        }
        step_size = m->engine.sim_step_size;
    }
    m->engine.step_budget_ut +=
        (step_size * 1000000000) / m->engine.microtick_ns;
    m->engine.step_budget_mt = m->engine.step_budget_ut / m->engine.macro2micro;

    /* Clear the TxRx list from previous step. */
    vector_clear(&m->engine.txrx_list, NULL, NULL);
    return 0;
}

static void process_slot(FlexrayBusModel* m)
{
    VectorSlotMapItem* slot_map_item = vector_find(&m->engine.slot_map,
        &(VectorSlotMapItem){ .slot_id = m->engine.pos_slot }, 0, NULL);
    if (slot_map_item == NULL) {
        /* No configured slot. */
        return;
    }

    log_debug(m->log_nc, "FlexRay%s: Process slot: %u (cycle=%u, mt=%u) len=%u",
        m->engine.log_id, m->engine.pos_slot, m->engine.pos_cycle,
        m->engine.pos_mt, vector_len(&slot_map_item->lpdus));

    /* Search for Tx and Rx LPDUs in this slot. */
    FlexrayLpdu* tx_lpdu = NULL;
    FlexrayLpdu* rx_lpdu = NULL;
    bool         tx_null_frame = false;
    for (size_t i = 0; i < vector_len(&slot_map_item->lpdus); i++) {
        FlexrayLpdu* lpdu_item = vector_at(&slot_map_item->lpdus, i, NULL);
        if (lpdu_item->lpdu_config.direction == NCodecPduFlexrayDirectionTx) {
            /* TX LPDU. */
            if (m->engine.pos_mt < m->engine.offset_network_mt) {
                /* Static Part / Dynamic Part. */
                if (lpdu_item->lpdu_config.cycle_repetition == 0) continue;
                if (m->engine.pos_cycle %
                        lpdu_item->lpdu_config.cycle_repetition !=
                    lpdu_item->lpdu_config.base_cycle)
                    continue;
                /* Tx identified. */
                tx_lpdu = lpdu_item;
                log_debug(m->log_nc,
                    "FlexRay%s:   Tx LPDU Identified (%s): "
                    "index=%u, base=%u, repeat=%u, status=%u",
                    m->engine.log_id,
                    (m->engine.pos_mt < m->engine.offset_dynamic_mt)
                        ? "static"
                        : "dynamic",
                    lpdu_item->lpdu_config.index.frame_table,
                    lpdu_item->lpdu_config.base_cycle,
                    lpdu_item->lpdu_config.cycle_repetition,
                    lpdu_item->lpdu_config.status);
                if (m->engine.pos_mt < m->engine.offset_dynamic_mt) {
                    /* Determine if the Tx will represent a NULL Frame. */
                    if (tx_lpdu->lpdu_config.status ==
                            NCodecPduFlexrayLpduStatusNone ||
                        tx_lpdu->lpdu_config.status ==
                            NCodecPduFlexrayLpduStatusTransmitted) {
                        tx_null_frame = true;
                    }
                }
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
            log_debug(m->log_nc,
                "FlexRay%s:   LPDU %04x (len=%u) no payload available",
                m->engine.log_id, tx_lpdu->lpdu_config.slot_id,
                tx_lpdu->lpdu_config.payload_length);
        }
        tx_lpdu->cycle = m->engine.pos_cycle;
        tx_lpdu->macrotick = m->engine.pos_mt;
        if (tx_lpdu->node_ident.node_id == m->engine.node_ident.node_id) {
            vector_push(&m->engine.txrx_list, &tx_lpdu);
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
            if (lpdu_item->node_ident.node_id == m->engine.node_ident.node_id) {
                if (m->engine.pos_mt < m->engine.offset_network_mt) {
                    /* Static / Dynamic Part. */
                    if (lpdu_item->lpdu_config.cycle_repetition == 0) continue;
                    if (m->engine.pos_cycle %
                            lpdu_item->lpdu_config.cycle_repetition !=
                        lpdu_item->lpdu_config.base_cycle)
                        continue;
                    /* Rx identified. */
                    rx_lpdu = lpdu_item;
                    log_debug(m->log_nc,
                        "FlexRay%s:   Rx LPDU Identified (%s): "
                        "index=%u, base=%u, repeat=%u, status=%u",
                        m->engine.log_id,
                        (m->engine.pos_mt < m->engine.offset_dynamic_mt)
                            ? "static"
                            : "dynamic",
                        lpdu_item->lpdu_config.index.frame_table,
                        lpdu_item->lpdu_config.base_cycle,
                        lpdu_item->lpdu_config.cycle_repetition,
                        lpdu_item->lpdu_config.status);
                }
            }
        }
        /* Perform the Rx for this LPDU (on this node). */
        if (rx_lpdu != NULL) {
            if (tx_null_frame) {
                if (rx_lpdu->lpdu_config.inhibit_null == false &&
                    m->engine.inhibit_null_frames == false) {
                    rx_lpdu->cycle = m->engine.pos_cycle;
                    rx_lpdu->macrotick = m->engine.pos_mt;
                    rx_lpdu->null_frame = true;
                    log_debug(m->log_nc, "FlexRay%s:   LPDU %04x: Rx <- NULL",
                        m->engine.log_id, rx_lpdu->lpdu_config.slot_id);
                    vector_push(&m->engine.txrx_list, &rx_lpdu);
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
                    log_debug(m->log_nc,
                        "FlexRay%s:   LPDU %04x: Rx <- Tx: payload_length=%u",
                        m->engine.log_id, tx_lpdu->lpdu_config.slot_id, len);
                    memset(rx_lpdu->payload + len, 0,
                        rx_lpdu->lpdu_config.payload_length - len);
                    memcpy(rx_lpdu->payload, tx_lpdu->payload, len);
                }
                rx_lpdu->cycle = m->engine.pos_cycle;
                rx_lpdu->macrotick = m->engine.pos_mt;
                rx_lpdu->null_frame = false;
                vector_push(&m->engine.txrx_list, &rx_lpdu);
            }
        }
    }
}

int consume_slot(FlexrayBusModel* m)
{
    if (m->engine.pos_mt < m->engine.offset_dynamic_mt) {
        /* In static part of cycle. */
        uint32_t need_mt = m->engine.static_slot_length_mt;
        uint32_t need_ut = need_mt * m->engine.macro2micro;
        if (need_ut > m->engine.step_budget_ut) {
            /* Not enough budget. */
            return 1;
        } else {
            /* Consume the slot. */
            process_slot(m);
            m->engine.step_budget_ut -= need_ut;
            m->engine.step_budget_mt -= need_mt;
            m->engine.pos_slot += 1;
            m->engine.pos_mt += need_mt;
            return 0;
        }
    } else if (m->engine.pos_mt < m->engine.offset_network_mt) {
        /* In dynamic part of cycle. */
        uint32_t           need_mt = m->engine.minislot_length_mt;
        bool               pending_tx = false;
        VectorSlotMapItem* slot_map_item = vector_find(&m->engine.slot_map,
            &(VectorSlotMapItem){ .slot_id = m->engine.pos_slot }, 0, NULL);
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
                            m->engine.bits_per_minislot - 1) /
                        m->engine.bits_per_minislot;
                    need_mt = mini_slot_count * m->engine.minislot_length_mt;
                }
            }
        }
        if (need_mt + m->engine.pos_mt > m->engine.macrotick_per_cycle) {
            log_info(m->log_nc,
                "FlexRay engine configuration exceeds cycle length: "
                "need_mt=%u, pos_mt=%u, cycle_mt=%u",
                need_mt, m->engine.pos_mt, m->engine.macrotick_per_cycle);
            need_mt = m->engine.macrotick_per_cycle - m->engine.pos_mt;
        }
        uint32_t need_ut = need_mt * m->engine.macro2micro;
        if (need_ut > m->engine.step_budget_ut) {
            /* Not enough budget. */
            return 1;
        } else {
            /* Consume the slot. */
            if (pending_tx) {
                process_slot(m);
            }
            m->engine.step_budget_ut -= need_ut;
            m->engine.step_budget_mt -= need_mt;
            m->engine.pos_slot += 1;
            m->engine.pos_mt += need_mt;
            return 0;
        }
    } else {
        /* At the end of the cycle.*/
        uint32_t remaining_ut = 0;
        if ((m->engine.pos_mt * m->engine.macro2micro) <
            m->engine.microtick_per_cycle) {
            remaining_ut = m->engine.microtick_per_cycle -
                           (m->engine.pos_mt * m->engine.macro2micro);
        } else {
            log_info(m->log_nc,
                "FlexRay engine configuration exceeds cycle length: "
                "pos_mt=%u, cycle_mt=%u",
                m->engine.pos_mt, m->engine.macrotick_per_cycle);
        }
        if (remaining_ut > m->engine.step_budget_ut) {
            /* Not enough budget. */
            return 1;
        } else {
            /* Consume the slot remainder. */
            m->engine.step_budget_ut -= remaining_ut;
            /* Cycle complete, reset the pos markers. */
            m->engine.pos_slot = 1;
            m->engine.pos_mt = 0;
            m->engine.pos_cycle = (m->engine.pos_cycle + 1) % MAX_CYCLE;
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

void release_config(FlexrayBusModel* m)
{
    for (size_t i = 0; i < m->engine.slot_map.length; i++) {
        VectorSlotMapItem slot_item;
        if (vector_at(&m->engine.slot_map, i, &slot_item)) {
            vector_clear(&slot_item.lpdus, __flexray_lpdu_destroy, NULL);
            vector_reset(&slot_item.lpdus);
        }
    }
    vector_reset(&m->engine.slot_map);
    vector_reset(&m->engine.txrx_list);
    vector_clear(&m->engine.config_list, __flexray_config_destroy, NULL);
    vector_reset(&m->engine.config_list);
}

int shift_cycle(FlexrayBusModel* m, uint32_t mt, uint8_t cycle, bool force)
{
    if (mt < m->engine.offset_dynamic_mt) {
        /* In static part of cycle. */

        /* Set the fundamental sync properties. */
        m->engine.pos_mt = mt;
        m->engine.pos_cycle = cycle % MAX_CYCLE;
        /* Adjust derivate properties. */
        m->engine.pos_slot =
            m->engine.pos_mt / m->engine.static_slot_length_mt + 1;
        /* No budget is carried over. */
        m->engine.step_budget_ut = 0;
        m->engine.step_budget_mt = 0;

        return 0;
    } else if (force) {
        /* Unit testing support, shift on basis of no TX in dynamic part. */
        m->engine.pos_mt = mt;
        m->engine.pos_cycle = cycle % MAX_CYCLE;
        /* Adjust derivate properties. */
        m->engine.pos_slot = ((m->engine.pos_mt - m->engine.offset_dynamic_mt) /
                                 m->engine.minislot_length_mt) +
                             m->engine.static_slot_count + 1;
        /* No budget is carried over. */
        m->engine.step_budget_ut = 0;
        m->engine.step_budget_mt = 0;
        return 0;
    } else {
        /* In dynamic part of cycle, shift is not possible. */
        return 1;
    }
}

int set_lpdu(FlexrayBusModel* m, uint64_t node_id, uint32_t slot_id,
    uint32_t frame_config_index, NCodecPduFlexrayLpduStatus status,
    const uint8_t* payload, size_t payload_len)
{
    /* Search for the LPDU. */
    VectorSlotMapItem* slot_map_item = NULL;
    slot_map_item = vector_find(&m->engine.slot_map,
        &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);
    if (slot_map_item == NULL) {
        /* No configured slot. */
        log_debug(m->log_nc, "No configured slot_map!");
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
        log_debug(m->log_nc, "No LPDU found in slot_map!");
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
