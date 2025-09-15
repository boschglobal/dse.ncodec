// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <dse/platform.h>
#include <dse/ncodec/codec.h>

#define UNUSED(x) ((void)x)


/* Declare an extension to the NCodecStreamVTable type. */
typedef struct __stream {
    NCodecStreamVTable s;

    uint8_t* buffer;
    size_t   buffer_len;
    size_t   len;
    size_t   pos;
    bool     resizable;
} __stream;


DLL_PRIVATE size_t stream_read(
    NCODEC* nc, uint8_t** data, size_t* len, int32_t pos_op)
{
    NCodecInstance* _nc = (NCodecInstance*)nc;
    if (_nc == NULL || _nc->stream == NULL) return -ENOSTR;
    if (data == NULL || len == NULL) return -EINVAL;

    __stream* _s = (__stream*)_nc->stream;
    /* Check EOF. */
    if (_s->pos >= _s->len) {
        *data = NULL;
        *len = 0;
        return 0;
    }
    /* Return buffer, from current pos. */
    *data = &_s->buffer[_s->pos];
    *len = _s->len - _s->pos;
    /* Advance the position indicator. */
    if (pos_op == NCODEC_POS_UPDATE) _s->pos = _s->len;

    return *len;
}

DLL_PRIVATE size_t stream_write(NCODEC* nc, uint8_t* data, size_t len)
{
    NCodecInstance* _nc = (NCodecInstance*)nc;
    if (_nc == NULL || _nc->stream == NULL) return -ENOSTR;

    __stream* _s = (__stream*)_nc->stream;

    if ((_s->pos + len) > _s->buffer_len) {
        if (_s->resizable) {
            _s->buffer = realloc(_s->buffer, _s->pos + len);
        } else {
            return -EMSGSIZE;
        }
    }
    memcpy(&_s->buffer[_s->pos], data, len);
    _s->pos += len;
    if (_s->pos > _s->len) _s->len = _s->pos;
    return len;
}

DLL_PRIVATE int64_t stream_seek(NCODEC* nc, size_t pos, int32_t op)
{
    NCodecInstance* _nc = (NCodecInstance*)nc;
    if (_nc && _nc->stream) {
        __stream* _s = (__stream*)_nc->stream;
        if (op == NCODEC_SEEK_SET) {
            if (pos > _s->len) {
                _s->pos = _s->len;
            } else {
                _s->pos = pos;
            }
        } else if (op == NCODEC_SEEK_CUR) {
            pos = _s->pos + pos;
            if (pos > _s->len) {
                _s->pos = _s->len;
            } else {
                _s->pos = pos;
            }
        } else if (op == NCODEC_SEEK_END) {
            _s->pos = _s->len;
        } else if (op == NCODEC_SEEK_RESET) {
            _s->pos = _s->len = 0;
        } else if (op == 42) {
            _s->pos = _s->len = _s->buffer_len;
        } else {
            return -EINVAL;
        }

        return _s->pos;
    }
    return -ENOSTR;
}

DLL_PRIVATE int64_t stream_tell(NCODEC* nc)
{
    NCodecInstance* _nc = (NCodecInstance*)nc;
    if (_nc && _nc->stream) {
        __stream* _s = (__stream*)_nc->stream;
        return _s->pos;
    }
    return -ENOSTR;
}

DLL_PRIVATE int32_t stream_eof(NCODEC* nc)
{
    NCodecInstance* _nc = (NCodecInstance*)nc;
    if (_nc && _nc->stream) {
        __stream* _s = (__stream*)_nc->stream;
        if (_s->pos < _s->len) return 0;
    }
    return 1;
}

DLL_PRIVATE int32_t stream_close(NCODEC* nc)
{
    NCodecInstance* _nc = (NCodecInstance*)nc;
    if (_nc && _nc->stream) {
        __stream* _s = (__stream*)_nc->stream;
        free(_s->buffer);
        free(_nc->stream);
        _nc->stream = NULL;
        return 0;
    }
    return -ENOSTR;
}


/* Public stream interface. */
void* ncodec_buffer_stream_create(size_t buffer_size)
{
    __stream* stream = calloc(1, sizeof(__stream));
    *stream = (__stream){
        .s =
            (struct NCodecStreamVTable){
                .read = stream_read,
                .write = stream_write,
                .seek = stream_seek,
                .tell = stream_tell,
                .eof = stream_eof,
                .close = stream_close,
            },
        .len = 0,
        .pos = 0,
    };

    if (buffer_size) {
        stream->buffer = calloc(buffer_size, sizeof(uint8_t));
        stream->buffer_len = buffer_size;
        stream->resizable = true;
    } else {
        stream->buffer_len = 0;
        stream->resizable = true;
    }

    return stream;
}
