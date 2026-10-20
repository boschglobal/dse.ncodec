// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <ffi.h>
#include <dse/clib/collections/vector.h>
#include <dse/diagnet/service/flatbuffers/args_builder.h>
#include <dse/diagnet/service/flatbuffers/args_reader.h>
#include <dse/diagnet/service/service.h>


#define sa(name) ServiceArgs_##name


enum {
    BUILDER_ACTIVE = 1,
    BUILDER_FINALIZED = 2,
};

static int _builder_ready(const CallArgumentsBuilder* builder)
{
    if (builder == NULL) return -EINVAL;
    if (builder->status < 0) return builder->status;
    return builder->status == BUILDER_ACTIVE ? 0 : -EINVAL;
}

static int _append_argument(
    CallArgumentsBuilder* builder, sa(Value_union_ref_t) value)
{
    flatcc_builder_t* B = &builder->builder;
    sa(Argument_ref_t) argument;

    if (value.value == 0 || (argument = sa(Argument_create(B, value))) == 0 ||
        sa(Argument_vec_push(B, argument)) == NULL) {
        builder->status = -ENOMEM;
        return builder->status;
    }
    return 0;
}


int call_arguments_builder_init(CallArgumentsBuilder* builder)
{
    if (builder == NULL) return -EINVAL;
    *builder = (CallArgumentsBuilder){ 0 };
    if (flatcc_builder_init(&builder->builder) != 0) {
        builder->status = -ENOMEM;
        return builder->status;
    }
    builder->status = BUILDER_ACTIVE;
    if (sa(Arguments_start_as_root(&builder->builder)) != 0 ||
        sa(Argument_vec_start(&builder->builder)) != 0) {
        builder->status = -ENOMEM;
        return builder->status;
    }
    return 0;
}


void call_arguments_builder_destroy(CallArgumentsBuilder* builder)
{
    if (builder == NULL) return;
    free(builder->buffer);
    flatcc_builder_clear(&builder->builder);
    *builder = (CallArgumentsBuilder){ 0 };
}

#define DEFINE_ADD(name, type, table)                                          \
    int call_arguments_add_##name(CallArgumentsBuilder* builder, type value)   \
    {                                                                          \
        int rc = _builder_ready(builder);                                      \
        if (rc != 0) return rc;                                                \
        return _append_argument(                                               \
            builder, sa(Value_as_##table)(                                     \
                         sa(table##_create)(&builder->builder, value)));       \
    }

DEFINE_ADD(i8, int8_t, Int8Value)
DEFINE_ADD(u8, uint8_t, UInt8Value)
DEFINE_ADD(i16, int16_t, Int16Value)
DEFINE_ADD(u16, uint16_t, UInt16Value)
DEFINE_ADD(i32, int32_t, Int32Value)
DEFINE_ADD(u32, uint32_t, UInt32Value)
DEFINE_ADD(i64, int64_t, Int64Value)
DEFINE_ADD(u64, uint64_t, UInt64Value)
DEFINE_ADD(f32, float, Float32Value)
DEFINE_ADD(f64, double, Float64Value)

#undef DEFINE_ADD


int call_arguments_add_string(CallArgumentsBuilder* builder, const char* value)
{
    int rc = _builder_ready(builder);
    if (rc != 0) return rc;
    if (value == NULL) return builder->status = -EINVAL;

    flatbuffers_string_ref_t ref =
        flatbuffers_string_create_str(&builder->builder, value);
    if (ref == 0) return builder->status = -ENOMEM;
    return _append_argument(
        builder, sa(Value_as_StringValue)(
                     sa(StringValue_create(&builder->builder, ref))));
}


int call_arguments_add_binary(
    CallArgumentsBuilder* builder, const uint8_t* value, size_t length)
{
    int rc = _builder_ready(builder);
    if (rc != 0) return rc;
    if (value == NULL && length != 0) return builder->status = -EINVAL;

    flatbuffers_uint8_vec_ref_t ref =
        flatbuffers_uint8_vec_create(&builder->builder, value, length);
    if (ref == 0) return builder->status = -ENOMEM;
    return _append_argument(
        builder, sa(Value_as_BinaryValue)(
                     sa(BinaryValue_create(&builder->builder, ref))));
}


int call_arguments_finalize(
    CallArgumentsBuilder* builder, NCodecDiagSrpcMessageContext* ctx)
{
    int rc = _builder_ready(builder);
    if (rc != 0) return rc;
    if (ctx == NULL) return builder->status = -EINVAL;

    flatcc_builder_t* B = &builder->builder;
    sa(Argument_vec_ref_t) items = sa(Argument_vec_end(B));
    if (items == 0 || sa(Arguments_items_add(B, items)) != 0 ||
        sa(Arguments_end_as_root(B)) == 0) {
        return builder->status = -ENOMEM;
    }

    size_t length = 0;
    builder->buffer = flatcc_builder_finalize_buffer(B, &length);
    if (builder->buffer == NULL) return builder->status = -ENOMEM;

    ctx->request = builder->buffer;
    ctx->request_len = length;
    builder->status = BUILDER_FINALIZED;
    return 0;
}


void call_arguments_destroy(CallArguments* arguments)
{
    if (arguments == NULL) return;
    vector_reset(&arguments->args);
    vector_reset(&arguments->values);
}


int call_arguments_decode(const NCodecDiagSrpcInvokeCall* call,
    const ServiceFunction* func, CallArguments* out)
{
    CallArguments decoded = { 0 };
    int           error = -EINVAL;

    /* out must be a fresh, empty CallArguments. */
    if (call == NULL || func == NULL || out == NULL) return -EINVAL;
    if (func->arg_count < 0 ||
        (func->arg_count != 0 && func->arg_types == NULL)) {
        return -EINVAL;
    }
    if (call->ctx.request == NULL || call->ctx.request_len == 0) {
        return -EBADMSG;
    }

    sa(Arguments_table_t) root = sa(Arguments_as_root(call->ctx.request));
    sa(Argument_vec_t) items = sa(Arguments_items(root));
    size_t count = sa(Argument_vec_len(items));

    if (count != (size_t)func->arg_count) return -EINVAL;
    if (count == 0) {
        *out = decoded;
        return 0;
    }
    if (count > SIZE_MAX / sizeof(ArgumentValue) ||
        count > SIZE_MAX / sizeof(void*)) {
        return -EOVERFLOW;
    }

    decoded.values = vector_make(sizeof(ArgumentValue), count, NULL);
    decoded.args = vector_make(sizeof(void*), count, NULL);
    if (decoded.values.items == NULL || decoded.args.items == NULL) {
        error = -ENOMEM;
        goto fail;
    }

    ArgumentValue* values = decoded.values.items;
    void**         args = decoded.args.items;

    for (size_t i = 0; i < count; i++) {
        sa(Argument_table_t) argument = sa(Argument_vec_at(items, i));
        const void* payload = sa(Argument_value(argument));
        ffi_type*   expected = func->arg_types[i];

        if (payload == NULL) {
            error = -EBADMSG;
            goto fail;
        }

        switch (sa(Argument_value_type(argument))) {
#define DECODE_SCALAR(table, member, ffi_name)                                 \
    case sa(Value_##table): {                                                  \
        if (expected != &ffi_name) goto fail;                                  \
        values[i].member = sa(table##_value)((sa(table##_table_t))payload);    \
        args[i] = &values[i].member;                                           \
        break;                                                                 \
    }

            DECODE_SCALAR(Int8Value, i8, ffi_type_sint8)
            DECODE_SCALAR(UInt8Value, u8, ffi_type_uint8)
            DECODE_SCALAR(Int16Value, i16, ffi_type_sint16)
            DECODE_SCALAR(UInt16Value, u16, ffi_type_uint16)
            DECODE_SCALAR(Int32Value, i32, ffi_type_sint32)
            DECODE_SCALAR(UInt32Value, u32, ffi_type_uint32)
            DECODE_SCALAR(Int64Value, i64, ffi_type_sint64)
            DECODE_SCALAR(UInt64Value, u64, ffi_type_uint64)
            DECODE_SCALAR(Float32Value, f32, ffi_type_float)
            DECODE_SCALAR(Float64Value, f64, ffi_type_double)

#undef DECODE_SCALAR

        case sa(Value_StringValue):
            if (expected != &ffi_type_pointer) goto fail;
            values[i].str =
                sa(StringValue_value)((sa(StringValue_table_t))payload);
            args[i] = &values[i].str;
            break;

        case sa(Value_BinaryValue):
            if (expected != &ffi_type_pointer) goto fail;
            /* Borrowed bytes; length is a separate serialized argument. */
            values[i].binary =
                sa(BinaryValue_value)((sa(BinaryValue_table_t))payload);
            args[i] = &values[i].binary;
            break;

        default:
            /* Includes NONE and unsupported union alternatives. */
            error = -EBADMSG;
            goto fail;
        }
    }

    decoded.values.length = count;
    decoded.args.length = count;
    *out = decoded;
    return 0;

fail:
    call_arguments_destroy(&decoded);
    return error;
}

#undef sa
