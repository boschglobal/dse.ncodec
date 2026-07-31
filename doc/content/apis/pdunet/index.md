---
title: PDU Network API Reference
linkTitle: PDUNet
---
## PDU Network


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

### Example


{{< readfile file="examples/pdunet_api.c" code="true" lang="c" >}}




## Typedefs

## Functions

### pdunet_create

Create and configure a `PduNetworkDesc` object to represent a PDU Network.

#### Parameters

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

#### Returns

PDUNET*
: PDU Network object, or NULL if required arguments are invalid or allocation
  fails.



### pdunet_destroy

Destroy a PDU Network object and release all resources owned by it.

#### Parameters

n (PDUNET*)
: PDU Network object.

#### Returns

None.



### pdunet_map_signals

Map external signal vectors to the PDU Network signal matrix.

#### Parameters

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

#### Returns

int
: 0 when the signal mapping was created, non-zero otherwise.



### pdunet_rx

Receive PDUs from the configured NCodec object. If a visitor is provided, then
call the visitor after a PDU is received.

#### Parameters

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

#### Returns

None.



### pdunet_tx

Transmit PDUs to the configured NCodec object. If a visitor is provided, then
call the visitor before transmitting a PDU, and only transmit the PDU
if `needs_tx` is set on the `PduObject` after the visitor returns.

#### Parameters

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

#### Returns

None.



### pdunet_visit

Call a visitor function for each PDU in the PDU Network.

#### Parameters

n (PDUNET*)
: PDU Network object.

r (PDURANGE*)
: Range object, optional. When NULL the visitor function is called for all PDUs
  in the PDU Network.

visit (PduNetworkVisitFunc)
: Visit callback function called for each PDU object in the provided range.

data (void*)
: Data object passed to the visit callback function. Optional.

#### Returns

None.



