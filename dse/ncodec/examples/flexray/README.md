<!--
Copyright 2025 Robert Bosch GmbH

SPDX-License-Identifier: Apache-2.0
-->

# FlexRay Examples

This examples demonstrate various FlexRay use cases:

```text
dse/ncodec                NCodec API source code.
└── examples/flexray      Example FlexRay implementations.
    └── bridge.c          Example bridging to/from a FlexRay Network.
    └── wup.c             Example demonstrating Wake-up use case.
    └── startup.c         Example demonstrating Startup use case.
    └── txrx.c            Example demonstrating LPDU exchange.
    └── board_stub.c      Stub board API (pin and power).
    └── ecu_stub.c        Stub ECU API (run function).
    └── flexray_anycpu.c  Stub FlexRay Interface with generalised API.
```

## Use Cases
* Bridge - Connect NCodec nodes to a FlexRay bus (real or virtual).
  - Question: derive timing from frames (header), or explicitly from an API?
* WUP - WUP from pin or bus.
* Startup - Coldstart sequence.
* TxRx - Example of Tx/Rx between NCodec and FlexRay Interrace.

## Stubs
* Board - pins, power.
* ECU - run() and other functions.
* FlexRay AnyCPU - generalised FlexRay Interface.
