<!--
Copyright 2026 Robert Bosch GmbH

SPDX-License-Identifier: Apache-2.0
-->

# AB Codec Example

These examples demonstrate AB Codec operation and integration.

```text
dse/ncodec                  NCodec API source code.
└── examples/ab-codec       AB Codec examples.
    ├── ncodec.c            Minimal NCodec implementation with trace.
    └── cosim.c             Example CoSim step() function.
```


## Running the Examples

```bash
# Get the code.
$ git clone https://github.com/boschglobal/dse.ncodec.git
$ cd dse.ncodec

# Build the examples.
$ make examples
...


# Run the CoSim step() example.
$ dse/ncodec/build/_out/examples/ab-codec/ab-cosim
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


# Run an example and write a trace file.
$ export NCODEC_TRACE_FILE=ncodec-trace.bin
$ dse/ncodec/build/_out/examples/ab-codec/ab-cosim
...
$ ls *.bin
ncodec-trace.bin
```
