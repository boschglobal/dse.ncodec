// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <dse/log.h>
#include <dse/clib/collections/vector.h>
#include <dse/clib/data/marshal.h>
#include <dse/ncodec/codec.h>
#include <dse/pdunet/network/network.h>
#include <dse/pdunet/internal.h>
#include <dse/pdunet/pdunet.h>


#define UNUSED(x)               ((void)x)
#define ARRAY_SIZE(a)           (sizeof(a) / sizeof((a)[0]))
#define MODEL_DEFAULT_STEP_SIZE 0.0005


/**
pdunet_create
=============

Create and configure a `PduNetworkDesc` object to represent a PDU Network.

Parameters
----------
nc (NCODEC*)
: NCodec object used for PDU transmission and reception.

doc (void*)
: Network document object to parse and configure.

step_size (double)
: Simulation step size. When less than or equal to zero,
  `MODEL_DEFAULT_STEP_SIZE` is used.

L (lua_State*)
: Lua state used for optional PDU Rx/Tx callback functions.

log (DseLog*)
: Logger object. When NULL, the default logger is used.

Returns
-------
PDUNET*
: PDU Network object, or NULL if required arguments are invalid or allocation
  fails.
*/
PDUNET* pdunet_create(
    NCODEC* nc, void* doc, double step_size, lua_State* L, DseLog* log)
{
    log_notice(log, "PDU Net: Create Network");
    if (nc == NULL) {
        log_fatal(log, "NCodec object is NULL");
        errno = EINVAL;
        return NULL;
    }
    if (doc == NULL) {
        log_fatal(log, "Doc object is NULL");
        errno = EINVAL;
        return NULL;
    }
    PduNetworkDesc* net = calloc(1, sizeof(PduNetworkDesc));
    if (net == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    *net = (PduNetworkDesc){
        .ncodec = nc,
        .doc = doc,
        .schedule.step_size = MODEL_DEFAULT_STEP_SIZE,
        .pdus = vector_make(sizeof(PduItem), 10, NULL),
        .lua.lua_state = L,
        .log = log,
    };
    if (net->log == NULL) {
        net->default_log = dse_log_default();
        net->log = &net->default_log;
    }
    if (step_size > 0) {
        net->schedule.step_size = step_size;
    }
    net->schedule.step_size_epsilon = net->schedule.step_size * 0.01;
    log_notice(net->log, "PDU Net: Step size: %f", net->schedule.step_size);

    /* Parse the network. */
    int rc;
    log_notice(net->log, "PDU Net: Parse Network");
    rc = pdunet_parse(net, net->doc);
    if (rc == 0) {
        rc = pdunet_configure(net);
        if (rc != 0) {
            log_error(net->log, "Configure fail: rc=%d", rc);
            return net;
        }
        rc = pdunet_transform(net, NULL);
        if (rc != 0) {
            log_error(net->log, "Transform fail: rc=%d", rc);
            return net;
        }
    } else if (rc == -ENODATA) {
        log_fatal(net->log,
            "Parse fail: Network not found in YAML files (rc=%d)", rc);
    } else {
        log_fatal(net->log, "Parse fail: rc=%d", rc);
        return net;
    }

    /* Set initial conditions for the network. */
    pdunet_visit(net, NULL, pdunet_visit_set_checksum, NULL);
    pdunet_visit(net, NULL, pdunet_visit_clear_tx_flag, NULL);

    return net;
}


/**
pdunet_map_signals
==================

Map external signal vectors to the PDU Network signal matrix.

Parameters
----------
n (PDUNET*)
: PDU Network object.

name (const char*)
: Name of the signal group to map.

count (size_t)
: Number of entries in the signal and scalar arrays.

signal (const char**)
: Array of signal names.

scalar (double*)
: Array of scalar signal values to marshal to and from the PDU Network.

Returns
-------
int
: 0 when the signal mapping was created, non-zero otherwise.
*/
int pdunet_map_signals(PDUNET* n, const char* name, size_t count,
    const char** signal, double* scalar)
{
    PduNetworkDesc* net = __pdunet(n);
    log_notice(
        net->log, "PDU Net: Search for SignalGroup (Network=%s)", net->name);
    pdunet_build_msm(net, name, count, signal, scalar);
    if ((net->msm.in == NULL) && (net->msm.out == NULL)) {
        log_error(net->log, "Marshal table not created");
        return 1;
    }


    /* Setup marshalling to signals. */
    log_notice(net->log, "  SignalVector <-> Network Rx Mapping for: %s", name);
    for (uint32_t i = 0; net->msm.in && i < net->msm.in->count; i++) {
        log_notice(net->log, "    Signal: %s (%d) <-> %s (%d)",
            signal[net->msm.in->signal.index[i]], net->msm.in->signal.index[i],
            *(const char**)vector_at(&net->matrix.signal.name,
                net->msm.in->source.index[i] + net->msm.in->offset, NULL),
            net->msm.in->source.index[i] + net->msm.in->offset);
    }
    log_notice(net->log, "  SignalVector <-> Network Tx Mapping for: %s", name);
    for (uint32_t i = 0; net->msm.out && i < net->msm.out->count; i++) {
        log_notice(net->log, "    Signal: %s (%d) <-> %s (%d)",
            signal[net->msm.out->signal.index[i]],
            net->msm.out->signal.index[i],
            *(const char**)vector_at(&net->matrix.signal.name,
                net->msm.out->source.index[i] + net->msm.out->offset, NULL),
            net->msm.out->source.index[i] + net->msm.out->offset);
    }
    return 0;
}


/**
pdunet_visit
============

Call a visitor function for each PDU in the PDU Network.

Parameters
----------
n (PDUNET*)
: PDU Network object.

r (PDURANGE*)
: Range object, optional. When NULL the visitor function is called for all PDUs
  in the PDU Network.

visit (PduNetworkVisitFunc)
: Visit callback function called for each PDU object in the provided range.

data (void*)
: Data object passed to the visit callback function. Optional.

Returns
-------
None.
*/
void pdunet_visit(PDUNET* n, PDURANGE* r, PduNetworkVisitFunc visit, void* data)
{
    UNUSED(r);
    PduNetworkDesc* net = __pdunet(n);
    if (net == NULL || visit == NULL) return;

    for (size_t i = 0; i < vector_len(&net->matrix.pdu); i++) {
        PduObject* pdu = vector_at(&net->matrix.pdu, i, NULL);
        if (pdu) visit(net, pdu, data);
    }
}


/**
pdunet_tx
=========

Transmit PDUs to the configured NCodec object. If a visitor is provided, then
call the visitor before transmitting a PDU, and only transmit the PDU
if `needs_tx` is set on the `PduObject` after the visitor returns.

Parameters
----------
n (PDUNET*)
: PDU Network object.

r (PDURANGE*)
: Range object, optional. When NULL the visitor function is called for all PDUs
  in the PDU Network.

visit (PduNetworkVisitFunc)
: Visit callback function called before transmission for each PDU object in the
  provided range. Optional.

data (void*)
: Data object passed to the visit callback function. Optional.

simulation_time (double)
: Current simulation time used to schedule cyclic PDU transmission. Negative
  values are treated as zero.

Returns
-------
None.
*/
void pdunet_tx(PDUNET* n, PDURANGE* r, PduNetworkVisitFunc visit, void* data,
    double simulation_time)
{
    PduNetworkDesc* net = __pdunet(n);
    PduRange*       range = __pdurange(r);

    if (net == NULL) return;
    if (simulation_time < 0) simulation_time = 0.0;

    /* Marshal from SignalVector to PDU Network. */
    marshal_signalmap_out(net->log, net->msm.out);

    log_debug(net->log, "PDU Net: TX");
    ncodec_truncate(net->ncodec);

    /* Configuration (if network requires). */
    if (net->network.vtable.config_done == false) {
        if (net->network.vtable.config) {
            net->network.vtable.config(net);
        }
        net->network.vtable.config_done = true;
    }

    /* Schedule, based on normalised simulation time. */
    net->schedule.simulation_time =
        (simulation_time + net->schedule.step_size_epsilon) /
        net->schedule.step_size;
    pdunet_schedule(net);

    /* Encode PDUs, call visitor, then Tx. */
    pdunet_encode_linear(net, NULL);
    pdunet_encode_pack(net, NULL);
    pdunet_visit(net, range, pdunet_visit_needs_tx, NULL);
    pdunet_visit(net, range, pdunet_visit_container_mapto, NULL);
    if (visit) pdunet_visit(net, range, visit, data);
    if (net->network.vtable.lpdu_tx) {
        net->network.vtable.lpdu_tx(net);
    }

    /* Status (if network requires). */
    if (net->network.vtable.status) {
        net->network.vtable.status(net);
    }

    ncodec_flush(net->ncodec);

    /* Marshal from PDU Network to SignalVector (update changed signals). */
    // TODO: trigger on actual Tx to reduce CPU consumption in idle steps.
    marshal_signalmap_in(net->log, net->msm.out);
}


/**
pdunet_rx
=========

Receive PDUs from the configured NCodec object. If a visitor is provided, then
call the visitor after a PDU is received.

Parameters
----------
n (PDUNET*)
: PDU Network object.

r (PDURANGE*)
: Range object, optional. When NULL the visitor function is called for all PDUs
  in the PDU Network.

visit (PduNetworkVisitFunc)
: Visit callback function called after reception for each PDU object in the
  provided range. Optional.

data (void*)
: Data object passed to the visit callback function. Optional.

Returns
-------
None.
*/
void pdunet_rx(PDUNET* n, PDURANGE* r, PduNetworkVisitFunc visit, void* data)
{
    PduNetworkDesc* net = __pdunet(n);
    PduRange*       range = __pdurange(r);

    if (net == NULL) return;

    log_debug(net->log, "PDU Net: RX");
    ncodec_seek(net->ncodec, 0, NCODEC_SEEK_SET);

    /* Receive PDUs, call visitor. */
    if (net->network.vtable.lpdu_rx) {
        net->network.vtable.lpdu_rx(net);
    }
    pdunet_visit(net, NULL, pdunet_visit_container_mapfrom, NULL);
    if (visit) pdunet_visit(net, range, visit, data);

    /* Decode PDUs. */
    pdunet_decode_unpack(net, NULL);
    pdunet_decode_linear(net, NULL);
    pdunet_visit(net, NULL, pdunet_visit_clear_update_flag, NULL);

    /* Marshal from PDU Network to SignalVector. */
    marshal_signalmap_in(net->log, net->msm.in);
}


void pdunet_visit_clear_update_flag(PDUNET* n, PDUOBJECT* o, void* data)
{
    UNUSED(n);
    UNUSED(data);
    PduObject* pdu = __pduobject(o);
    if (pdu) pdu->update_signals = false;
}


void pdunet_visit_clear_tx_flag(PDUNET* n, PDUOBJECT* o, void* data)
{
    UNUSED(n);
    UNUSED(data);
    PduObject* pdu = __pduobject(o);
    if (pdu) pdu->needs_tx = false;
}


void pdunet_visit_clear_checksum(PDUNET* n, PDUOBJECT* o, void* data)
{
    UNUSED(n);
    UNUSED(data);
    PduObject* pdu = __pduobject(o);
    if (pdu) pdu->checksum = 0;
}


void pdunet_visit_set_checksum(PDUNET* n, PDUOBJECT* o, void* data)
{
    UNUSED(data);
    PduNetworkDesc* net = __pdunet(n);
    PduObject*      pdu = __pduobject(o);
    if (pdu == NULL || pdu->pdu == NULL) return;
    if (pdu->pdu->dir == PduDirectionTx) {
        uint8_t* payload = NULL;
        vector_at(&(net->matrix.payload), pdu->matrix.pdu_idx, &payload);
        assert(payload);
        size_t payload_len = pdu->pdu->length;
        pdu->checksum = pdunet_checksum(payload, payload_len);
    }
}


void pdunet_visit_needs_tx(PDUNET* n, PDUOBJECT* o, void* data)
{
    UNUSED(data);
    PduNetworkDesc* net = __pdunet(n);
    PduObject*      pdu = __pduobject(o);
    if (pdu == NULL || pdu->pdu == NULL) return;

    if (pdu->pdu->dir == PduDirectionTx) {
        if (pdu->container.header != HeaderFormatNone) {
            /* Container PDU, preserve the needs_tx set by schedule. Later
            call to pdunet_visit_container_mapto will call tx function. */
        } else {
            uint32_t checksum = pdunet_checksum(
                pdu->ncodec.pdu.payload, pdu->ncodec.pdu.payload_len);
            log_trace(net->log, "Pdu: [%u] checksum=%u, new checksum=%u",
                pdu->matrix.pdu_idx, pdu->checksum, checksum);
            if (checksum != pdu->checksum) {
                pdu->needs_tx = true;
                if (pdu->pdu->container.id == 0) {
                    pdu->checksum = checksum;
                    /* Apply Tx payload modifications. */
                    pdunet_call_tx_func(net, pdu);
                } else {
                    /* Container I-PDU (id != 0) checksums are updated in
                     * pdunet_visit_container_mapto(), and only after being
                     * mapped into the Container (eff. Tx). */
                }
            } else {
                pdu->needs_tx = false;
            }
        }
        log_trace(net->log, "Pdu: [%u] needs_tx=%u", pdu->matrix.pdu_idx,
            pdu->needs_tx);
    } else {
        pdu->needs_tx = false;
    }
}


void pdunet_call_tx_func(PDUNET* n, PDUOBJECT* o)
{
    PduNetworkDesc* net = __pdunet(n);
    PduObject*      pdu = __pduobject(o);
    if (pdu == NULL || pdu->pdu == NULL) return;
    if ((pdu->needs_tx != true) || (pdu->lua.tx_ref == 0)) return;

    /* Evaluate the Lua Tx function which may apply post-checksum payload
    modifications or _reject_ the PDU. */
    assert(net);
    lua_State* L = net->lua.lua_state;

    log_trace(net->log, "Lua Call: PDU Tx Tx[%u]: func=%d", pdu->matrix.pdu_idx,
        pdu->lua.tx_ref);

    int rc = pdunet_lua_pdu_call(net, L, pdu->lua.tx_ref,
        pdu->ncodec.pdu.payload, pdu->ncodec.pdu.payload_len, true);
    if (rc == 0) {
        /* The PDU may have its payload modified. */
        if (pdu->pdu->container.id == 0) {
            /* Basis for checksum comparison (i.e. setting needs_tx) is not
            currently based on this modified payload. Therefore do not update
            the checksum here. */
        } else {
            /* Container I-PDU (id != 0) checksums are updated in
            pdunet_visit_container_mapto(), and only after being
            mapped into the Container (eff. Tx). */
        }
    } else {
        /* The PDU was rejected. */
        pdu->needs_tx = false;
        log_trace(
            net->log, "Pdu: [%u] rejected, reason=%d", pdu->matrix.pdu_idx, rc);
    }
}


int pdunet_call_rx_func(
    PDUNET* n, PDUOBJECT* o, uint8_t* payload, size_t payload_len)
{
    PduNetworkDesc* net = __pdunet(n);
    PduObject*      pdu = __pduobject(o);
    if (pdu == NULL || pdu->pdu == NULL) return 0;
    if (pdu->lua.rx_ref == 0) return 0;

    /* Evaluate the Lua Rx function which may apply payload
    modifications or _reject_ the PDU. */
    assert(net);
    lua_State* L = net->lua.lua_state;

    log_trace(net->log, "Lua Call: PDU Rx Rx[%u]: func=%d", pdu->matrix.pdu_idx,
        pdu->lua.rx_ref);

    int rc = pdunet_lua_pdu_call(
        net, L, pdu->lua.rx_ref, payload, payload_len, true);
    if (rc != 0) {
        log_trace(
            net->log, "Pdu: [%u] rejected, reason=%d", pdu->matrix.pdu_idx, rc);
    }
    return rc;
}


/**
pdunet_destroy
==============

Destroy a PDU Network object and release all resources owned by it.

Parameters
----------
n (PDUNET*)
: PDU Network object.

Returns
-------
None.
*/
void pdunet_destroy(PDUNET* n)
{
    PduNetworkDesc* net = __pdunet(n);

    if (net) {
        for (size_t i = 0; i < vector_len(&net->pdus); i++) {
            PduItem* pdu = vector_at(&net->pdus, i, NULL);
            vector_reset(&pdu->signals);
            if (pdu->metadata.config) free(pdu->metadata.config);
        }
        vector_reset(&net->pdus);
        marshal_signalmap_destroy(net->msm.in);
        marshal_signalmap_destroy(net->msm.out);
        pdunet_matrix_clear(net);
        pdunet_lua_teardown(net);
        if (net->network.metadata.config) free(net->network.metadata.config);
        free(net);
    }
}
