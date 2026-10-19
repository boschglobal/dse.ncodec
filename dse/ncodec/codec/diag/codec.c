// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/diag/codec.h>


#define UNUSED(x) ((void)x)
#define CODEC     "application/x-automotive-bus"


/* interface=diagnostic; type=uds; schema=fbs */
extern int32_t uds_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t uds_read(NCODEC* nc, NCodecMessage* msg);

/* interface=diagnostic; type=tp; schema=fbs */
extern int32_t tp_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t tp_read(NCODEC* nc, NCodecMessage* msg);

/* interface=diagnostic; type=srpc; schema=fbs */
extern int32_t srpc_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t srpc_read(NCODEC* nc, NCodecMessage* msg);

/* interface=diagnostic; type=xil; schema=fbs */
extern int32_t xil_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t xil_read(NCODEC* nc, NCodecMessage* msg);


static char* trim(char* s)
{
    if (s == NULL) return s;

    while (*s && isspace(*s))
        s++;
    char* e = s + strlen(s);
    while ((s != e) && isspace(*(e - 1)))
        e--;
    *e = '\0';
    return s;
}

static void __free_item(void* item, void* data)
{
    UNUSED(data);
    void** _item = item;
    free(*_item);
}

void* diag_alloc(DiagCodecInstance* nc, size_t count, size_t size)
{
    /* Allocate at least one item, so that empty vectors remain present. */
    void* data = calloc(count ? count : 1, size);
    if (nc->free_list.capacity == 0) {
        nc->free_list = vector_make(sizeof(void*), 0, NULL);
    }
    vector_push(&nc->free_list, &data);
    return data;
}

static void free_codec(DiagCodecInstance* _nc)
{
    if (_nc == NULL) return;

    if (_nc->interface) free(_nc->interface);
    if (_nc->type) free(_nc->type);
    if (_nc->schema) free(_nc->schema);
    if (_nc->node_id_str) free(_nc->node_id_str);
    if (_nc->swc_id_str) free(_nc->swc_id_str);
    if (_nc->ecu_id_str) free(_nc->ecu_id_str);
    if (_nc->name) free(_nc->name);
    if (_nc->loopback_str) free(_nc->loopback_str);

    if (_nc->fbs_builder_initalized) flatcc_builder_clear(&_nc->fbs_builder);

    vector_clear(&_nc->free_list, __free_item, NULL);
    vector_reset(&_nc->free_list);
}


uint8_t* diag_stream_read(
    DiagCodecInstance* nc, const char* identifier, size_t* length)
{
    NCodecStreamVTable* stream = (NCodecStreamVTable*)nc->c.stream;

    /* Next message? */
    uint8_t* buffer = NULL;
    size_t   buffer_len = 0;
    stream->read((NCODEC*)nc, &buffer, &buffer_len, NCODEC_POS_NC);

    uint8_t*       msg_ptr = buffer;
    uint8_t* const buffer_ptr = buffer;
    while ((size_t)(msg_ptr - buffer_ptr) < buffer_len) {
        size_t remaining = buffer_len - (size_t)(msg_ptr - buffer_ptr);
        if (remaining < sizeof(flatbuffers_uoffset_t)) break;

        /* Messages start with a size prefix. */
        size_t msg_len = 0;
        msg_ptr = flatbuffers_read_size_prefix(msg_ptr, &msg_len);
        if (msg_len == 0) break;
        if (msg_len > remaining - sizeof(flatbuffers_uoffset_t)) break;
        /* Advance the stream pos (+4 for size prefix). */
        stream->seek((NCODEC*)nc, msg_len + sizeof(flatbuffers_uoffset_t),
            NCODEC_SEEK_CUR);
        /* Return the message. */
        if (flatbuffers_has_identifier(msg_ptr, identifier)) {
            *length = msg_len + sizeof(flatbuffers_uoffset_t);
            return msg_ptr;
        }
        /* Next message in the stream. */
        msg_ptr += msg_len;
    }

    /* No message in stream. */
    stream->seek((NCODEC*)nc, 0, NCODEC_SEEK_END);
    *length = 0;
    return NULL;
}


int32_t diag_stream_write(DiagCodecInstance* nc)
{
    NCodecStreamVTable* stream = (NCodecStreamVTable*)nc->c.stream;

    uint8_t* buffer = NULL;
    size_t   length = 0;

    buffer = flatcc_builder_finalize_buffer(&nc->fbs_builder, &length);
    flatcc_builder_reset(&nc->fbs_builder);
    if (buffer) {
        stream->write((NCODEC*)nc, buffer, length);
        free(buffer);
    }
    return length;
}


static int32_t codec_config(NCODEC* nc, NCodecConfigItem item)
{
    DiagCodecInstance* _nc = (DiagCodecInstance*)nc;
    if (_nc == NULL) return -ENOSTR;

    /* Selectors. */
    if (strcmp(item.name, "interface") == 0) {
        if (_nc->interface) free(_nc->interface);
        _nc->interface = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "type") == 0) {
        if (_nc->type) free(_nc->type);
        _nc->type = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "schema") == 0) {
        if (_nc->schema) free(_nc->schema);
        _nc->schema = strdup(item.value);
        return 0;
    }

    /* Parameters. */
    if (strcmp(item.name, "node_id") == 0) {
        if (_nc->node_id_str) free(_nc->node_id_str);
        _nc->node_id_str = strdup(item.value);
        _nc->node_id = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "swc_id") == 0) {
        if (_nc->swc_id_str) free(_nc->swc_id_str);
        _nc->swc_id_str = strdup(item.value);
        _nc->swc_id = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "ecu_id") == 0) {
        if (_nc->ecu_id_str) free(_nc->ecu_id_str);
        _nc->ecu_id_str = strdup(item.value);
        _nc->ecu_id = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "name") == 0) {
        if (_nc->name) free(_nc->name);
        _nc->name = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "loopback") == 0) {
        if (_nc->loopback_str) free(_nc->loopback_str);
        _nc->loopback_str = strdup(item.value);
        _nc->loopback = strtoul(item.value, NULL, 10);
        return 0;
    }

    return -EINVAL;
}


static NCodecConfigItem codec_stat(NCODEC* nc, int32_t* index)
{
    DiagCodecInstance* _nc = (DiagCodecInstance*)nc;
    if ((_nc == NULL) || (index == NULL)) {
        return (struct NCodecConfigItem){};
    }

    const char* name = NULL;
    const char* value = NULL;
    switch (*index) {
    case 0:
        name = "interface";
        value = _nc->interface;
        break;
    case 1:
        name = "type";
        value = _nc->type;
        break;
    case 2:
        name = "schema";
        value = _nc->schema;
        break;
    case 3:
        name = "node_id";
        value = _nc->node_id_str;
        break;
    case 4:
        name = "swc_id";
        value = _nc->swc_id_str;
        break;
    case 5:
        name = "ecu_id";
        value = _nc->ecu_id_str;
        break;
    case 6:
        name = "name";
        value = _nc->name;
        break;
    case 7:
        name = "loopback";
        value = _nc->loopback_str;
        break;
    default:
        *index = -1;
    }

    return (struct NCodecConfigItem){
        .name = name,
        .value = value,
    };
}


static int32_t codec_flush(NCODEC* nc)
{
    DiagCodecInstance* _nc = (DiagCodecInstance*)nc;
    if (_nc == NULL) return -ENOSTR;
    if (_nc->c.stream == NULL) return -ENOSR;

    /* Each write emits a complete message, nothing to flush. */
    return 0;
}


static int32_t codec_truncate(NCODEC* nc)
{
    DiagCodecInstance* _nc = (DiagCodecInstance*)nc;
    if (_nc == NULL) return -ENOSTR;
    if (_nc->c.stream == NULL) return -ENOSR;
    NCodecStreamVTable* stream = (NCodecStreamVTable*)_nc->c.stream;

    stream->seek(nc, 0, NCODEC_SEEK_RESET);
    vector_clear(&_nc->free_list, __free_item, NULL);

    return 0;
}


static void codec_close(NCODEC* nc)
{
    if (nc == NULL) return;
    free_codec((DiagCodecInstance*)nc);
    free(nc);
}


NCODEC* diag_codec_create(const char* mime_type)
{
    if (mime_type == NULL) return NULL;

    char*              _buf = strdup(mime_type);
    char*              _pos = NULL;
    DiagCodecInstance* _nc = NULL;

    /* Check the MIMEtype is correct. */
    char* _codec_name = strtok_r(_buf, "; ", &_pos);
    if (_codec_name == NULL ||
        strncmp(_codec_name, CODEC, strlen(CODEC)) != 0) {
        goto create_fail;
    }

    /* Allocate the codec object. */
    _nc = calloc(1, sizeof(DiagCodecInstance));
    _nc->c.mime_type = mime_type;

    /* Parse out the remaining parameters from the MIMEtype. */
    char* _param;
    while ((_param = strtok_r(NULL, "; ", &_pos)) != NULL) {
        char* name = _param;
        char* value = strchr(_param, '=');
        if (value) *value++ = '\0';
        if (name && value) {
            codec_config((void*)_nc, (struct NCodecConfigItem){
                                         .name = (const char*)trim(name),
                                         .value = (const char*)trim(value),
                                     });
        }
    }
    free(_buf);
    _buf = NULL;

    /* Guard conditions for this codec. */
    if (_nc->interface == NULL || strcmp(_nc->interface, "diagnostic")) {
        goto create_fail;
    }
    if (_nc->type == NULL) {
        goto create_fail;
    }
    if (_nc->schema == NULL || strcmp(_nc->schema, "fbs")) {
        goto create_fail;
    }

    /* Determine which codec implementation to use. */
    _nc->c.codec = (struct NCodecVTable){
        .config = codec_config,
        .stat = codec_stat,
        .flush = codec_flush,
        .truncate = codec_truncate,
        .close = codec_close,
    };
    if (strcmp(_nc->type, "uds") == 0) {
        _nc->c.codec.write = uds_write;
        _nc->c.codec.read = uds_read;
    } else if (strcmp(_nc->type, "tp") == 0) {
        _nc->c.codec.write = tp_write;
        _nc->c.codec.read = tp_read;
    } else if (strcmp(_nc->type, "srpc") == 0) {
        _nc->c.codec.write = srpc_write;
        _nc->c.codec.read = srpc_read;
    } else if (strcmp(_nc->type, "xil") == 0) {
        _nc->c.codec.write = xil_write;
        _nc->c.codec.read = xil_read;
    } else {
        goto create_fail;
    }

    /* Complete the setup of this codec instance. */
    flatcc_builder_init(&_nc->fbs_builder);
    _nc->fbs_builder.buffer_flags |= flatcc_builder_with_size;
    _nc->fbs_builder_initalized = true;

    return (void*)_nc;

create_fail:
    if (_buf) free(_buf);
    if (_nc) {
        free_codec(_nc);
        free(_nc);
    }
    return NULL;
}
