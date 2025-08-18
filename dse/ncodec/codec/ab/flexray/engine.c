// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <math.h>
#include <dse/logger.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/ncodec/codec/ab/flexray/flexray.h>


#define UNUSED(x) ((void)x)
#define MAX_CYCLE 64 /* 0.. 63 */


typedef struct VectorSlotMapItem {
    uint32_t slot_id;
    Vector   lpdus;
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
        return -EBADE;
    } else {
        *param = v;
        return 0;
    }
}

int process_config(NCodecPduFlexrayConfig* config, FlexrayEngine* engine)
{
    if (config == NULL || engine == NULL) return -EINVAL;
    if (config->bit_rate == NCodecPduFlexrayBitrateNone) return 0;
    if (config->bit_rate > NCodecPduFlexrayBitrate2_5) return -EINVAL;

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
    if (engine->slot_map.capacity == 0) {
        engine->slot_map =
            vector_make(sizeof(VectorSlotMapItem), 0, VectorSlotMapItemCompar);
    }
    for (size_t i = 0; i < config->frame_config.count; i++) {
        uint16_t slot_id = config->frame_config.table[i].slot_id;

        /* Find the Slot Map entry. */
        VectorSlotMapItem* slot_map_item = NULL;
        slot_map_item = vector_find(&engine->slot_map,
            &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);
        if (slot_map_item == NULL) {
            vector_push(&engine->slot_map,
                &(VectorSlotMapItem){ .slot_id = slot_id,
                    .lpdus = vector_make(sizeof(FlexrayLpdu), 0, NULL) });
            slot_map_item = vector_find(&engine->slot_map,
                &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);

            /* Continue if vector operations failed. */
            if (slot_map_item == NULL) continue;
        }

        /* Add the LPDU config to the Slot Map. */
        vector_push(&slot_map_item->lpdus,
            &(FlexrayLpdu){ .node_ident = config->node_ident,
                .lpdu_config = config->frame_config.table[i] });
    }
    vector_sort(&engine->slot_map);

    /* Configure TXRX Inform List (hold references to LPDUs). */
    if (engine->txrx_list.capacity == 0) {
        engine->txrx_list = vector_make(sizeof(FlexrayLpdu*), 0, NULL);
    }

    /* Configuration complete. */
    return 0;
}

int calculate_budget(FlexrayEngine* engine, double step_size)
{
    if (engine == NULL) return -EINVAL;

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

    log_trace("Process slot: %u (cycle=%u, mt=%u)", engine->pos_slot,
        engine->pos_cycle, engine->pos_mt);

    /* Search for Tx and Rx LPDUs in this slot. */
    FlexrayLpdu* tx_lpdu = NULL;
    FlexrayLpdu* rx_lpdu = NULL;
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
                    log_trace("Tx LPDU Identified (static): base=%u, repeat=%u",
                        lpdu_item->lpdu_config.base_cycle,
                        lpdu_item->lpdu_config.cycle_repetition);
                }
            } else if (engine->pos_mt < engine->offset_network_mt) {
                /* Dynamic Part. */
                log_trace("Tx LPDU Identified (dynamic)");
                tx_lpdu = lpdu_item;
            }
        } else if (lpdu_item->lpdu_config.direction ==
                   NCodecPduFlexrayDirectionRx) {
            /* RX LPDU, only for the NCodec object representing this node. */
            if (lpdu_item->node_ident.node_id == engine->node_ident.node_id) {
                if (engine->pos_mt < engine->offset_dynamic_mt) {
                    /* Static Part. */
                    if (lpdu_item->lpdu_config.cycle_repetition == 0) continue;
                    if (engine->pos_cycle %
                            lpdu_item->lpdu_config.cycle_repetition ==
                        lpdu_item->lpdu_config.base_cycle) {
                        rx_lpdu = lpdu_item;
                        log_trace("Rx LPDU: base=%u, repeat=%u",
                            lpdu_item->lpdu_config.base_cycle,
                            lpdu_item->lpdu_config.cycle_repetition);
                    }
                } else if (engine->pos_mt < engine->offset_network_mt) {
                    /* Dynamic Part. */
                    log_trace("Rx LPDU Identified");
                    rx_lpdu = lpdu_item;
                }
            }
        }
    }
    if (tx_lpdu == NULL) return;

    /* Perform the Tx -> Rx. */
    if (tx_lpdu->lpdu_config.status ==
        NCodecPduFlexrayLpduStatusNotTransmitted) {
        /* Process the TX. */
        if (tx_lpdu->lpdu_config.transmit_mode !=
            NCodecPduFlexrayTransmitModeContinuous) {
            tx_lpdu->lpdu_config.status = NCodecPduFlexrayLpduStatusTransmitted;
        }
        if (tx_lpdu->node_ident.node_id == engine->node_ident.node_id) {
            vector_push(&engine->txrx_list, &tx_lpdu);
        }
        /* And the associated RX, if identified. */
        if (rx_lpdu != NULL) {
            rx_lpdu->lpdu_config.status = NCodecPduFlexrayLpduStatusReceived;
            if (rx_lpdu->payload == NULL) {
                rx_lpdu->payload = calloc(
                    rx_lpdu->lpdu_config.payload_length, sizeof(uint8_t));
            }
            if (tx_lpdu->payload) {
                size_t len = rx_lpdu->lpdu_config.payload_length;
                if (len > tx_lpdu->lpdu_config.payload_length) {
                    len = tx_lpdu->lpdu_config.payload_length;
                }
                log_trace("Rx <- Tx: payload_length=%u", len);
                memset(rx_lpdu->payload + len, 0,
                    rx_lpdu->lpdu_config.payload_length - len);
                memcpy(rx_lpdu->payload, tx_lpdu->payload, len);
            }
            vector_push(&engine->txrx_list, &rx_lpdu);
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
                    uint mini_slot_count =
                        (40 + (lpdu_item->lpdu_config.payload_length * 8) +
                            engine->bits_per_minislot - 1) /
                        engine->bits_per_minislot;
                    need_mt = mini_slot_count * engine->minislot_length_mt;
                }
            }
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
        /* At the end of the cycle. */
        uint32_t need_ut = engine->microtick_per_cycle -
                           (engine->pos_mt * engine->macro2micro);
        if (need_ut > engine->step_budget_ut) {
            /* Not enough budget. */
            return 1;
        } else {
            /* Consume the slot. */
            engine->step_budget_ut -= need_ut;
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

int set_payload(FlexrayEngine* engine, uint64_t node_id, uint32_t slot_id,
    NCodecPduFlexrayLpduStatus status, uint8_t* payload, size_t payload_len)
{
    /* Search for the LPDU. */
    VectorSlotMapItem* slot_map_item = NULL;
    slot_map_item = vector_find(
        &engine->slot_map, &(VectorSlotMapItem){ .slot_id = slot_id }, 0, NULL);
    if (slot_map_item == NULL) {
        /* No configured slot. */
        return -EINVAL;
    }
    FlexrayLpdu* lpdu = NULL;
    for (size_t i = 0; i < vector_len(&slot_map_item->lpdus); i++) {
        FlexrayLpdu* lpdu_item = vector_at(&slot_map_item->lpdus, i, NULL);
        if (lpdu_item->node_ident.node_id == node_id &&
            lpdu_item->lpdu_config.direction == NCodecPduFlexrayDirectionTx) {
            lpdu = lpdu_item;
            break;
        }
    }
    if (lpdu == NULL) {
        return -EINVAL;
    }

    /* Set the LPDU payload. */
    lpdu->lpdu_config.status = status;
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

    return 0;
}
