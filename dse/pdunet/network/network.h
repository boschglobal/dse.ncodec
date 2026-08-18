// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_PDUNET_NETWORK_NETWORK_H_
#define DSE_PDUNET_NETWORK_NETWORK_H_

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <lua.h>
#include <lauxlib.h>
#include <dse/platform.h>
#include <dse/clib/collections/vector.h>
#define XXH_INLINE_ALL
#include <dse/clib/data/xxhash.h>
#include <dse/clib/util/yaml.h>
#include <dse/ncodec/interface/pdu.h>
#include <dse/pdunet/internal.h>
#include <dse/pdunet/pdunet.h>


#if defined(__GNUC__) || defined(__clang__)
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif


/*
Schema Parsing Data Types
=========================
*/

#define NONE PdunetSchemaFieldTypeNONE
#define S    PdunetSchemaFieldTypeS
#define D    PdunetSchemaFieldTypeD
#define B    PdunetSchemaFieldTypeB
#define U8   PdunetSchemaFieldTypeU8
#define U16  PdunetSchemaFieldTypeU16
#define U32  PdunetSchemaFieldTypeU32


typedef struct PdunetSchemaLabel {
    const char* name;
    const char* value;
} PdunetSchemaLabel;

typedef struct PdunetSchemaObject {
    const char* kind;
    const char* name;
    void*       doc;
    /* Data passed from schema_object_search. */
    void*       data;
} PdunetSchemaObject;

typedef struct PdunetSchemaObjectSelector {
    const char*        kind;
    const char*        name;
    PdunetSchemaLabel* labels;
    int                labels_len;
    /* Data passed to match handler. */
    void*              data;
} PdunetSchemaObjectSelector;

typedef enum PdunetSchemaFieldType {
    PdunetSchemaFieldTypeNONE = 0,
    PdunetSchemaFieldTypeU8,
    PdunetSchemaFieldTypeU16,
    PdunetSchemaFieldTypeU32,
    PdunetSchemaFieldTypeD, /* double */
    PdunetSchemaFieldTypeB, /* bool */
    PdunetSchemaFieldTypeS  /* string */
} PdunetSchemaFieldType;

typedef struct PdunetSchemaFieldMapSpec {
    const char* key;
    uint8_t     val;
} PdunetSchemaFieldMapSpec;

typedef struct PdunetSchemaFieldSpec {
    PdunetSchemaFieldType           type;
    const char*                     path;
    size_t                          offset;
    const PdunetSchemaFieldMapSpec* map;
} PdunetSchemaFieldSpec;

typedef void* (*PdunetSchemaObjectGenerator)(void* data);


/*
PDU Network Internal API
========================
*/
static inline uint32_t pdunet_checksum(
    const uint8_t* restrict payload, size_t len)
{
    if (payload == NULL) return 0;
    return XXH32((void*)payload, len, 0);
}

/* network.c */
DLL_PRIVATE void pdunet_schedule(PduNetwork* net);
DLL_PRIVATE int  pdunet_parse(PduNetwork* net, void* doc);
DLL_PRIVATE void pdunet_build_msm(PduNetwork* net, const char* name,
    size_t count, const char** signal, double* scalar);
DLL_PRIVATE int  pdunet_configure(PduNetwork* net);
DLL_PRIVATE int  pdunet_transform(PduNetwork* net, PduNetworkSortFunc sort);
DLL_PRIVATE PduNetworkNCodecVTable pdunet_network_factory(PduNetwork* net);
DLL_PRIVATE void pdunet_parse_pdus(PduNetwork* net, PdunetSchemaObject* object);
DLL_PRIVATE PduItem       pdunet_pdu_generator(PduNetwork* net, YamlNode* n);
DLL_PRIVATE PduSignalItem pdunet_signal_generator(
    PduNetwork* net, YamlNode* n, PduItem* pdu);
DLL_PRIVATE void pdunet_visit_setup_containers(
    PduNetwork* net, PduObject* pdu, void* data);
DLL_PRIVATE void pdunet_visit_container_mapto(
    PduNetwork* net, PduObject* pdu, void* data);
DLL_PRIVATE void pdunet_visit_container_mapfrom(
    PduNetwork* net, PduObject* pdu, void* data);
DLL_PRIVATE void pdunet_set_all_tx_signals_active(PduNetwork* net);


/* schema.c */
DLL_PRIVATE void* pdunet_schema_object_enumerator(PdunetSchemaObject* object,
    const char* path, uint32_t* index, PdunetSchemaObjectGenerator generator);
DLL_PRIVATE void  pdunet_schema_load_object(PduNetwork* net, void* node,
     void* object, const PdunetSchemaFieldSpec* spec, size_t count);

/* matrix.c */
DLL_PRIVATE int pdunet_matrix_transform(
    PduNetwork* net, PduNetworkSortFunc sort);
DLL_PRIVATE void pdunet_matrix_clear(PduNetwork* net);
DLL_PRIVATE void pdunet_pdu_calculate_linear_tx_active(PduNetwork* net);
DLL_PRIVATE void pdunet_pdu_calculate_linear_range(
    PduNetwork* net, PduRange* r);
DLL_PRIVATE void pdunet_pdu_pack_range(PduNetwork* net, PduRange* r);

DLL_PRIVATE void pdunet_encode_linear(PduNetwork* net, PduRange* range);
DLL_PRIVATE void pdunet_decode_linear(PduNetwork* net, PduRange* range);
DLL_PRIVATE void pdunet_encode_pack(PduNetwork* net, PduRange* range);
DLL_PRIVATE void pdunet_decode_unpack(PduNetwork* net, PduRange* range);

/* lua.c */
DLL_PRIVATE void pdunet_parse_network_functions(PduNetwork* net);
DLL_PRIVATE void pdunet_load_lua_func(
    YamlNode* n, const char* path, const char** out);
DLL_PRIVATE int pdunet_lua_install_script(
    PduNetwork* net, lua_State* L, const char* lua_script);
DLL_PRIVATE int  pdunet_lua_setup(PduNetwork* net);
DLL_PRIVATE int  pdunet_lua_pdu_call(PduNetwork* net, lua_State* L,
     int32_t func_ref, uint8_t* payload, uint32_t payload_len, bool no_err_log);
DLL_PRIVATE int  pdunet_lua_signal_call(PduNetwork* net, lua_State* L,
     int32_t func_ref, double* phys, uint64_t* raw, uint8_t* payload,
     uint32_t payload_len);
DLL_PRIVATE void pdunet_lua_teardown(PduNetwork* net);

/* flexray.c */
DLL_PRIVATE void pdunet_flexray_parse_network(PduNetwork* net);
DLL_PRIVATE void pdunet_flexray_parse_pdu(
    PduNetwork* net, PduItem* pdu, void* n);
DLL_PRIVATE size_t pdunet_flexray_config(PduNetwork* net);
DLL_PRIVATE size_t pdunet_flexray_lpdu_tx(PduNetwork* net);
DLL_PRIVATE size_t pdunet_flexray_lpdu_rx(PduNetwork* net);
DLL_PRIVATE void   pdunet_flexray_parse_network_metadata(
      PduNetwork* net, YamlNode* md);
DLL_PRIVATE void pdunet_flexray_parse_pdu_metadata(
    PduNetwork* net, PduItem* pdu, YamlNode* md);

/* can.c */
DLL_PRIVATE void   pdunet_can_parse_network(PduNetwork* net);
DLL_PRIVATE void   pdunet_can_parse_pdu(PduNetwork* net, PduItem* pdu, void* n);
DLL_PRIVATE size_t pdunet_can_config(PduNetwork* net);
DLL_PRIVATE size_t pdunet_can_lpdu_tx(PduNetwork* net);
DLL_PRIVATE size_t pdunet_can_lpdu_rx(PduNetwork* net);
DLL_PRIVATE void   pdunet_can_parse_pdu_metadata(
      PduNetwork* net, PduItem* pdu, YamlNode* md);

#endif  // DSE_PDUNET_NETWORK_NETWORK_H_
