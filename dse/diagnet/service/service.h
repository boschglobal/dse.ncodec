// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_DIAGNET_SERVICE_SERVICE_H_
#define DSE_DIAGNET_SERVICE_SERVICE_H_

#include <stddef.h>
#include <stdint.h>
#include <ffi.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/interface/diag/srpc.h>
#include "flatcc/flatcc_builder.h"


typedef struct ServiceManager {
    /* Identifying properties of the service. */
    const char* ecu_name;
    uint32_t    ecu_id;
    uint32_t    swc_id;

    /* Service registration and data. */
    Vector groups;   /* vector<ServiceGroup> */
    Vector requests; /* vector<NCodecDiagSrpcMessageContext> */
} ServiceManager;


typedef struct ServiceGroup {
    const char* name;
    void*       nc;
    Vector      funcs; /* vector<ServiceFunction> */
} ServiceGroup;


typedef struct ServiceFunction {
    const char* name;

    /* FFI registration fields. */
    void (*fn_ptr)(void);
    ffi_type*  return_type;
    int        arg_count;
    ffi_type** arg_types;

    /* FFI compiled call interface. */
    ffi_cif cif;
} ServiceFunction;


/* Service Manager API. */
ServiceManager service_manager_create(
    const char* ecu_name, uint32_t ecu_id, uint32_t swc_id);

/* Service API. */
int  service_register(ServiceManager* sm, void* nc, const char* name,
     ServiceFunction registry[], size_t count);
void service_process_requests(ServiceManager* sm);
int  service_dispatch(
     ServiceManager* sm, NCodecDiagSrpcInvokeCall* call, ffi_sarg* result);
void service_destroy(ServiceManager* sm);

/* Client API. */
int  service_request(ServiceManager* sm, const char* ecu_name, uint32_t ecu_id,
     uint32_t swc_id, const char* service, const char* function,
     NCodecDiagSrpcMessageContext* ctx);
int  service_response(ServiceManager* sm, NCodecDiagSrpcMessageContext** ctx);
void service_reset(ServiceManager* sm);
void service_ctx_cancel(ServiceManager* sm, uint8_t context_id);
void service_ctx_delete(NCodecDiagSrpcMessageContext* ctx);


typedef union ArgumentValue {
    int8_t         i8;
    uint8_t        u8;
    int16_t        i16;
    uint16_t       u16;
    int32_t        i32;
    uint32_t       u32;
    int64_t        i64;
    uint64_t       u64;
    float          f32;
    double         f64;
    const char*    str;
    const uint8_t* binary;
} ArgumentValue;

typedef struct CallArguments {
    Vector values; /* vector<ArgumentValue> */
    Vector args;   /* vector<void*> */
} CallArguments;

typedef struct CallArgumentsBuilder {
    flatcc_builder_t builder;
    void*            buffer;
    int              status;
} CallArgumentsBuilder;

/* Call builder API. */
int  call_arguments_builder_init(CallArgumentsBuilder* builder);
void call_arguments_builder_destroy(CallArgumentsBuilder* builder);
int  call_arguments_add_i8(CallArgumentsBuilder* builder, int8_t value);
int  call_arguments_add_u8(CallArgumentsBuilder* builder, uint8_t value);
int  call_arguments_add_i16(CallArgumentsBuilder* builder, int16_t value);
int  call_arguments_add_u16(CallArgumentsBuilder* builder, uint16_t value);
int  call_arguments_add_i32(CallArgumentsBuilder* builder, int32_t value);
int  call_arguments_add_u32(CallArgumentsBuilder* builder, uint32_t value);
int  call_arguments_add_i64(CallArgumentsBuilder* builder, int64_t value);
int  call_arguments_add_u64(CallArgumentsBuilder* builder, uint64_t value);
int  call_arguments_add_f32(CallArgumentsBuilder* builder, float value);
int  call_arguments_add_f64(CallArgumentsBuilder* builder, double value);
int call_arguments_add_string(CallArgumentsBuilder* builder, const char* value);
int call_arguments_add_binary(
    CallArgumentsBuilder* builder, const uint8_t* value, size_t length);
int call_arguments_finalize(
    CallArgumentsBuilder* builder, NCodecDiagSrpcMessageContext* ctx);

/* Call arguments API. */
void call_arguments_destroy(CallArguments* arguments);
int  call_arguments_decode(const NCodecDiagSrpcInvokeCall* call,
     const ServiceFunction* func, CallArguments* out);

#endif  // DSE_DIAGNET_SERVICE_SERVICE_H_
