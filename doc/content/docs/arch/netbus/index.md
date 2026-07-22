---
title: "Network Bus"
linkTitle: "Network Bus"
weight: 1050
tags:
  - NCodec
  - Architecture
github_repo: "https://github.com/boschglobal/dse.ncodec"
github_subdir: "doc"
---

## Synopsis

A Network Bus is used to connect a network between a DSE SimBus Binary Signal, which may represent several Network Nodes, and any number of external (to the SimBus) Network Nodes. The Network Bus ensures that the operation of Network Codec objects, which represent connections to the Network, remains consistent when Bus Models are being operated (by the Network Codec).


## Design

### Sequence Diagram

```mermaid
%%{init: {
  'theme': 'default',
  'sequence': {
    'messageMargin': 2,
    'boxTextMargin': 2,
    'noteMargin': 4,
    'mirrorActors': false
  }
}}%%
sequenceDiagram
    title Network Bus
    autonumber

    %% nodes
    participant FMU as FMU
    participant Importer as Importer
    box "ModelC FMU"
        participant NetBus as NetBus<br/>(NCodec)
        participant SimBus as SimBus<br/>(Runtime)
        participant Model as Model<br/>(NCcodec)
    end

    Note over FMU: F1 (tx)
    Note over Model: M1 (tx)
    Model->>SimBus: write
    Note over SimBus: M1 (tx)
    Importer->>FMU: get (F1)
    Note over Importer: F1 (tx)
    Importer->>NetBus: set (F1)
    activate NetBus
    NetBus->>NetBus: truncate
    NetBus->>NetBus: write
    NetBus->>NetBus: flush
    Note over NetBus: F1 (tx)
    deactivate NetBus
    Importer->>NetBus: step
    loop step
        SimBus->>NetBus: pull (F1)
        Note over SimBus: M1 (tx)
        Note over SimBus: F1 (tx)
        SimBus->>NetBus: push (M1)
        Note over NetBus: M1 (tx)
        Note over NetBus: F1 (tx)
        SimBus->>+Model: step
        Note over Model: M1 (tx)
        Note over Model: F1 (tx)
        Model->>Model: read
        Note over Model: M1 (rx)
        Model->>Model: truncate
        Model->>Model: write
        Model->>Model: flush
        Note over Model: M1 (tx)
        Model-->>-SimBus: [ok]
        Note over SimBus: M1 (tx)
    end
    Importer->>+NetBus: get
    NetBus->>NetBus: read
    Note over NetBus: F1 (rx)
    NetBus-->>-Importer: get (F1)
    Note over Importer: F1 (rx)
    Importer->>FMU: set (F1)
    Note over FMU: F1 (rx)

```


### Class Diagram

```mermaid
---
config:
  class:
    hierarchicalNamespaces: false
---
classDiagram
    class NetbusTerminalVTable {
        <<struct>>
        -size_t size

        -find(net, t) *NetbusTerminal
        -read(t, data, len) int32_t
        -write(t, data, len) int32_t
        -append(t, data, len) int32_t
    }

    class NetbusDesc {
        <<struct>>
        +vector~NetbusNetwork~ networks

        +netbus_create(void) *NetBusDesc
        +netbus_step(nb) int32_t "@simbus step"
        +netbus_destroy(nb) void
    }

    class NetbusNetwork {
        <<struct>>
        %% vtable methods are network/implementation specific (i.e. FlexRay+FMU)
        -NetbusTerminalVTable vtable
        +const char* name
        -NCodecStream simbus
        %% tx from terminals, buffered before merge with simbus
        -NCodecStream buffer
        +vector~NetbusTerminal~ terminals

        +netbus_add_network(nb, network, simbus, vtable) *NetbusNetwork
        +netbus_find_network(nb, network) *NetbusNetwork
    }

    class NetbusTerminal {
        <<struct>>
        %% push/pull acts upon instance data
        -void* instance
        -NetbusNetwork* net
        -const char* mimetype
        %% codec receives write/append
        -NCODEC* codec
        %% tx_acc stream is appended by codec read, and reset on terminal read
        +NCodecStream tx_accum

        %% Provide (derived type) stack var cast to NetbusTerminal
        +netbus_connect_terminal(net, inst, terminal) *NetbusTerminal
        +netbus_find_terminal(net, terminal) *NetbusTerminal
        %% push/pull call implementation specific vtable methods
        +netbus_push(nb, t) int32_t
        +netbus_pull(nb, t) int32_t
    }

    class NetbusFmuTerminal {
        <<struct>>
        -NetbusTerminal t
        +int32_t rx_vref
        +int32_t tx_vref
    }



    NetbusDesc *-- NetbusNetwork : networks vector
    NetbusNetwork *-- NetbusTerminal : terminals vector
    %% NetbusFmuTerminal --> NetbusTerminalVTable
    NetbusNetwork *-- NetbusTerminalVTable
    NetbusTerminal <|-- NetbusFmuTerminal
    NetbusTerminal --> NetbusTerminalVTable

```


### Data Flowchart

```mermaid
---
config:
  flowchart:
    defaultRenderer: "elk"
---
flowchart TB
    S_pullpush -.-> NCodec
    S_append -.-> Accum
    %% SimBus Time Domain
    subgraph SimbusDomain ["SimBus Time Domain"]
        direction TB
        %%R_pull["netbus_pull()<br>pull rx from all terminals"]
        S_step["netbus_step()"]
        --> S_pullpush["Pull/Push SimBus<->Codec"]
        --> S_read["Codec Read"]
        --> S_append["Append to Accumulator"]
    end

    %% Central Shared Buffer State
    subgraph State
        subgraph Network ["Network"]
            Buffer["Buffer"]
        end
        subgraph Terminal ["Terminal"]
            NCodec["NCodec"]
            Accum["Accumulator"]
        end
    end

    %% Terminal Domain
    subgraph TerminalDomain ["Terminal Time Domain"]
        direction TB
        T_step["step()"]
        --> T_pull["netbus_pull() -> NCodec<br>(Rx) read Tx from node"]
        --> T_push["netbus_push() -> Tx<br>(Tx) write Rx (accum) to node"]
        --> T_clear["Reset Accumlator"]
    end

    %% Inter-subgraph connections
    Buffer -.-> S_pullpush
    NCodec -.-> S_read
    Accum -.-> T_push
    T_pull -.-> Buffer

```

