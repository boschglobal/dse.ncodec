#ifndef CAN_READER_H
#define CAN_READER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#ifndef FLATBUFFERS_COMMON_READER_H
#include "flatbuffers_common_reader.h"
#endif
#include "flatcc/flatcc_flatbuffers.h"
#ifndef __alignas_is_defined
#include <stdalign.h>
#endif
#include "flatcc/flatcc_prologue.h"
#undef flatbuffers_identifier
#define flatbuffers_identifier "RICA"
#undef flatbuffers_extension
#define flatbuffers_extension ".can"

typedef struct AutomotiveBus_Register_Can_TimeSpec AutomotiveBus_Register_Can_TimeSpec_t;
typedef const AutomotiveBus_Register_Can_TimeSpec_t *AutomotiveBus_Register_Can_TimeSpec_struct_t;
typedef AutomotiveBus_Register_Can_TimeSpec_t *AutomotiveBus_Register_Can_TimeSpec_mutable_struct_t;
typedef const AutomotiveBus_Register_Can_TimeSpec_t *AutomotiveBus_Register_Can_TimeSpec_vec_t;
typedef AutomotiveBus_Register_Can_TimeSpec_t *AutomotiveBus_Register_Can_TimeSpec_mutable_vec_t;
typedef struct AutomotiveBus_Register_Can_MessageTiming AutomotiveBus_Register_Can_MessageTiming_t;
typedef const AutomotiveBus_Register_Can_MessageTiming_t *AutomotiveBus_Register_Can_MessageTiming_struct_t;
typedef AutomotiveBus_Register_Can_MessageTiming_t *AutomotiveBus_Register_Can_MessageTiming_mutable_struct_t;
typedef const AutomotiveBus_Register_Can_MessageTiming_t *AutomotiveBus_Register_Can_MessageTiming_vec_t;
typedef AutomotiveBus_Register_Can_MessageTiming_t *AutomotiveBus_Register_Can_MessageTiming_mutable_vec_t;

typedef const struct AutomotiveBus_Register_Can_CanStatus_table *AutomotiveBus_Register_Can_CanStatus_table_t;
typedef struct AutomotiveBus_Register_Can_CanStatus_table *AutomotiveBus_Register_Can_CanStatus_mutable_table_t;
typedef const flatbuffers_uoffset_t *AutomotiveBus_Register_Can_CanStatus_vec_t;
typedef flatbuffers_uoffset_t *AutomotiveBus_Register_Can_CanStatus_mutable_vec_t;
typedef const struct AutomotiveBus_Register_Can_Frame_table *AutomotiveBus_Register_Can_Frame_table_t;
typedef struct AutomotiveBus_Register_Can_Frame_table *AutomotiveBus_Register_Can_Frame_mutable_table_t;
typedef const flatbuffers_uoffset_t *AutomotiveBus_Register_Can_Frame_vec_t;
typedef flatbuffers_uoffset_t *AutomotiveBus_Register_Can_Frame_mutable_vec_t;
typedef const struct AutomotiveBus_Register_Can_MetaFrame_table *AutomotiveBus_Register_Can_MetaFrame_table_t;
typedef struct AutomotiveBus_Register_Can_MetaFrame_table *AutomotiveBus_Register_Can_MetaFrame_mutable_table_t;
typedef const flatbuffers_uoffset_t *AutomotiveBus_Register_Can_MetaFrame_vec_t;
typedef flatbuffers_uoffset_t *AutomotiveBus_Register_Can_MetaFrame_mutable_vec_t;
typedef const struct AutomotiveBus_Register_Can_RegisterFile_table *AutomotiveBus_Register_Can_RegisterFile_table_t;
typedef struct AutomotiveBus_Register_Can_RegisterFile_table *AutomotiveBus_Register_Can_RegisterFile_mutable_table_t;
typedef const flatbuffers_uoffset_t *AutomotiveBus_Register_Can_RegisterFile_vec_t;
typedef flatbuffers_uoffset_t *AutomotiveBus_Register_Can_RegisterFile_mutable_vec_t;
#ifndef AutomotiveBus_Register_Can_CanStatus_identifier
#define AutomotiveBus_Register_Can_CanStatus_identifier flatbuffers_identifier
#endif
#define AutomotiveBus_Register_Can_CanStatus_type_hash ((flatbuffers_thash_t)0x2c09d5bb)
#define AutomotiveBus_Register_Can_CanStatus_type_identifier "\xbb\xd5\x09\x2c"
#ifndef AutomotiveBus_Register_Can_TimeSpec_identifier
#define AutomotiveBus_Register_Can_TimeSpec_identifier flatbuffers_identifier
#endif
#define AutomotiveBus_Register_Can_TimeSpec_type_hash ((flatbuffers_thash_t)0xe3302bbb)
#define AutomotiveBus_Register_Can_TimeSpec_type_identifier "\xbb\x2b\x30\xe3"
#ifndef AutomotiveBus_Register_Can_MessageTiming_identifier
#define AutomotiveBus_Register_Can_MessageTiming_identifier flatbuffers_identifier
#endif
#define AutomotiveBus_Register_Can_MessageTiming_type_hash ((flatbuffers_thash_t)0x129e876a)
#define AutomotiveBus_Register_Can_MessageTiming_type_identifier "\x6a\x87\x9e\x12"
#ifndef AutomotiveBus_Register_Can_Frame_identifier
#define AutomotiveBus_Register_Can_Frame_identifier flatbuffers_identifier
#endif
#define AutomotiveBus_Register_Can_Frame_type_hash ((flatbuffers_thash_t)0x97a9977a)
#define AutomotiveBus_Register_Can_Frame_type_identifier "\x7a\x97\xa9\x97"
#ifndef AutomotiveBus_Register_Can_MetaFrame_identifier
#define AutomotiveBus_Register_Can_MetaFrame_identifier flatbuffers_identifier
#endif
#define AutomotiveBus_Register_Can_MetaFrame_type_hash ((flatbuffers_thash_t)0xb3ddcf21)
#define AutomotiveBus_Register_Can_MetaFrame_type_identifier "\x21\xcf\xdd\xb3"
#ifndef AutomotiveBus_Register_Can_RegisterFile_identifier
#define AutomotiveBus_Register_Can_RegisterFile_identifier flatbuffers_identifier
#endif
#define AutomotiveBus_Register_Can_RegisterFile_type_hash ((flatbuffers_thash_t)0x4382eed2)
#define AutomotiveBus_Register_Can_RegisterFile_type_identifier "\xd2\xee\x82\x43"

/** 
 *     L2 CAN Bus Model
 *     ================ */
typedef int8_t AutomotiveBus_Register_Can_BusState_enum_t;
__flatbuffers_define_integer_type(AutomotiveBus_Register_Can_BusState, AutomotiveBus_Register_Can_BusState_enum_t, 8)
#define AutomotiveBus_Register_Can_BusState_BusOff ((AutomotiveBus_Register_Can_BusState_enum_t)INT8_C(0))
#define AutomotiveBus_Register_Can_BusState_Idle ((AutomotiveBus_Register_Can_BusState_enum_t)INT8_C(1))
#define AutomotiveBus_Register_Can_BusState_Sync ((AutomotiveBus_Register_Can_BusState_enum_t)INT8_C(2))

static inline const char *AutomotiveBus_Register_Can_BusState_name(AutomotiveBus_Register_Can_BusState_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_BusState_BusOff: return "BusOff";
    case AutomotiveBus_Register_Can_BusState_Idle: return "Idle";
    case AutomotiveBus_Register_Can_BusState_Sync: return "Sync";
    default: return "";
    }
}

static inline int AutomotiveBus_Register_Can_BusState_is_known_value(AutomotiveBus_Register_Can_BusState_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_BusState_BusOff: return 1;
    case AutomotiveBus_Register_Can_BusState_Idle: return 1;
    case AutomotiveBus_Register_Can_BusState_Sync: return 1;
    default: return 0;
    }
}

/** 
 *     L3 CAN Bus Model
 *     ================ */
typedef int8_t AutomotiveBus_Register_Can_BufferDirection_enum_t;
__flatbuffers_define_integer_type(AutomotiveBus_Register_Can_BufferDirection, AutomotiveBus_Register_Can_BufferDirection_enum_t, 8)
#define AutomotiveBus_Register_Can_BufferDirection_Tx ((AutomotiveBus_Register_Can_BufferDirection_enum_t)INT8_C(0))
#define AutomotiveBus_Register_Can_BufferDirection_Rx ((AutomotiveBus_Register_Can_BufferDirection_enum_t)INT8_C(1))

static inline const char *AutomotiveBus_Register_Can_BufferDirection_name(AutomotiveBus_Register_Can_BufferDirection_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_BufferDirection_Tx: return "Tx";
    case AutomotiveBus_Register_Can_BufferDirection_Rx: return "Rx";
    default: return "";
    }
}

static inline int AutomotiveBus_Register_Can_BufferDirection_is_known_value(AutomotiveBus_Register_Can_BufferDirection_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_BufferDirection_Tx: return 1;
    case AutomotiveBus_Register_Can_BufferDirection_Rx: return 1;
    default: return 0;
    }
}

typedef int8_t AutomotiveBus_Register_Can_BufferStatus_enum_t;
__flatbuffers_define_integer_type(AutomotiveBus_Register_Can_BufferStatus, AutomotiveBus_Register_Can_BufferStatus_enum_t, 8)
#define AutomotiveBus_Register_Can_BufferStatus_None ((AutomotiveBus_Register_Can_BufferStatus_enum_t)INT8_C(0))
#define AutomotiveBus_Register_Can_BufferStatus_RxError ((AutomotiveBus_Register_Can_BufferStatus_enum_t)INT8_C(1))

static inline const char *AutomotiveBus_Register_Can_BufferStatus_name(AutomotiveBus_Register_Can_BufferStatus_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_BufferStatus_None: return "None";
    case AutomotiveBus_Register_Can_BufferStatus_RxError: return "RxError";
    default: return "";
    }
}

static inline int AutomotiveBus_Register_Can_BufferStatus_is_known_value(AutomotiveBus_Register_Can_BufferStatus_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_BufferStatus_None: return 1;
    case AutomotiveBus_Register_Can_BufferStatus_RxError: return 1;
    default: return 0;
    }
}

typedef int8_t AutomotiveBus_Register_Can_FrameType_enum_t;
__flatbuffers_define_integer_type(AutomotiveBus_Register_Can_FrameType, AutomotiveBus_Register_Can_FrameType_enum_t, 8)
#define AutomotiveBus_Register_Can_FrameType_StandardFrame ((AutomotiveBus_Register_Can_FrameType_enum_t)INT8_C(0))
#define AutomotiveBus_Register_Can_FrameType_ExtendedFrame ((AutomotiveBus_Register_Can_FrameType_enum_t)INT8_C(1))

static inline const char *AutomotiveBus_Register_Can_FrameType_name(AutomotiveBus_Register_Can_FrameType_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_FrameType_StandardFrame: return "StandardFrame";
    case AutomotiveBus_Register_Can_FrameType_ExtendedFrame: return "ExtendedFrame";
    default: return "";
    }
}

static inline int AutomotiveBus_Register_Can_FrameType_is_known_value(AutomotiveBus_Register_Can_FrameType_enum_t value)
{
    switch (value) {
    case AutomotiveBus_Register_Can_FrameType_StandardFrame: return 1;
    case AutomotiveBus_Register_Can_FrameType_ExtendedFrame: return 1;
    default: return 0;
    }
}


struct AutomotiveBus_Register_Can_TimeSpec {
    alignas(8) int64_t psec10;
};
static_assert(sizeof(AutomotiveBus_Register_Can_TimeSpec_t) == 8, "struct size mismatch");

static inline const AutomotiveBus_Register_Can_TimeSpec_t *AutomotiveBus_Register_Can_TimeSpec__const_ptr_add(const AutomotiveBus_Register_Can_TimeSpec_t *p, size_t i) { return p + i; }
static inline AutomotiveBus_Register_Can_TimeSpec_t *AutomotiveBus_Register_Can_TimeSpec__ptr_add(AutomotiveBus_Register_Can_TimeSpec_t *p, size_t i) { return p + i; }
static inline AutomotiveBus_Register_Can_TimeSpec_struct_t AutomotiveBus_Register_Can_TimeSpec_vec_at(AutomotiveBus_Register_Can_TimeSpec_vec_t vec, size_t i)
__flatbuffers_struct_vec_at(vec, i)
static inline size_t AutomotiveBus_Register_Can_TimeSpec__size(void) { return 8; }
static inline size_t AutomotiveBus_Register_Can_TimeSpec_vec_len(AutomotiveBus_Register_Can_TimeSpec_vec_t vec)
__flatbuffers_vec_len(vec)
__flatbuffers_struct_as_root(AutomotiveBus_Register_Can_TimeSpec)

__flatbuffers_define_struct_scalar_field(AutomotiveBus_Register_Can_TimeSpec, psec10, flatbuffers_int64, int64_t)

struct AutomotiveBus_Register_Can_MessageTiming {
    alignas(8) AutomotiveBus_Register_Can_TimeSpec_t send_request;
    alignas(8) AutomotiveBus_Register_Can_TimeSpec_t arbitration;
    alignas(8) AutomotiveBus_Register_Can_TimeSpec_t reception;
};
static_assert(sizeof(AutomotiveBus_Register_Can_MessageTiming_t) == 24, "struct size mismatch");

static inline const AutomotiveBus_Register_Can_MessageTiming_t *AutomotiveBus_Register_Can_MessageTiming__const_ptr_add(const AutomotiveBus_Register_Can_MessageTiming_t *p, size_t i) { return p + i; }
static inline AutomotiveBus_Register_Can_MessageTiming_t *AutomotiveBus_Register_Can_MessageTiming__ptr_add(AutomotiveBus_Register_Can_MessageTiming_t *p, size_t i) { return p + i; }
static inline AutomotiveBus_Register_Can_MessageTiming_struct_t AutomotiveBus_Register_Can_MessageTiming_vec_at(AutomotiveBus_Register_Can_MessageTiming_vec_t vec, size_t i)
__flatbuffers_struct_vec_at(vec, i)
static inline size_t AutomotiveBus_Register_Can_MessageTiming__size(void) { return 24; }
static inline size_t AutomotiveBus_Register_Can_MessageTiming_vec_len(AutomotiveBus_Register_Can_MessageTiming_vec_t vec)
__flatbuffers_vec_len(vec)
__flatbuffers_struct_as_root(AutomotiveBus_Register_Can_MessageTiming)

__flatbuffers_define_struct_struct_field(AutomotiveBus_Register_Can_MessageTiming, send_request, AutomotiveBus_Register_Can_TimeSpec_struct_t)
__flatbuffers_define_struct_struct_field(AutomotiveBus_Register_Can_MessageTiming, arbitration, AutomotiveBus_Register_Can_TimeSpec_struct_t)
__flatbuffers_define_struct_struct_field(AutomotiveBus_Register_Can_MessageTiming, reception, AutomotiveBus_Register_Can_TimeSpec_struct_t)


struct AutomotiveBus_Register_Can_CanStatus_table { uint8_t unused__; };

static inline size_t AutomotiveBus_Register_Can_CanStatus_vec_len(AutomotiveBus_Register_Can_CanStatus_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AutomotiveBus_Register_Can_CanStatus_table_t AutomotiveBus_Register_Can_CanStatus_vec_at(AutomotiveBus_Register_Can_CanStatus_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AutomotiveBus_Register_Can_CanStatus_table_t, vec, i, 0)
__flatbuffers_table_as_root(AutomotiveBus_Register_Can_CanStatus)

__flatbuffers_define_scalar_field(0, AutomotiveBus_Register_Can_CanStatus, sync, AutomotiveBus_Register_Can_BusState, AutomotiveBus_Register_Can_BusState_enum_t, INT8_C(1))

struct AutomotiveBus_Register_Can_Frame_table { uint8_t unused__; };

static inline size_t AutomotiveBus_Register_Can_Frame_vec_len(AutomotiveBus_Register_Can_Frame_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AutomotiveBus_Register_Can_Frame_table_t AutomotiveBus_Register_Can_Frame_vec_at(AutomotiveBus_Register_Can_Frame_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AutomotiveBus_Register_Can_Frame_table_t, vec, i, 0)
__flatbuffers_table_as_root(AutomotiveBus_Register_Can_Frame)

__flatbuffers_define_scalar_field(0, AutomotiveBus_Register_Can_Frame, frame_id, flatbuffers_uint32, uint32_t, UINT32_C(0))
__flatbuffers_define_vector_field(1, AutomotiveBus_Register_Can_Frame, payload, flatbuffers_uint8_vec_t, 0)
__flatbuffers_define_scalar_field(2, AutomotiveBus_Register_Can_Frame, length, flatbuffers_uint8, uint8_t, UINT8_C(0))
__flatbuffers_define_scalar_field(3, AutomotiveBus_Register_Can_Frame, rtr, flatbuffers_bool, flatbuffers_bool_t, UINT8_C(0))
__flatbuffers_define_scalar_field(4, AutomotiveBus_Register_Can_Frame, frame_type, AutomotiveBus_Register_Can_FrameType, AutomotiveBus_Register_Can_FrameType_enum_t, INT8_C(0))

struct AutomotiveBus_Register_Can_MetaFrame_table { uint8_t unused__; };

static inline size_t AutomotiveBus_Register_Can_MetaFrame_vec_len(AutomotiveBus_Register_Can_MetaFrame_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AutomotiveBus_Register_Can_MetaFrame_table_t AutomotiveBus_Register_Can_MetaFrame_vec_at(AutomotiveBus_Register_Can_MetaFrame_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AutomotiveBus_Register_Can_MetaFrame_table_t, vec, i, 0)
__flatbuffers_table_as_root(AutomotiveBus_Register_Can_MetaFrame)

__flatbuffers_define_scalar_field(0, AutomotiveBus_Register_Can_MetaFrame, status, AutomotiveBus_Register_Can_BufferStatus, AutomotiveBus_Register_Can_BufferStatus_enum_t, INT8_C(0))
__flatbuffers_define_scalar_field(1, AutomotiveBus_Register_Can_MetaFrame, direction, AutomotiveBus_Register_Can_BufferDirection, AutomotiveBus_Register_Can_BufferDirection_enum_t, INT8_C(0))
__flatbuffers_define_scalar_field(2, AutomotiveBus_Register_Can_MetaFrame, can_fd_enabled, flatbuffers_bool, flatbuffers_bool_t, UINT8_C(0))
__flatbuffers_define_table_field(3, AutomotiveBus_Register_Can_MetaFrame, frame, AutomotiveBus_Register_Can_Frame_table_t, 0)
__flatbuffers_define_struct_field(4, AutomotiveBus_Register_Can_MetaFrame, timing, AutomotiveBus_Register_Can_MessageTiming_struct_t, 0)

/** 
 *     CAN Register File
 *     ================= */
struct AutomotiveBus_Register_Can_RegisterFile_table { uint8_t unused__; };

static inline size_t AutomotiveBus_Register_Can_RegisterFile_vec_len(AutomotiveBus_Register_Can_RegisterFile_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AutomotiveBus_Register_Can_RegisterFile_table_t AutomotiveBus_Register_Can_RegisterFile_vec_at(AutomotiveBus_Register_Can_RegisterFile_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AutomotiveBus_Register_Can_RegisterFile_table_t, vec, i, 0)
__flatbuffers_table_as_root(AutomotiveBus_Register_Can_RegisterFile)

__flatbuffers_define_vector_field(0, AutomotiveBus_Register_Can_RegisterFile, buffer, AutomotiveBus_Register_Can_MetaFrame_vec_t, 0)


#include "flatcc/flatcc_epilogue.h"
#endif /* CAN_READER_H */
