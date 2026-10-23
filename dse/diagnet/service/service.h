// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_DIAGNET_SERVICE_SERVICE_H_
#define DSE_DIAGNET_SERVICE_SERVICE_H_

#include <stddef.h>
#include <stdint.h>
#include <ffi.h>
#include <flatcc/flatcc_builder.h>
#include <dse/clib/collections/vector.h>
#include <dse/ncodec/interface/diag/srpc.h>
#include <dse/platform.h>


/**
Diagnostic Network Service RPC
===================

The Diagnostic Network Service RPC API provides service registration, request
dispatch, and client request/response handling over a Network Codec.

A Service Manager identifies a local ECU and software component. Service
groups associate a Network Codec with a group of named functions which represent
that service. Clients use the same registration to send requests and collect
responses from those remote services.

Service functions are invoked through libffi by default. A service group can
provide a custom dispatch implementation through `service_set_dispatch`.
*/


typedef struct ServiceManager {
    /* Identifying properties of the service. */
    const char* ecu_name;
    uint32_t    ecu_id;
    uint32_t    swc_id;

    /* Service registration and data. */
    Vector groups;   /* vector<ServiceGroup> */
    Vector requests; /* vector<NCodecDiagSrpcMessageContext> */
} ServiceManager;


typedef int (*ServiceDispatch)(ServiceManager* sm,
    const NCodecDiagSrpcInvokeCall* call, NCodecDiagSrpcInvokeReturn** ret);

typedef struct ServiceGroup {
    const char* name;
    void*       nc;
    Vector      funcs; /* vector<ServiceFunction> */

    /* VTable interface. */
    struct {
        ServiceDispatch dispatch;
    } vtable;
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
DLL_PUBLIC ServiceManager service_manager_create(
    const char* ecu_name, uint32_t ecu_id, uint32_t swc_id);

/* Service API. */
DLL_PUBLIC int service_register(ServiceManager* sm, void* nc, const char* name,
    ServiceFunction registry[], size_t count);
DLL_PUBLIC int service_set_dispatch(
    ServiceManager* sm, const char* name, ServiceDispatch func);
DLL_PUBLIC void service_process_requests(ServiceManager* sm);
DLL_PUBLIC int  service_dispatch(ServiceManager* sm,
     const NCodecDiagSrpcInvokeCall* call, NCodecDiagSrpcInvokeReturn** ret);
DLL_PUBLIC void service_destroy(ServiceManager* sm);


/* Client API. */
DLL_PUBLIC int  service_request(ServiceManager* sm, const char* ecu_name,
     uint32_t ecu_id, uint32_t swc_id, const char* service, const char* function,
     NCodecDiagSrpcMessageContext* ctx);
DLL_PUBLIC int  service_response(ServiceManager* sm,
     NCodecDiagSrpcMessageContext** ctx, uint32_t* return_value,
     int32_t* error_code);
DLL_PUBLIC void service_reset(ServiceManager* sm);
DLL_PUBLIC void service_ctx_cancel(ServiceManager* sm, uint32_t context_id);
DLL_PUBLIC void service_ctx_delete(NCodecDiagSrpcMessageContext* ctx);


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
DLL_PUBLIC int  call_arguments_builder_init(CallArgumentsBuilder* builder);
DLL_PUBLIC void call_arguments_builder_destroy(CallArgumentsBuilder* builder);
DLL_PUBLIC int  call_arguments_add_i8(
     CallArgumentsBuilder* builder, int8_t value);
DLL_PUBLIC int call_arguments_add_u8(
    CallArgumentsBuilder* builder, uint8_t value);
DLL_PUBLIC int call_arguments_add_i16(
    CallArgumentsBuilder* builder, int16_t value);
DLL_PUBLIC int call_arguments_add_u16(
    CallArgumentsBuilder* builder, uint16_t value);
DLL_PUBLIC int call_arguments_add_i32(
    CallArgumentsBuilder* builder, int32_t value);
DLL_PUBLIC int call_arguments_add_u32(
    CallArgumentsBuilder* builder, uint32_t value);
DLL_PUBLIC int call_arguments_add_i64(
    CallArgumentsBuilder* builder, int64_t value);
DLL_PUBLIC int call_arguments_add_u64(
    CallArgumentsBuilder* builder, uint64_t value);
DLL_PUBLIC int call_arguments_add_f32(
    CallArgumentsBuilder* builder, float value);
DLL_PUBLIC int call_arguments_add_f64(
    CallArgumentsBuilder* builder, double value);
DLL_PUBLIC int call_arguments_add_string(
    CallArgumentsBuilder* builder, const char* value);
DLL_PUBLIC int call_arguments_add_binary(
    CallArgumentsBuilder* builder, const uint8_t* value, size_t length);
DLL_PUBLIC int call_arguments_finalize(
    CallArgumentsBuilder* builder, NCodecDiagSrpcMessageContext* ctx);

/* Call arguments API. */
DLL_PUBLIC void call_arguments_destroy(CallArguments* arguments);
DLL_PUBLIC int  call_arguments_decode(const NCodecDiagSrpcInvokeCall* call,
     const ServiceFunction* func, CallArguments* out);

#endif  // DSE_DIAGNET_SERVICE_SERVICE_H_
