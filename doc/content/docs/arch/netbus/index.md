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


### Data Class Diagram

```mermaid
---
config:
  class:
    hierarchicalNamespaces: false
---
classDiagram
    class NetbusTerminalVTable {
        <<struct>>
        +read(NetbusTerminal* t, uint8_t** data, size_t* len) int*
        +write(NetbusTerminal* t, const uint8_t* data, size_t len) int*
        +append(NetbusTerminal* t, const uint8_t* data, size_t len) int*
        -item_size(void) size_t*
    }

    class NetbusDesc {
        <<struct>>
        -SignalVector simbus
        +NetbusTerminalVTable vtable
        +vector~NetbusNetwork~ networks
    }

    class NetbusNetwork {
        <<struct>>
        -uint8_t** data
        -size_t* len
        -size_t* alloc_len
        +vector~NetbusTerminal~ terminals
    }

    class NetbusTerminal {
        <<struct>>
        -const char* mimetype
        -NCODEC* codec
    }

    class NetbusFmuTerminal {
        <<struct>>
        -NetbusTerminal t
        +int32_t rx_vref
        +int32_t tx_vref
    }



    NetbusDesc *-- NetbusNetwork : networks vector
    NetbusNetwork *-- NetbusTerminal : terminals vector
    NetbusFmuTerminal --> NetbusTerminalVTable
    NetbusDesc *-- NetbusTerminalVTable
    NetbusTerminal <|-- NetbusFmuTerminal
```
