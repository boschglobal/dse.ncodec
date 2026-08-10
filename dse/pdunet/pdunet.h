// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_PDUNET_PDUNET_H_
#define DSE_PDUNET_PDUNET_H_

#include <stdint.h>
#include <lua.h>
#include <dse/platform.h>


/* DLL Interface visibility. */
#if defined _WIN32 || defined __CYGWIN__
#ifdef DLL_BUILD
#define DLL_PUBLIC __declspec(dllexport)
#else
#define DLL_PUBLIC __declspec(dllimport)
#endif /* DLL_BUILD */
#define DLL_PRIVATE
#else /* Linux */
#define DLL_PUBLIC  __attribute__((visibility("default")))
#define DLL_PRIVATE __attribute__((visibility("hidden")))
#endif /* _WIN32 || defined __CYGWIN__ */


/**
PDU Network
===========

The PDU Network (PDUNet) provides a network-level abstraction for
exchanging PDUs through an `NCODEC` instance. It parses a network description,
builds the required PDU and signal mapping structures, schedules cyclic
transmissions, and handles Rx/Tx processing for PDUs. PDUNet supports a variety
of PDU formats and layouts, including Container PDUs and Multiplex PDUs.

It also marshals values between external signal vectors and the internal PDU
payload representation, allowing simulation signals to be encoded into network
PDUs and decoded back into signal values. This effectively creates a "restbus"
simulation.

PDUNet can optionally use Lua callbacks to customize Rx/Tx processing. These
callbacks can inspect or modify PDU payloads and may also reject PDUs before
they are transmitted or processed. The Lua callback code is defined directly in
the PDUNet YAML configuration file.

Example
-------

{{< readfile file="examples/pdunet_api.c" code="true" lang="c" >}}

*/

typedef struct PduNetwork PduNetwork;
typedef struct PduObject  PduObject;
typedef struct PduRange   PduRange;

typedef void (*PduNetworkSortFunc)(
    PduNetwork* net, PduRange* range, void* data);
typedef void (*PduNetworkVisitFunc)(
    PduNetwork* net, PduObject* pdu, void* data);


/* Primary API. */
DLL_PUBLIC PduNetwork* pdunet_create(
    void* ncodec, void* doc, double step_size, lua_State* L, DseLog* log);
DLL_PUBLIC void pdunet_destroy(PduNetwork* net);

DLL_PUBLIC int pdunet_map_signals(PduNetwork* net, const char* name,
    size_t count, const char** signal, double* scalar);


/* Secondary API. */
DLL_PUBLIC int pdunet_sort(PduNetwork* net, PduNetworkSortFunc sort);

DLL_PUBLIC void pdunet_visit(
    PduNetwork* net, PduRange* range, PduNetworkVisitFunc visit, void* data);
DLL_PUBLIC void pdunet_tx(PduNetwork* net, PduRange* range,
    PduNetworkVisitFunc visit, void* data, double simulation_time);
DLL_PUBLIC void pdunet_rx(
    PduNetwork* net, PduRange* range, PduNetworkVisitFunc visit, void* data);

DLL_PUBLIC void pdunet_visit_clear_update_flag(
    PduNetwork* net, PduObject* pdu, void* data);
DLL_PUBLIC void pdunet_visit_clear_tx_flag(
    PduNetwork* net, PduObject* pdu, void* data);
DLL_PUBLIC void pdunet_visit_clear_checksum(
    PduNetwork* net, PduObject* pdu, void* data);
DLL_PUBLIC void pdunet_visit_set_checksum(
    PduNetwork* net, PduObject* pdu, void* data);
DLL_PUBLIC void pdunet_visit_needs_tx(
    PduNetwork* net, PduObject* pdu, void* data);

DLL_PUBLIC void pdunet_call_tx_func(PduNetwork* net, PduObject* pdu);
DLL_PUBLIC int  pdunet_call_rx_func(
     PduNetwork* net, PduObject* pdu, uint8_t* payload, size_t payload_len);

#endif  // DSE_PDUNET_PDUNET_H_
