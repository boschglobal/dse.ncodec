// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdint.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/diag/codec.h>
#include <dse/ncodec/interface/diag/tp.h>
#include <dse/ncodec/schema/abs/diagnostic/tp_builder.h>


#undef ns
#define ns(x)    FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Diagnostic_Tp, x)
#define isotp(x) FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Diagnostic_IsoTp, x)
#define doip(x)  FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Diagnostic_DoIp, x)


extern void*    diag_alloc(DiagCodecInstance* nc, size_t count, size_t size);
extern uint8_t* diag_stream_read(
    DiagCodecInstance* nc, const char* identifier, size_t* length);
extern int32_t diag_stream_write(DiagCodecInstance* nc);


static uint32_t _emit_single_frame(
    flatcc_builder_t* B, const NCodecDiagIsoTpSingleFrame* f)
{
    isotp(SingleFrame_start(B));
    isotp(SingleFrame_data_length_add(B, f->data_length));
    if (f->payload) {
        isotp(SingleFrame_payload_add(
            B, flatbuffers_uint8_vec_create(B, f->payload, f->payload_len)));
    }
    return isotp(SingleFrame_end(B));
}

static uint32_t _emit_first_frame(
    flatcc_builder_t* B, const NCodecDiagIsoTpFirstFrame* f)
{
    isotp(FirstFrame_start(B));
    isotp(FirstFrame_total_length_add(B, f->total_length));
    if (f->payload) {
        isotp(FirstFrame_payload_add(
            B, flatbuffers_uint8_vec_create(B, f->payload, f->payload_len)));
    }
    return isotp(FirstFrame_end(B));
}

static uint32_t _emit_consecutive_frame(
    flatcc_builder_t* B, const NCodecDiagIsoTpConsecutiveFrame* f)
{
    isotp(ConsecutiveFrame_start(B));
    isotp(ConsecutiveFrame_sequence_number_add(B, f->sequence_number));
    if (f->payload) {
        isotp(ConsecutiveFrame_payload_add(
            B, flatbuffers_uint8_vec_create(B, f->payload, f->payload_len)));
    }
    return isotp(ConsecutiveFrame_end(B));
}

static uint32_t _emit_flow_control(
    flatcc_builder_t* B, const NCodecDiagIsoTpFlowControl* f)
{
    isotp(FlowControl_start(B));
    isotp(FlowControl_status_add(B, f->status));
    isotp(FlowControl_block_size_add(B, f->block_size));
    isotp(FlowControl_st_min_ms_add(B, f->st_min_ms));
    return isotp(FlowControl_end(B));
}

static uint32_t _emit_frame(flatcc_builder_t* B, const NCodecDiagIsoTpFrame* f)
{
    isotp(SingleFrame_ref_t) single_frame = 0;
    isotp(FirstFrame_ref_t) first_frame = 0;
    isotp(ConsecutiveFrame_ref_t) consecutive_frame = 0;
    isotp(FlowControl_ref_t) flow_control = 0;

    switch (f->frame_type) {
    case NCodecDiagIsoTpFramePayloadTypeSingleFrame:
        single_frame = _emit_single_frame(B, &f->frame.single_frame);
        break;
    case NCodecDiagIsoTpFramePayloadTypeFirstFrame:
        first_frame = _emit_first_frame(B, &f->frame.first_frame);
        break;
    case NCodecDiagIsoTpFramePayloadTypeConsecutiveFrame:
        consecutive_frame =
            _emit_consecutive_frame(B, &f->frame.consecutive_frame);
        break;
    case NCodecDiagIsoTpFramePayloadTypeFlowControl:
        flow_control = _emit_flow_control(B, &f->frame.flow_control);
        break;
    default:
        break;
    }

    isotp(Frame_start(B));
    if (single_frame) {
        isotp(Frame_frame_SingleFrame_add(B, single_frame));
    } else if (first_frame) {
        isotp(Frame_frame_FirstFrame_add(B, first_frame));
    } else if (consecutive_frame) {
        isotp(Frame_frame_ConsecutiveFrame_add(B, consecutive_frame));
    } else if (flow_control) {
        isotp(Frame_frame_FlowControl_add(B, flow_control));
    }
    isotp(Frame_timestamp_us_add(B, f->timestamp_us));
    return isotp(Frame_end(B));
}

static uint32_t _emit_iso_tp_metadata(
    flatcc_builder_t* B, const NCodecDiagIsoTpMetadata* md)
{
    isotp(Metadata_start(B));
    isotp(Metadata_can_id_add(B, md->can_id));
    isotp(Metadata_total_length_add(B, md->total_length));
    return isotp(Metadata_end(B));
}

static uint32_t _emit_do_ip_metadata(
    flatcc_builder_t* B, const NCodecDiagDoIpMetadata* md)
{
    doip(Metadata_start(B));
    doip(Metadata_protocol_version_add(B, md->protocol_version));
    doip(Metadata_payload_type_add(B, md->payload_type));
    doip(Metadata_source_address_add(B, md->source_address));
    doip(Metadata_target_address_add(B, md->target_address));
    return doip(Metadata_end(B));
}

static void _emit_message(flatcc_builder_t* B, const NCodecDiagTpMessage* m)
{
    ns(Message_start_as_root_with_size(B));
    switch (m->transport_meta_type) {
    case NCodecDiagTpTransportMetadataTypeIsoTp:
        ns(Message_transport_meta_IsoTp_add(
            B, _emit_iso_tp_metadata(B, &m->transport_meta.iso_tp)));
        break;
    case NCodecDiagTpTransportMetadataTypeDoIp:
        ns(Message_transport_meta_DoIp_add(
            B, _emit_do_ip_metadata(B, &m->transport_meta.do_ip)));
        break;
    default:
        break;
    }
    if (m->payload) {
        ns(Message_payload_add(
            B, flatbuffers_uint8_vec_create(B, m->payload, m->payload_len)));
    }
    if (m->frames) {
        ns(Message_frames_start(B));
        for (size_t i = 0; i < m->frames_len; i++) {
            ns(Message_frames_push(B, _emit_frame(B, &m->frames[i])));
        }
        ns(Message_frames_end(B));
    }
    ns(Message_start_timestamp_us_add(B, m->start_timestamp_us));
    ns(Message_end_timestamp_us_add(B, m->end_timestamp_us));
    ns(Message_end_as_root(B));
}


static void _decode_frame(isotp(Frame_table_t) t, NCodecDiagIsoTpFrame* f)
{
    switch (isotp(Frame_frame_type(t))) {
    case isotp(FramePayload_SingleFrame): {
        NCodecDiagIsoTpSingleFrame* sf = &f->frame.single_frame;
        isotp(SingleFrame_table_t) ft =
            (isotp(SingleFrame_table_t))isotp(Frame_frame(t));
        f->frame_type = NCodecDiagIsoTpFramePayloadTypeSingleFrame;
        sf->data_length = isotp(SingleFrame_data_length(ft));
        sf->payload = isotp(SingleFrame_payload(ft));
        sf->payload_len = flatbuffers_uint8_vec_len(sf->payload);
    } break;
    case isotp(FramePayload_FirstFrame): {
        NCodecDiagIsoTpFirstFrame* ff = &f->frame.first_frame;
        isotp(FirstFrame_table_t) ft =
            (isotp(FirstFrame_table_t))isotp(Frame_frame(t));
        f->frame_type = NCodecDiagIsoTpFramePayloadTypeFirstFrame;
        ff->total_length = isotp(FirstFrame_total_length(ft));
        ff->payload = isotp(FirstFrame_payload(ft));
        ff->payload_len = flatbuffers_uint8_vec_len(ff->payload);
    } break;
    case isotp(FramePayload_ConsecutiveFrame): {
        NCodecDiagIsoTpConsecutiveFrame* cf = &f->frame.consecutive_frame;
        isotp(ConsecutiveFrame_table_t) ft =
            (isotp(ConsecutiveFrame_table_t))isotp(Frame_frame(t));
        f->frame_type = NCodecDiagIsoTpFramePayloadTypeConsecutiveFrame;
        cf->sequence_number = isotp(ConsecutiveFrame_sequence_number(ft));
        cf->payload = isotp(ConsecutiveFrame_payload(ft));
        cf->payload_len = flatbuffers_uint8_vec_len(cf->payload);
    } break;
    case isotp(FramePayload_FlowControl): {
        NCodecDiagIsoTpFlowControl* fc = &f->frame.flow_control;
        isotp(FlowControl_table_t) ft =
            (isotp(FlowControl_table_t))isotp(Frame_frame(t));
        f->frame_type = NCodecDiagIsoTpFramePayloadTypeFlowControl;
        fc->status = (NCodecDiagIsoTpFlowStatus)isotp(FlowControl_status(ft));
        fc->block_size = isotp(FlowControl_block_size(ft));
        fc->st_min_ms = isotp(FlowControl_st_min_ms(ft));
    } break;
    default:
        break;
    }
    f->timestamp_us = isotp(Frame_timestamp_us(t));
}

static void _decode_message(
    DiagCodecInstance* nc, ns(Message_table_t) t, NCodecDiagTpMessage* m)
{
    switch (ns(Message_transport_meta_type(t))) {
    case ns(TransportMetadata_IsoTp): {
        NCodecDiagIsoTpMetadata* md = &m->transport_meta.iso_tp;
        isotp(Metadata_table_t) mt =
            (isotp(Metadata_table_t))ns(Message_transport_meta(t));
        m->transport_meta_type = NCodecDiagTpTransportMetadataTypeIsoTp;
        md->can_id = isotp(Metadata_can_id(mt));
        md->total_length = isotp(Metadata_total_length(mt));
    } break;
    case ns(TransportMetadata_DoIp): {
        NCodecDiagDoIpMetadata* md = &m->transport_meta.do_ip;
        doip(Metadata_table_t) mt =
            (doip(Metadata_table_t))ns(Message_transport_meta(t));
        m->transport_meta_type = NCodecDiagTpTransportMetadataTypeDoIp;
        md->protocol_version = doip(Metadata_protocol_version(mt));
        md->payload_type =
            (NCodecDiagDoIpPayloadType)doip(Metadata_payload_type(mt));
        md->source_address = doip(Metadata_source_address(mt));
        md->target_address = doip(Metadata_target_address(mt));
    } break;
    default:
        break;
    }
    m->payload = ns(Message_payload(t));
    m->payload_len = flatbuffers_uint8_vec_len(m->payload);

    isotp(Frame_vec_t) frames = ns(Message_frames(t));
    if (frames) {
        size_t                len = isotp(Frame_vec_len(frames));
        NCodecDiagIsoTpFrame* data =
            diag_alloc(nc, len, sizeof(NCodecDiagIsoTpFrame));
        for (size_t i = 0; i < len; i++) {
            _decode_frame(isotp(Frame_vec_at(frames, i)), &data[i]);
        }
        m->frames = data;
        m->frames_len = len;
    }
    m->start_timestamp_us = ns(Message_start_timestamp_us(t));
    m->end_timestamp_us = ns(Message_end_timestamp_us(t));
}


int32_t tp_write(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*   _nc = (DiagCodecInstance*)nc;
    NCodecDiagTpMessage* _msg = (NCodecDiagTpMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    flatcc_builder_t* B = &_nc->fbs_builder;
    flatcc_builder_reset(B);
    _emit_message(B, _msg);
    return diag_stream_write(_nc);
}


int32_t tp_read(NCODEC* nc, NCodecMessage* msg)
{
    DiagCodecInstance*   _nc = (DiagCodecInstance*)nc;
    NCodecDiagTpMessage* _msg = (NCodecDiagTpMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    /* Reset the message, in case caller ignores the return value. */
    *_msg = (NCodecDiagTpMessage){};

    size_t   length = 0;
    uint8_t* buffer = diag_stream_read(_nc, flatbuffers_identifier, &length);
    if (buffer == NULL) return -ENOMSG;

    _decode_message(_nc, ns(Message_as_root(buffer)), _msg);
    return length;
}
