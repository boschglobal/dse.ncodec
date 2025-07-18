// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/interface/frame.h>
#include <dse/ncodec/schema/abs/stream/frame_builder.h>


#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AutomotiveBus_Stream_Frame, x)


static void initialize_stream(ABCodecInstance* nc)
{
    if (nc->fbs_stream_initalized) return;

    flatcc_builder_t* B = &nc->fbs_builder;
    flatcc_builder_reset(B);
    ns(Stream_start_as_root_with_size(B));
    ns(Stream_frames_start(B));
    nc->fbs_stream_initalized = true;
}


static void reset_stream(ABCodecInstance* nc)
{
    if (nc->fbs_stream_initalized == false) return;

    flatcc_builder_t* B = &nc->fbs_builder;
    flatcc_builder_reset(B);
    nc->fbs_stream_initalized = false;
}


static void finalize_stream(
    ABCodecInstance* nc, uint8_t** buffer, size_t* length)
{
    if (nc->fbs_stream_initalized == false) {
        *buffer = NULL;
        *length = 0;
        return;
    }

    flatcc_builder_t* B = &nc->fbs_builder;
    ns(Stream_frames_end(B));
    ns(Stream_end_as_root(B));
    *buffer = flatcc_builder_finalize_buffer(B, length);
    reset_stream(nc);
}


int32_t can_write(NCODEC* nc, NCodecMessage* msg)
{
    ABCodecInstance*  _nc = (ABCodecInstance*)nc;
    NCodecCanMessage* _msg = (NCodecCanMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    flatcc_builder_t* B = &_nc->fbs_builder;

    initialize_stream(_nc);
    ns(Stream_frames_push_start(B));
    ns(CanFrame_start(B));
    /* Encode the message. */
    ns(CanFrame_frame_id_add(B, _msg->frame_id));
    ns(CanFrame_frame_type_add(B, _msg->frame_type));
    ns(CanFrame_payload_add(
        B, flatbuffers_uint8_vec_create(B, _msg->buffer, _msg->len)));
    /* Add additional metadata. */
    ns(CanFrame_bus_id_add(B, _nc->bus_id));
    ns(CanFrame_node_id_add(B, _nc->node_id));
    ns(CanFrame_interface_id_add(B, _nc->interface_id));
    /* Complete the encoding. */
    ns(Frame_f_CanFrame_add(B, ns(CanFrame_end(B))));
    ns(Stream_frames_push_end(B));

    return _msg->len;
}


static void get_msg_from_stream(NCODEC* nc)
{
    ABCodecInstance* _nc = (ABCodecInstance*)nc;

    /* Reset the message (and frame) parsing state. */
    _nc->msg_ptr = NULL;
    _nc->msg_len = 0;
    _nc->vector = NULL;
    _nc->vector_idx = 0;
    _nc->vector_len = 0;

    /* Next message? */
    uint8_t* buffer;
    size_t   length;
    _nc->c.stream->read(nc, &buffer, &length, NCODEC_POS_NC);

    uint8_t*       msg_ptr = buffer;
    uint8_t* const buffer_ptr = buffer;
    while ((size_t)(msg_ptr - buffer_ptr) < length) {
        /* Messages start with a size prefix. */
        size_t msg_len = 0;
        msg_ptr = flatbuffers_read_size_prefix(msg_ptr, &msg_len);
        if (msg_len == 0) break;
        /* Advance the stream pos (+4 for size prefix). */
        _nc->c.stream->seek(nc, msg_len + 4, NCODEC_SEEK_CUR);
        /* Set the parsing state. */
        if (flatbuffers_has_identifier(msg_ptr, flatbuffers_identifier)) {
            _nc->msg_ptr = msg_ptr;
            _nc->msg_len = msg_len;
            return;
        }
        /* Next message in the stream. */
        msg_ptr += msg_len;
    }

    /* No message in stream. */
    _nc->c.stream->seek(nc, 0, NCODEC_SEEK_END);
}

static void get_vector_from_message(NCODEC* nc)
{
    ABCodecInstance* _nc = (ABCodecInstance*)nc;

    /* Reset the frame parsing state. */
    _nc->vector = NULL;
    _nc->vector_idx = 0;
    _nc->vector_len = 0;

    /* Guard conditions. */
    if (_nc->msg_ptr == NULL) return;

    /* Decode the vector of frames. */
    ns(Stream_table_t) stream = ns(Stream_as_root(_nc->msg_ptr));
    _nc->vector = ns(Stream_frames(stream));
    _nc->vector_len = ns(Frame_vec_len(_nc->vector));
}


int32_t can_read(NCODEC* nc, NCodecMessage* msg)
{
    ABCodecInstance*  _nc = (ABCodecInstance*)nc;
    NCodecCanMessage* _msg = (NCodecCanMessage*)msg;
    if (_nc == NULL) return -ENOSTR;
    if (_msg == NULL) return -EINVAL;
    if (_nc->c.stream == NULL) return -ENOSR;

    /* Reset the message, in case caller ignores the return value. */
    _msg->len = 0;
    _msg->frame_type = CAN_BASE_FRAME;
    _msg->buffer = NULL;

    /* Process the stream/frames. */
    if (_nc->msg_ptr == NULL) get_msg_from_stream(nc);
    if (_nc->vector == NULL) get_vector_from_message(nc);
    while (_nc->msg_ptr && _nc->vector) {
        for (uint32_t _vi = _nc->vector_idx; _vi < _nc->vector_len; _vi++) {
            ns(Frame_table_t) frame = ns(Frame_vec_at(_nc->vector, _vi));
            if (!ns(Frame_f_is_present(frame))) continue;
            ns(FrameTypes_union_type_t) frame_type = ns(Frame_f_type(frame));
            if (frame_type != ns(FrameTypes_CanFrame)) continue;

            /* Filter: sender==receiver. */
            ns(CanFrame_table_t) can_frame =
                (ns(CanFrame_table_t))ns(Frame_f(frame));
            if ((_nc->node_id) &&
                (_nc->node_id == ns(CanFrame_node_id(can_frame))))
                continue;

            /* Return the message. */
            _msg->frame_id = ns(CanFrame_frame_id(can_frame));
            _msg->frame_type = ns(CanFrame_frame_type(can_frame));
            flatbuffers_uint8_vec_t payload = ns(CanFrame_payload(can_frame));
            _msg->buffer =
                (uint8_t*)payload;  // TODO think about this cast ... caller
                                    // should not modify ... restrict?
            _msg->len = flatbuffers_uint8_vec_len(payload);
            _msg->sender.bus_id = ns(CanFrame_bus_id(can_frame));
            _msg->sender.node_id = ns(CanFrame_node_id(can_frame));
            _msg->sender.interface_id = ns(CanFrame_interface_id(can_frame));

            /* ... but don't forget to save the vector index either. */
            _nc->vector_idx = _vi + 1;
            return _msg->len;
        }

        /* Next msg/vector? */
        get_msg_from_stream(nc);
        if (_nc->msg_ptr) get_vector_from_message(nc);
    }
    /* No messages in stream. */
    _nc->c.stream->seek(nc, 0, NCODEC_SEEK_END);
    return -ENOMSG;
}


int32_t can_flush(NCODEC* nc)
{
    ABCodecInstance* _nc = (ABCodecInstance*)nc;
    if (_nc == NULL) return -ENOSTR;
    if (_nc->c.stream == NULL) return -ENOSR;

    uint8_t* buffer = NULL;
    size_t   length = 0;

    finalize_stream(_nc, &buffer, &length);
    if (buffer) {
        _nc->c.stream->write(nc, buffer, length);
        free(buffer);
    }
    return length;
}


int32_t can_truncate(NCODEC* nc)
{
    ABCodecInstance* _nc = (ABCodecInstance*)nc;
    if (_nc == NULL) return -ENOSTR;
    if (_nc->c.stream == NULL) return -ENOSR;

    reset_stream(_nc);
    _nc->c.stream->seek(nc, 0, NCODEC_SEEK_RESET);

    return 0;
}
