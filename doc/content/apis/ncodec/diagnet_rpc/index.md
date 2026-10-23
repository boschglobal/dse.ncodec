---
title: Diagnostic Network Service RPC API Reference
linkTitle: DIAGNet RPC
---
## Diagnostic Network Service RPC


The Diagnostic Network Service RPC API provides service registration, request
dispatch, and client request/response handling over a Network Codec.

A Service Manager identifies a local ECU and software component. Service
groups associate a Network Codec with a group of named functions which represent
that service. Clients use the same registration to send requests and collect
responses from those remote services.

Service functions are invoked through libffi by default. A service group can
provide a custom dispatch implementation through `service_set_dispatch`.



## service_manager_create


Create a Service Manager with empty service and client request registries.

### Parameters

ecu_name (const char*)
: Local ECU name. The caller owns the string and must keep it valid for the
    lifetime of the Service Manager.

ecu_id (uint32_t)
: Local ECU identifier, used when a request does not specify an ECU name.

swc_id (uint32_t)
: Local software component identifier.

### Returns

ServiceManager
: The initialized Service Manager. Release its resources with `service_destroy`.



## Typedefs

### ArgumentValue

```c
typedef struct ArgumentValue {
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    int64_t i64;
    uint64_t u64;
    float f32;
    double f64;
    const char* str;
    const uint8_t* binary;
}
```

### CallArguments

```c
typedef struct CallArguments {
    int values;
    int args;
}
```

### CallArgumentsBuilder

```c
typedef struct CallArgumentsBuilder {
    int builder;
    void* buffer;
    int status;
}
```

### ServiceFunction

```c
typedef struct ServiceFunction {
    const char* name;
    void (*)(void) fn_ptr;
    int* return_type;
    int arg_count;
    int** arg_types;
    int cif;
}
```

### ServiceGroup

```c
typedef struct ServiceGroup {
    const char* name;
    void* nc;
    int funcs;
    struct {
        ServiceDispatch dispatch;
    } vtable;
}
```

### ServiceManager

```c
typedef struct ServiceManager {
    const char* ecu_name;
    uint32_t ecu_id;
    uint32_t swc_id;
    int groups;
    int requests;
}
```

## Functions

### call_arguments_add_binary

Append copied binary bytes. Encode any length required by the called function
as a separate argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (const uint8_t*)
: Bytes to copy. May be NULL when length is zero.

length (size_t)
: Number of bytes to copy.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for invalid arguments, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_f32

Append a single-precision floating-point argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (float)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_f64

Append a double-precision floating-point argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (double)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_i16

Append a signed 16-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (int16_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_i32

Append a signed 32-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (int32_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_i64

Append a signed 64-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (int64_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_i8

Append a signed 8-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (int8_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_string

Append a string argument. The string is copied into the builder.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (const char*)
: Non-NULL, null-terminated string.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder or NULL string, or the builder's stored error
    (-ENOMEM on allocation failure).



### call_arguments_add_u16

Append an unsigned 16-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (uint16_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_u32

Append an unsigned 32-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (uint32_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_u64

Append an unsigned 64-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (uint64_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_add_u8

Append an unsigned 8-bit argument.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

value (uint8_t)
: Argument value.

#### Returns

0
: The argument was appended.

<0
: -EINVAL for an invalid builder, or the builder's stored error (-ENOMEM on
    allocation failure).



### call_arguments_builder_destroy

Release builder resources, including the finalized request buffer.

#### Parameters

builder (CallArgumentsBuilder*)
: Initialized builder to release and clear. May be NULL.



### call_arguments_builder_init

Initialize an argument builder. Destroy it after use, including after failure.

#### Parameters

builder (CallArgumentsBuilder*)
: Fresh builder object to initialize.

#### Returns

0
: The builder is ready to accept arguments.

-EINVAL
: The builder is NULL.

-ENOMEM
: Builder allocation failed.



### call_arguments_decode

Decode request arguments and check their count and types against a registered
function. String and binary values borrow the request buffer.

#### Parameters

call (const NCodecDiagSrpcInvokeCall*)
: Request whose argument buffer must remain valid while decoded values are used.

func (const ServiceFunction*)
: Registered function defining the expected argument types.

out (CallArguments*)
: (out) Fresh, empty argument storage. Release with `call_arguments_destroy`.
    Left unchanged on failure.

#### Returns

0
: Arguments were decoded.

-EINVAL
: Invalid parameters or argument count/type mismatch.

-EBADMSG
: Missing argument bytes or unsupported argument representation.

-ENOMEM
: Argument allocation failed.

-EOVERFLOW
: Argument storage size exceeds the allocation limit.



### call_arguments_destroy

Release decoded argument storage. Borrowed string and binary bytes are not
freed.

#### Parameters

arguments (CallArguments*)
: Decoded arguments to release. May be NULL.



### call_arguments_finalize

Finish encoding and set the context's request buffer and length. The buffer
remains builder owned and is valid until `call_arguments_builder_destroy`.
No more arguments may be added after success.

#### Parameters

builder (CallArgumentsBuilder*)
: Active argument builder.

ctx (NCodecDiagSrpcMessageContext*)
: (out) Context receiving the request buffer. Any previous buffer is not freed.

#### Returns

0
: The encoded request was assigned to the context.

<0
: -EINVAL for invalid arguments, or the builder's stored error (-ENOMEM on
    allocation failure).



### service_ctx_cancel

Remove a pending client request from response tracking and release its copied
context and buffers. No cancellation message is sent to the remote service.

#### Parameters

sm (ServiceManager*)
: Service Manager object. If NULL, no request is cancelled.

context_id (uint32_t)
: Identifier of the pending request. If no request matches, no action is taken.



### service_ctx_delete

Release a client response context returned by `service_response`. The context
and its copied buffers occupy a single allocation and are released together.
Do not pass a caller-owned request context or free its copied buffers
separately.

#### Parameters

ctx (NCodecDiagSrpcMessageContext*)
: Client response context to release. If NULL, no memory is released.



### service_destroy

Release all registered service functions and pending client requests. Pending
request contexts and their owned buffers are deleted. Network Codecs and
referenced name strings are not released.

#### Parameters

sm (ServiceManager*)
: Service Manager object. If NULL, no resources are released.



### service_dispatch

Invoke a registered service function through its prepared libffi interface.
Request arguments are decoded before invocation and released afterwards.

#### Parameters

sm (ServiceManager*)
: Service Manager object.

call (const NCodecDiagSrpcInvokeCall*)
: Request specifying the service, function, and encoded arguments.

ret (NCodecDiagSrpcInvokeReturn**)
: (out) Heap allocated return object when a response is required. Set to NULL
    when no response is generated. The caller owns the returned object and its
    response buffer. May be NULL to suppress response allocation.

#### Returns

0
: The function was invoked successfully.

-EINVAL
: A required argument, service name, function name, or encoded argument is
    invalid.

-ENOENT
: The service group or function is not registered.

-ENOMEM
: Argument decoding or response allocation failed.

-EBADMSG
: The request argument buffer is missing or contains an unsupported argument
    representation.

-EOVERFLOW
: The decoded argument count exceeds the allocation size limit.



### service_process_requests

Process available requests from each service group's Network Codec. Requests
are selected by the local ECU name or identifier and software component
identifier, then passed to the group's dispatch implementation.

Responses are encoded after draining and truncating the input stream, then
flushed to the connected stream. Dispatch return objects and their owned
request/response buffers are released after processing. Dispatch and write
failures are logged.

#### Parameters

sm (ServiceManager*)
: Service Manager object. If NULL, no requests are processed.



### service_register

Register a service group and prepare its functions for invocation through
libffi. The default dispatch implementation is `service_dispatch`.

Function definitions and argument type arrays are copied. Service and function
names, function pointers, and FFI type objects remain caller owned and must
remain valid while the service group is registered. Functions registered
successfully remain available even if another function fails registration.

#### Parameters

sm (ServiceManager*)
: Service Manager object.

nc (void*)
: Network Codec used to receive service requests and send responses.

name (const char*)
: Name of the service group.

registry (ServiceFunction[])
: Function definitions to register. Must not be NULL, including when count is
    zero.

count (size_t)
: Number of function definitions in the registry.

#### Returns

0
: The service group and its functions were registered.

-EINVAL
: A required argument is NULL or a function's FFI call interface is invalid.

-EEXIST
: A service group with the specified name is already registered.

-ENOMEM
: The service group could not be located after insertion or an argument type
    array could not be allocated.



### service_request

Write and flush a service request through a registered service group's Network
Codec. Requests requiring a response are tracked by context identifier. The
request context and its buffers remain caller owned; a deep copy is retained
for response tracking.

#### Parameters

sm (ServiceManager*)
: Service Manager object.

ecu_name (const char*)
: Target ECU name. Must not be NULL.

ecu_id (uint32_t)
: Target ECU identifier.

swc_id (uint32_t)
: Target software component identifier.

service (const char*)
: Name of the registered service group and remote service.

function (const char*)
: Name of the remote function to invoke.

ctx (NCodecDiagSrpcMessageContext*)
: Request context containing encoded arguments, the context identifier, and
    response requirements.

#### Returns

0
: The request was written and flushed, and response tracking was established
    when required.

-EINVAL
: A required pointer argument is NULL.

-ENOENT
: The service group is not registered or has no Network Codec.

-EEXIST
: A request with the same context identifier is already awaiting a response.

-ENOMEM
: The tracking context could not be copied or added to the request registry.
    Failure to add the context can occur after the request has been sent.

<int>
: A negative error returned by `ncodec_write` or `ncodec_flush`.



### service_reset

Truncate the Network Codec stream for every registered service group. Pending
client request contexts remain registered. Codec truncation errors are ignored.

#### Parameters

sm (ServiceManager*)
: Service Manager object. If NULL, no streams are truncated.



### service_response

Read a response matching a pending request and the Service Manager's ECU and
software component identifiers. The matching request is removed from response
tracking. Messages that are not matching responses are consumed and ignored.

#### Parameters

sm (ServiceManager*)
: Service Manager object.

ctx (NCodecDiagSrpcMessageContext**)
: (out) Heap allocated copy of the response context and its request/response
    buffers. The caller owns this object and must release it with
    `service_ctx_delete`. Set to NULL when no response is returned, provided
    both arguments are valid.

return_value (uint32_t*)
: Simple cast of the return value, if any.

error_code (int32_t*)
: Error code related to the request.

#### Returns

0
: A matching response context was returned.

-EINVAL
: The Service Manager or output pointer is NULL.

-ENOMEM
: The response context could not be copied. The pending request remains
    registered, but the response message has been consumed.

-ENOMSG
: No matching response was available from the registered Network Codecs.



### service_set_dispatch

Set the dispatch implementation for a registered service group.

#### Parameters

sm (ServiceManager*)
: Service Manager object.

name (const char*)
: Name of the service group.

func (ServiceDispatch)
: Dispatch implementation. Set to NULL to disable request processing for the
    service group.

#### Returns

0
: The dispatch implementation was set.

-EINVAL
: The Service Manager or service group name is NULL.

-ENOENT
: The service group is not registered.



