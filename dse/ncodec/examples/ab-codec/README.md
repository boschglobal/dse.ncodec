<!--
Copyright 2026 Robert Bosch GmbH

SPDX-License-Identifier: Apache-2.0
-->

# AB Codec Example

These examples demonstrate AB Codec operation and integration.

```text
dse/ncodec                  NCodec API source code.
└── examples/ab-codec       AB Codec examples.
    ├── pdu_cosim.c         Minimal PDU NCodec Co-Simulation with trace.
    ├── fray_cosim.c        Minimal FlexRay NCodec Co-Simulation with trace.
    ├── fray_config.c       Static configuration tables for FlexRay example.
    ├── ncodec.c            NCodec supporting implementation.
    └── simbus.c            SimBus algorithm for FlexRay example.
```


## Running the Examples

```bash
# Get the code.
git clone https://github.com/boschglobal/dse.ncodec.git
cd dse.ncodec

# Build the examples.
make examples
```


### AB Codec with PDU Network

```bash
# Run the PDU Co-Simulation example.
dse/ncodec/build/_out/examples/ab-codec/ab-pdu-cosim
```

Example output:

```text
TRACE TX: 42 (length=11)
  payload (hex): 48 65 6C 6C 6F 20 57 6F 72 6C 64
  payload (str): "Hello World"
TRACE RX: 42 (length=11)
  payload (hex): 48 65 6C 6C 6F 20 57 6F 72 6C 64
  payload (str): "Hello World"
Message is: Hello World
TRACE TX: 42 (length=11)
  payload (hex): 48 65 6C 6C 6F 20 57 6F 72 6C 64
  payload (str): "Hello World"
```

To write a trace file:

```bash
export NCODEC_TRACE_PATH=.
dse/ncodec/build/_out/examples/ab-codec/ab-pdu-cosim

ls *.bin
ncodec.example.bin
```

### AB Codec with FlexRay Network

```bash
# Run the FlexRay CoSim example.
dse/ncodec/build/_out/examples/ab-codec/ab-fray-cosim
```

Example output:

```text
ECU 1 (@0.0005) -> Message is: Hello World from ECU 2
ECU 2 (@0.0005) -> Message is: Hello World from ECU 1
ECU 3 (@0.0005) -> Message is: Hello World from ECU 1
ECU 3 (@0.0005) -> Message is: Hello World from ECU 2
ECU 1 (@0.0055) -> Message is: Hello World from ECU 2
ECU 1 (@0.0055) -> Message is: Hello World from ECU 3
ECU 2 (@0.0055) -> Message is: Hello World from ECU 1
ECU 2 (@0.0055) -> Message is: Hello World from ECU 3
ECU 3 (@0.0055) -> Message is: Hello World from ECU 1
ECU 3 (@0.0055) -> Message is: Hello World from ECU 2
```

To write a trace file:

```bash
# Run an example and write a trace file for Node 2-0-0 (<ecu>-<cc>-<swc>).
export NCODEC_TRACE_PATH_2_0_0=.
dse/ncodec/build/_out/examples/ab-codec/ab-fray-cosim

ls *.bin
ncodec.fray.bin
```