<!--
Copyright 2026 Robert Bosch GmbH

SPDX-License-Identifier: Apache-2.0
-->

# AB Codec DLL Example

This example demonstrates loading a custom AB Codec DLL via the ncodec_load()
API method.

```text
dse/ncodec                  NCodec API source code.
└── examples/ab-codec-dll   AB Codec DLL examples.
    ├── codec.c             Minimal custom AB Codec DLL.
    └── main.c              Example simulation, calls ncodec_load().
```


## Running the Examples

```bash
# Get the code.
git clone https://github.com/boschglobal/dse.ncodec.git
cd dse.ncodec

# Build the examples.
make examples
```


### AB Codec with custom DLL

```bash
# Run the PDU Co-Simulation example.
cd dse/build/_out/examples/ab-codec-dll
ab-codec-dll-example lib/ab-codec-dll.so
```

Example output:

```text
$ ./ab-codec-dll-example lib/ab-codec-dll.so
Custom DLL: read
Custom DLL: write
Custom DLL: read
Message is: Hello World
Custom DLL: read
Custom DLL: write
Custom DLL: read
Message is: Hello World
Custom DLL: read
Custom DLL: write
Custom DLL: read
Message is: Hello World
Custom DLL: read
Custom DLL: write
Custom DLL: read
Message is: Hello World
Custom DLL: read
Custom DLL: write
```
