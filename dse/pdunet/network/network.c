// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/log.h>
#include <dse/pdunet/network/network.h>


#define UNUSED(x)     ((void)x)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


typedef struct MPduItem {
    uint32_t   id;
    uint16_t   priority;
    PduObject* pdu;
} MPduItem;


uint32_t pdunet_checksum(const uint8_t* payload, size_t len)
{
    if (payload == NULL) return 0;

    // FNV-1a hash (http://www.isthe.com/chongo/tech/comp/fnv/)
    uint32_t csum = 2166136261UL; /* FNV_OFFSET 32 bit */
    for (size_t i = 0; i < len; ++i) {
        csum = csum ^ payload[i];
        csum = csum * 16777619UL; /* FNV_PRIME 32 bit */
    }
    return csum;
}

static void _set_skip(PduNetwork* net, size_t offset, size_t count, bool skip)
{
    bool* skip_vec = (bool*)vector_at(&net->matrix.signal.skip, offset, NULL);
    for (size_t i = 0; i < count; i++) {
        skip_vec[i] = skip;
    }
}

void pdunet_schedule(PduNetwork* net)
{
    /* Signals not calculated. */
    _set_skip(net, 0, net->matrix.signal.count, true);

    for (size_t pdu_idx = 0; pdu_idx < vector_len(&net->matrix.pdu);
        pdu_idx++) {
        PduObject* o = vector_at(&(net->matrix.pdu), pdu_idx, NULL);
        if (o->pdu->dir != PduDirectionTx) continue;

        /* No schedule, Tx on-change only. */
        if (o->schedule.interval == 0) {
            if (o->container.header != HeaderFormatNone) {
                /* Container PDU. */
                /* Call to pdunet_visit_container_mapto() will evaluate the
                Container PDU and adjust needs_tx/checksum accordingly. */
                o->needs_tx = true;
                continue;
            } else {
                /* PDU / I-PDU. */
                /* Always calculate signals (and send PDU's if the
                 resultant payload has changed). */
                log_trace(
                    net->log, "Schedule TX: PDU %u: on_change", o->pdu->id);
                _set_skip(
                    net, o->matrix.range.offset, o->matrix.range.count, false);
                continue;
            }
        }

        /* Schedule: determine if schedule will fire. */
        int32_t epoch_offset =
            (net->schedule.step_size)
                ? net->schedule.epoch_offset / net->schedule.step_size
                : 0;
        int32_t base = epoch_offset + o->schedule.phase;
        log_trace(net->log,
            "Schedule TX: PDU %u @ %u: base=%d, epoch_offset=%d, "
            "phase=%u, interval=%u",
            o->pdu->id, net->schedule.simulation_time, base, epoch_offset,
            o->schedule.phase, o->schedule.interval);

        /* Schedule: did not fire.*/
        if (base < 0 || net->schedule.simulation_time < (uint32_t)base) {
            if (o->checksum == 0) {
                log_trace(net->log, "Schedule TX: PDU %u: pending", o->pdu->id);
            } else {
                log_trace(net->log, "Schedule TX: PDU %u: wait", o->pdu->id);
            }
            continue;
        }
        uint32_t delta = net->schedule.simulation_time - base;
        if (delta % o->schedule.interval != 0) {
            if (o->checksum == 0) {
                log_trace(net->log, "Schedule TX: PDU %u: pending", o->pdu->id);
            } else {
                log_trace(net->log, "Schedule TX: PDU %u: wait", o->pdu->id);
            }
            continue;
        }

        /* Schedule: fired. */

        /* Container PDU. */
        if (o->container.header != HeaderFormatNone) {
            /* Call to pdunet_visit_container_mapto() will evaluate
            the Container PDU and adjust needs_tx/checksum accordingly. */
            log_trace(
                net->log, "Schedule TX: Container PDU %u: trigger", o->pdu->id);
            o->needs_tx = true;
            continue;
        }

        /* PDU / I-PDU. */
        _set_skip(net, o->matrix.range.offset, o->matrix.range.count, false);
        if (o->schedule.trigger == PduScheduleTriggerPeriodic) {
            /* Periodic Schedule: PDU Tx occurs according to the schedule. */
            log_trace(net->log, "Schedule TX: PDU %u: trigger - periodic",
                o->pdu->id);
            o->checksum = 0; /* Force Tx according to schedule. */
        } else {
            /* On Change Schedule: PDU Tx only if signals have changed. */
            log_trace(
                net->log, "Schedule TX: PDU %u: trigger - change ", o->pdu->id);
        }
    }
}


PduNetworkNCodecVTable pdunet_network_factory(PduNetwork* net)
{
    YamlNode* md = dse_yaml_find_node(net->doc, "spec/metadata/flexray");
    if (md) {
        net->network.transport_type = NCodecPduTransportTypeFlexray;
        return (PduNetworkNCodecVTable){
            .parse_network = pdunet_flexray_parse_network,
            .parse_pdu = pdunet_flexray_parse_pdu,
            .config = pdunet_flexray_config,
            .lpdu_tx = pdunet_flexray_lpdu_tx,
            .lpdu_rx = pdunet_flexray_lpdu_rx,
        };
    }

    const char* pdunet_label = NULL;
    dse_yaml_get_string(net->doc, "metadata/labels/pdunet", &pdunet_label);
    if (pdunet_label && strcmp(pdunet_label, "can") == 0) {
        net->network.transport_type = NCodecPduTransportTypeCan;
        return (PduNetworkNCodecVTable){
            .parse_network = pdunet_can_parse_network,
            .parse_pdu = pdunet_can_parse_pdu,
            .config = pdunet_can_config,
            .lpdu_tx = pdunet_can_lpdu_tx,
            .lpdu_rx = pdunet_can_lpdu_rx,
        };
    }

    return (PduNetworkNCodecVTable){ 0 };
}


static void* _object_match_handler(void* object)
{
    return object;  // YamlNode
}


void pdunet_parse_pdus(PduNetwork* net, PdunetSchemaObject* object)
{
    uint32_t index = 0;
    do {
        YamlNode* n = pdunet_schema_object_enumerator(
            object, "spec/pdus", &index, _object_match_handler);
        if (n == NULL) break;
        PduItem pdu = pdunet_pdu_generator(net, n);
        if (pdu.id == 0) continue;
        vector_push(&net->pdus, &pdu);
    } while (1);
}


PduItem pdunet_pdu_generator(PduNetwork* net, YamlNode* n)
{
    PduItem pdu = { .id = 0 };

    static const PdunetSchemaFieldMapSpec dir_map[] = {
        { "Rx", PduDirectionRx },
        { "Tx", PduDirectionTx },
        { NULL },
    };
    static const PdunetSchemaFieldMapSpec header_map[] = {
        { "Static", HeaderFormatStatic },
        { "Short", HeaderFormatShort },
        { "Full", HeaderFormatFull },
        { NULL },
    };
    static const PdunetSchemaFieldMapSpec trigger_map[] = {
        { "Change", PduScheduleTriggerChange },
        { "Periodic", PduScheduleTriggerPeriodic },
        { NULL },
    };
    static const PdunetSchemaFieldSpec spec[] = {
        // clang-format off
        { S, "pdu", offsetof(PduItem, name) },
        { U32, "id", offsetof(PduItem, id) }, // Indicates successful parsing.
        { U32, "length", offsetof(PduItem, length) },
        { U8, "dir", offsetof(PduItem, dir), dir_map },
        { U8, "container/header", offsetof(PduItem, container.header), header_map },
        { U32, "container/id", offsetof(PduItem, container.id) },
        { U32, "container/priority", offsetof(PduItem, container.priority) },
        { D, "schedule/phase", offsetof(PduItem, schedule.phase) },
        { D, "schedule/interval", offsetof(PduItem, schedule.interval) },
        { U8, "schedule/trigger", offsetof(PduItem, schedule.trigger),  trigger_map },
        // clang-format on
    };
    pdunet_schema_load_object(net, n, &pdu, spec, ARRAY_SIZE(spec));
    pdunet_load_lua_func(n, "functions/encode", &pdu.lua.encode);
    pdunet_load_lua_func(n, "functions/decode", &pdu.lua.decode);
    pdunet_load_lua_func(n, "functions/tx", &pdu.lua.tx);
    pdunet_load_lua_func(n, "functions/rx", &pdu.lua.rx);

    // Metadata.
    if (net->network.vtable.parse_pdu) {
        net->network.vtable.parse_pdu(net, &pdu, n);
    }

    // Parse spec/pdus/signals.
    YamlNode* sigs = dse_yaml_find_node(n, "signals");
    if (sigs) {
        pdu.signals = vector_make(sizeof(PduSignalItem), 10, NULL);
        for (uint32_t i = 0; i < hashlist_length(&sigs->sequence); i++) {
            YamlNode*     sig = hashlist_at(&sigs->sequence, i);
            PduSignalItem signal = pdunet_signal_generator(net, sig, &pdu);
            if (signal.name == NULL) continue;
            vector_push(&pdu.signals, &signal);
        }
    }

    // Return the object.
    return pdu;
}


PduSignalItem pdunet_signal_generator(
    PduNetwork* net, YamlNode* n, PduItem* pdu)
{
    PduSignalItem signal = { .factor = NAN,
        .offset = NAN,
        .min = NAN,
        .max = NAN,
        .start_bit = 0xffff };

    static const PdunetSchemaFieldSpec spec[] = {
        // clang-format off
        { S, "signal", offsetof(PduSignalItem, name) },
        // Encoding parameters.
        { U16, "encoding/start", offsetof(PduSignalItem, start_bit) },
        { U16, "encoding/length", offsetof(PduSignalItem, length_bits) },
        { D, "encoding/factor", offsetof(PduSignalItem, factor) },
        { D, "encoding/offset", offsetof(PduSignalItem, offset) },
        { D, "encoding/min", offsetof(PduSignalItem, min) },
        { D, "encoding/max", offsetof(PduSignalItem, max) },
        // clang-format on
    };
    pdunet_schema_load_object(net, n, &signal, spec, ARRAY_SIZE(spec));

    bool is_valid = true;
    if (signal.factor == 0.0) {
        if (net->log->level != LOG_QUIET)
            log_error(net->log, "Invalid signal encoding: factor=0.0 (%s)",
                signal.name);
        is_valid = false;
    }
    if (signal.start_bit == 0xffff) {
        if (net->log->level != LOG_QUIET)
            log_error(net->log,
                "Invalid signal encoding: start not specified (%s)",
                signal.name);
        is_valid = false;
    }
    if (signal.length_bits == 0) {
        if (net->log->level != LOG_QUIET)
            log_error(net->log,
                "Invalid signal encoding: length not specified (%s)",
                signal.name);
        is_valid = false;
    }
    if (signal.start_bit > (pdu->length * 8)) {
        if (net->log->level != LOG_QUIET)
            log_error(net->log,
                "Invalid signal encoding: start is beyond payload "
                "length (%s)",
                signal.name);
        is_valid = false;
    }
    if ((signal.start_bit + signal.length_bits) > (pdu->length * 8)) {
        if (net->log->level != LOG_QUIET)
            log_error(net->log,
                "Invalid signal encoding: length is beyond payload "
                "length (%s)",
                signal.name);
        is_valid = false;
    }
    if (is_valid == false) {
        signal.name = NULL;  // Caller will reject.
    }

    pdunet_load_lua_func(n, "functions/encode", &signal.lua.encode);
    pdunet_load_lua_func(n, "functions/decode", &signal.lua.decode);

    return signal;
}


void pdunet_build_msm(PduNetwork* net, const char* name, size_t count,
    const char** signal, double* scalar)
{
    assert(net);
    assert(name);
    assert(signal);
    assert(scalar);

    MarshalMapSpec signal_spec = (MarshalMapSpec){
        .name = name,
        .count = count,
        .signal = signal,
        .scalar = scalar,
    };

    /* Loop over matrix ranges and emit a MSM for each range. */
    Vector in = vector_make(sizeof(MarshalSignalMap), 0, NULL);
    Vector out = vector_make(sizeof(MarshalSignalMap), 0, NULL);
    for (size_t range_idx = 0; range_idx < vector_len(&net->matrix.range);
        range_idx++) {
        PduRange* r = vector_at(&net->matrix.range, range_idx, NULL);
        assert(r);
        MarshalMapSpec source_spec = (MarshalMapSpec){
            .count = r->length,
            .signal = (const char**)vector_at(
                &net->matrix.signal.name, r->offset, NULL),
            .scalar =
                (double*)vector_at(&net->matrix.signal.phys, r->offset, NULL),
        };

        /* Build the MSM object for this range item. */
        errno = 0;
        MarshalSignalMap* msm =
            marshal_generate_signalmap(signal_spec, source_spec, NULL, false);
        if (errno != 0 || msm == NULL) {
            log_info(net->log,
                "Call to marshal_generate_signalmap() failed: errno=%d", errno);
            continue;
        }
        log_debug(net->log, "MSM for range[%d]: dir=%u, offset=%u, length=%u",
            range_idx, r->dir, r->offset, r->length);

        /* Save the offset, used when logging the map. */
        msm->offset = r->offset;

        /* Shallow copy the MSM object into a vector. */
        switch (r->dir) {
        case PduDirectionRx:
            vector_push(&in, msm);
            break;
        case PduDirectionTx:
            vector_push(&out, msm);
            break;
        default:
            break;
        }
        free(msm);
    }

    /* Add empty MSM object to each vector (creating an NTL). */
    vector_push(&in, &(MarshalSignalMap){});
    vector_push(&out, &(MarshalSignalMap){});

    /* Use the internal vector objects, which are NTLs. */
    net->msm.in = in.items;
    net->msm.out = out.items;
}


/*
pdunet_parse
============

Parameters
----------
net (PduNetwork*)
: PduNetwork object.
*/
int pdunet_parse(PduNetwork* net, void* doc)
{
    assert(net);
    assert(doc);

    const char* _v = NULL;
    dse_yaml_get_string(doc, "kind", &_v);
    if (_v == NULL || strcmp(_v, "Network") != 0) {
        log_error(net->log, "Wrong schema kind (have:%s, want:Network)", _v);
        return -EINVAL;
    }

    net->doc = doc;
    net->network.vtable = pdunet_network_factory(net);

    // Parse metadata/name.
    dse_yaml_get_string(net->doc, "metadata/name", &net->name);
    log_notice(net->log, "  Parsing network: %s", net->name);

    // Parse schedule.
    static const PdunetSchemaFieldSpec spec[] = {
        // clang-format off
        { D, "spec/schedule/epoch_offset", offsetof(PduNetwork, schedule.epoch_offset) },
        // clang-format on
    };
    pdunet_schema_load_object(net, net->doc, net, spec, ARRAY_SIZE(spec));

    // Parse spec/metadata.
    if (net->network.vtable.parse_network) {
        net->network.vtable.parse_network(net);
    }

    // Parse spec/pdus.
    PdunetSchemaObject object = {
        .doc = net->doc,
    };
    pdunet_parse_pdus(net, &object);

    return 0;
}


/*
pdunet_transform
================

Parameters
----------
net (PduNetwork*)
: PduNetwork object.
*/
int pdunet_transform(PduNetwork* net, PduNetworkSortFunc sort)
{
    int rc = pdunet_matrix_transform(net, sort);
    if (rc) return rc;

    pdunet_visit(net, NULL, pdunet_visit_setup_containers, NULL);
    return 0;
}


static int _sort_mpdu(const void* left, const void* right)
{
    const MPduItem* l = left;
    const MPduItem* r = right;
    if (l->priority < r->priority) return -1;
    if (l->priority > r->priority) return 1;
    return 0;
}


static void pdunet_visit_map_pdu(PduNetwork* net, PduObject* pdu, void* data)
{
    if (pdu == NULL || pdu->pdu == NULL) return;
    PduObject* c_pdu = data;
    if (c_pdu == NULL || c_pdu->pdu == NULL) return;
    if (pdu->pdu->dir != c_pdu->pdu->dir) return;

    if (pdu->pdu->container.id == c_pdu->pdu->id) {
        vector_push(&c_pdu->container.pdu_list,
            &(MPduItem){
                .id = pdu->pdu->id,
                .priority = pdu->pdu->container.priority,
                .pdu = pdu,
            });
        log_trace(net->log,
            "Container: [%u] L-PDU[%u] <-map- [%u] I-PDU[%u], priority=%u",
            c_pdu->matrix.pdu_idx, c_pdu->pdu->id, pdu->matrix.pdu_idx,
            pdu->pdu->id, pdu->pdu->container.priority);
    }
}


void pdunet_visit_setup_containers(PduNetwork* net, PduObject* pdu, void* data)
{
    UNUSED(data);
    if (pdu == NULL || pdu->pdu == NULL) return;
    if (pdu->container.header == HeaderFormatNone) return;

    /* Setup the Container mapping. */
    pdu->container.pdu_list = vector_make(sizeof(MPduItem), 4, _sort_mpdu);
    pdunet_visit(net, NULL, pdunet_visit_map_pdu, pdu);
    vector_sort(&pdu->container.pdu_list);
}

static size_t header_length[] = {
    [HeaderFormatNone] = 0,
    [HeaderFormatStatic] = 0,
    [HeaderFormatShort] = 4,
    [HeaderFormatFull] = 8,
};

void pdunet_visit_container_mapto(PduNetwork* net, PduObject* pdu, void* data)
{
    UNUSED(data);
    assert(net);
    if (pdu == NULL || pdu->pdu == NULL) return;
    if (pdu->pdu->dir != PduDirectionTx) return;
    if (pdu->container.header == HeaderFormatNone) return;
    if (pdu->needs_tx == false) return;

    // L-PDU.
    uint8_t* payload = NULL;
    vector_at(&(net->matrix.payload), pdu->matrix.pdu_idx, &payload);
    assert(payload);
    size_t payload_len = pdu->pdu->length;
    size_t payload_offset = 0;
    memset(payload, 0, payload_len);

    pdu->needs_tx = false; /* Will be set to true if I-PDU mapped. */

    /* I-PDUs (sorted by container.priority). */
    size_t count = vector_len(&pdu->container.pdu_list);
    for (size_t i = 0; i < count; i++) {
        MPduItem* pi = vector_at(&pdu->container.pdu_list, i, NULL);
        assert(pi);
        assert(pi->pdu);
        assert(pi->pdu->pdu);
        if (pi->pdu->needs_tx != true) continue;

        size_t len = pi->pdu->pdu->length;
        if ((len + header_length[pdu->container.header]) >
            (payload_len - payload_offset)) {
            // No room for this PDU, but others might still fit, so continue.
            log_trace(net->log,
                "Map to Container: [%u] L-PDU[%u] <-DELAY- [%u] I-PDU[%u]",
                pdu->matrix.pdu_idx, pdu->pdu->id, pi->pdu->pdu->id,
                pi->pdu->matrix.pdu_idx);
            pi->pdu->needs_tx = false;
            pi->pdu->checksum = 0;  // Triggers update of needs_tx on next.
            continue;
        }

        /* Update the checksum. The call to tx_func() may modify payload, the
        checksum must be on the basis of pdunet_encode_pack() as that
        value is used to determine if needs_tx will be set. */
        uint8_t* pi_payload = NULL;
        vector_at(&(net->matrix.payload), pi->pdu->matrix.pdu_idx, &pi_payload);
        assert(pi_payload);
        pi->pdu->checksum = pdunet_checksum(pi_payload, len);

        /* Apply Tx payload modifications to this I-PDU. */
        pdunet_call_tx_func(net, pi->pdu);
        if (pi->pdu->needs_tx == false) {
            /* This PDU was rejected. */
            continue;
        }

        /* Map in this I-PDU. */
        log_trace(net->log,
            "Map to Container: [%u] L-PDU[%u] <-map- [%u] I-PDU[%u], "
            "offset=%u, len=%u/%u",
            pdu->matrix.pdu_idx, pdu->pdu->id, pi->pdu->pdu->id,
            pi->pdu->matrix.pdu_idx, payload_offset, len,
            len + header_length[pdu->container.header]);
        switch (pdu->container.header) {
        case HeaderFormatStatic:
            break;
        case HeaderFormatShort:
            len = (uint8_t)len;
            payload[payload_offset + 0] = (uint8_t)(pi->id >> 16);
            payload[payload_offset + 1] = (uint8_t)(pi->id >> 8);
            payload[payload_offset + 2] = (uint8_t)pi->id;
            payload[payload_offset + 3] = (uint8_t)len;
            break;
        case HeaderFormatFull:
            len = (uint32_t)len;
            payload[payload_offset + 0] = (uint8_t)(pi->id >> 24);
            payload[payload_offset + 1] = (uint8_t)(pi->id >> 16);
            payload[payload_offset + 2] = (uint8_t)(pi->id >> 8);
            payload[payload_offset + 3] = (uint8_t)pi->id;
            payload[payload_offset + 4] = (uint8_t)(len >> 24);
            payload[payload_offset + 5] = (uint8_t)(len >> 16);
            payload[payload_offset + 6] = (uint8_t)(len >> 8);
            payload[payload_offset + 7] = (uint8_t)len;
            break;
        default:
            break;
        }
        payload_offset += header_length[pdu->container.header];
        memcpy(payload + payload_offset, pi_payload, len);
        payload_offset += len;

        /* Send the Container (which now contains this I-PDU). */
        pdu->needs_tx = true;
        pi->pdu->needs_tx = false;
    }
    /* Apply Tx payload modifications to this Container PDU. */
    pdunet_call_tx_func(net, pdu);
}

void pdunet_visit_container_mapfrom(PduNetwork* net, PduObject* pdu, void* data)
{
    UNUSED(data);
    assert(net);
    if (pdu == NULL || pdu->pdu == NULL) return;
    if (pdu->pdu->dir != PduDirectionRx) return;
    if (pdu->container.header == HeaderFormatNone) return;
    if (pdu->update_signals == false) return;

    // L-PDU.
    uint8_t* payload = NULL;
    vector_at(&(net->matrix.payload), pdu->matrix.pdu_idx, &payload);
    assert(payload);
    size_t payload_len = pdu->pdu->length;
    size_t payload_offset = 0;

    while (payload_offset < payload_len) {
        uint32_t id = 0;
        size_t   len = 0;
        // Is there a header.
        switch (pdu->container.header) {
        case HeaderFormatStatic:
            break;
        case HeaderFormatShort:
            if (header_length[pdu->container.header] <=
                (payload_len - payload_offset)) {
                id = (payload[payload_offset + 0] << 16) |
                     (payload[payload_offset + 1] << 8) |
                     payload[payload_offset + 2];
                len = payload[payload_offset + 3];
            }
            payload_offset += header_length[pdu->container.header];
            break;
        case HeaderFormatFull:
            if (header_length[pdu->container.header] <=
                (payload_len - payload_offset)) {
                id = (payload[payload_offset + 0] << 24) |
                     (payload[payload_offset + 1] << 16) |
                     (payload[payload_offset + 2] << 8) |
                     payload[payload_offset + 3];
                len = (payload[payload_offset + 4] << 24) |
                      (payload[payload_offset + 5] << 16) |
                      (payload[payload_offset + 6] << 8) |
                      payload[payload_offset + 7];
            }
            payload_offset += header_length[pdu->container.header];
            break;
        default:
            break;
        }
        if (id == 0) {
            // No more mapped I-PDUs in payload.
            break;
        }
        if ((payload_offset + len) > payload_len) {
            // Length exceeds remaining payload.
            break;
        }
        // Locate the I-PDU and copy the payload.
        size_t count = vector_len(&pdu->container.pdu_list);
        for (size_t i = 0; i < count; i++) {
            MPduItem* pi = vector_at(&pdu->container.pdu_list, i, NULL);
            assert(pi);
            assert(pi->pdu);
            if (pi->id != id) continue;

            // I-PDU - call rx function.
            int rc = pdunet_call_rx_func(
                net, pi->pdu, payload + payload_offset, len);
            if (rc != 0) continue; /* Discarded. */

            // I-PDU - copy payload
            uint8_t* pi_payload = NULL;
            vector_at(
                &(net->matrix.payload), pi->pdu->matrix.pdu_idx, &pi_payload);
            assert(pi_payload);
            size_t pi_payload_len = pi->pdu->pdu->length;
            if (pi_payload_len > len) {
                pi_payload_len = len;
            }
            memcpy(pi_payload, payload + payload_offset, pi_payload_len);
            log_debug(net->log,
                "Map from Container: [%u] L-PDU[%u] <-map- [%u] I-PDU[%u], "
                "offset=%u, len=%u/%u",
                pdu->matrix.pdu_idx, pdu->pdu->id, pi->pdu->pdu->id,
                pi->pdu->matrix.pdu_idx, payload_offset, len,
                len + header_length[pdu->container.header]);
            break;
        }
        // Position to the next mapped I-PDU.
        payload_offset += len;
    }
}


/*
pdunet_configure
================

Parameters
----------
net (PduNetwork*)
: PduNetwork object.
*/
int pdunet_configure(PduNetwork* net)
{
    assert(net);
    int rc = 0;

    rc = pdunet_lua_setup(net);
    if (rc != 0) return rc;

    assert(net);
    lua_State* L = net->lua.lua_state;

    pdunet_parse_network_functions(net);
    if (net->lua.global != NULL) {
        pdunet_lua_install_script(net, L, net->lua.global);
    }

    size_t pdu_count = vector_len(&net->pdus);
    for (size_t pdu_idx = 0; pdu_idx < pdu_count; pdu_idx++) {
        PduItem* p = vector_at(&net->pdus, pdu_idx, NULL);
        // PDU -> install lua func.
        p->lua.encode_ref = pdunet_lua_install_script(net, L, p->lua.encode);
        p->lua.decode_ref = pdunet_lua_install_script(net, L, p->lua.decode);

        p->lua.tx_ref = pdunet_lua_install_script(net, L, p->lua.tx);
        p->lua.rx_ref = pdunet_lua_install_script(net, L, p->lua.rx);

        for (size_t sig_idx = 0; sig_idx < vector_len(&p->signals); sig_idx++) {
            PduSignalItem* s = vector_at(&p->signals, sig_idx, NULL);
            // Signal -> install lua func.
            s->lua.encode_ref =
                pdunet_lua_install_script(net, L, s->lua.encode);
            s->lua.decode_ref =
                pdunet_lua_install_script(net, L, s->lua.decode);
        }
    }

    return rc;
}
