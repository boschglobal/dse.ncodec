// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>


#define UNUSED(x) ((void)x)
#define CODEC     "application/x-automotive-bus"


/* interface=stream; type=frame; bus=can; schema=fbs */
extern int32_t can_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t can_read(NCODEC* nc, NCodecMessage* msg);
extern int32_t can_flush(NCODEC* nc);
extern int32_t can_truncate(NCODEC* nc);

/* interface=stream; type=pdu; schema=fbs */
extern int32_t pdu_write(NCODEC* nc, NCodecMessage* msg);
extern int32_t pdu_read(NCODEC* nc, NCodecMessage* msg);
extern int32_t pdu_flush(NCODEC* nc);
extern int32_t pdu_truncate(NCODEC* nc);

extern void flexray_bus_model_create(ABCodecInstance* nc);
extern void flexray_pop_bus_model_create(ABCodecInstance* nc);


char* trim(char* s)
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

void clear_free_list(ABCodecInstance* _nc)
{
    vector_clear(&_nc->free_list, __free_item, NULL);
}

void destroy_free_list(ABCodecInstance* _nc)
{
    vector_clear(&_nc->free_list, __free_item, NULL);
    vector_reset(&_nc->free_list);
}

void free_codec(ABCodecInstance* _nc)
{
    if (_nc == NULL) return;

    if (_nc->interface) free(_nc->interface);
    if (_nc->type) free(_nc->type);
    if (_nc->bus) free(_nc->bus);
    if (_nc->schema) free(_nc->schema);
    if (_nc->bus_id_str) free(_nc->bus_id_str);
    if (_nc->node_id_str) free(_nc->node_id_str);
    if (_nc->interface_id_str) free(_nc->interface_id_str);
    if (_nc->swc_id_str) free(_nc->swc_id_str);
    if (_nc->ecu_id_str) free(_nc->ecu_id_str);
    if (_nc->cc_id_str) free(_nc->cc_id_str);
    if (_nc->name) free(_nc->name);
    if (_nc->model) free(_nc->model);
    if (_nc->mode) free(_nc->mode);
    if (_nc->pwr) free(_nc->pwr);
    if (_nc->vcn_count_str) free(_nc->vcn_count_str);
    if (_nc->poc_state_cha_str) free(_nc->poc_state_cha_str);
    if (_nc->poc_state_chb_str) free(_nc->poc_state_chb_str);

    if (_nc->fbs_builder_initalized) flatcc_builder_clear(&_nc->fbs_builder);

    /* The Bus Model NCodec object is a shallow copy, only free the
    specifically allocated resources. */
    if (_nc->reader.bus_model.nc != NULL) {
        destroy_free_list(_nc->reader.bus_model.nc);
        if (_nc->reader.bus_model.nc->fbs_builder_initalized) {
            flatcc_builder_clear(&_nc->reader.bus_model.nc->fbs_builder);
        }
        _nc->reader.bus_model.nc->c.stream->close(
            (NCODEC*)_nc->reader.bus_model.nc);
        free(_nc->reader.bus_model.nc);
    }
    if (_nc->reader.bus_model.model != NULL) {
        if (_nc->reader.bus_model.vtable.close) {
            _nc->reader.bus_model.vtable.close(&_nc->reader.bus_model);
        }
        free(_nc->reader.bus_model.model);
    }
    destroy_free_list(_nc);
}

void create_bus_model(ABCodecInstance* nc)
{
    if (strcmp(nc->type, "pdu") == 0) {
        if (nc->model && strcmp(nc->model, "flexray") == 0) {
            if (nc->mode) {
                if (strcmp(nc->mode, "pop") == 0) {
                    flexray_pop_bus_model_create(nc);
                } else {
                    log_fatal(
                        nc, "Unknown FlexRay bus model mode: %s", nc->mode);
                }
            } else {
                flexray_bus_model_create(nc);
            }
        }
    }
}


int32_t codec_config(NCODEC* nc, NCodecConfigItem item)
{
    ABCodecInstance* _nc = (ABCodecInstance*)nc;
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
    if (strcmp(item.name, "bus") == 0) {
        if (_nc->bus) free(_nc->bus);
        _nc->bus = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "schema") == 0) {
        if (_nc->schema) free(_nc->schema);
        _nc->schema = strdup(item.value);
        return 0;
    }

    /* Parameters. */
    if (strcmp(item.name, "bus_id") == 0) {
        if (_nc->bus_id_str) free(_nc->bus_id_str);
        _nc->bus_id_str = strdup(item.value);
        _nc->bus_id = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "node_id") == 0) {
        if (_nc->node_id_str) free(_nc->node_id_str);
        _nc->node_id_str = strdup(item.value);
        _nc->node_id = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "interface_id") == 0) {
        if (_nc->interface_id_str) free(_nc->interface_id_str);
        _nc->interface_id_str = strdup(item.value);
        _nc->interface_id = strtoul(item.value, NULL, 10);
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
    if (strcmp(item.name, "cc_id") == 0) {
        if (_nc->cc_id_str) free(_nc->cc_id_str);
        _nc->cc_id_str = strdup(item.value);
        _nc->cc_id = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "name") == 0) {
        if (_nc->name) free(_nc->name);
        _nc->name = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "model") == 0) {
        if (_nc->model) free(_nc->model);
        _nc->model = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "mode") == 0) {
        if (_nc->mode) free(_nc->mode);
        _nc->mode = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "pwr") == 0) {
        if (_nc->pwr) free(_nc->pwr);
        _nc->pwr = strdup(item.value);
        return 0;
    }
    if (strcmp(item.name, "vcn") == 0) {
        if (_nc->vcn_count_str) free(_nc->vcn_count_str);
        _nc->vcn_count_str = strdup(item.value);
        _nc->vcn_count = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "poca") == 0) {
        if (_nc->poc_state_cha_str) free(_nc->poc_state_cha_str);
        _nc->poc_state_cha_str = strdup(item.value);
        _nc->poc_state_cha = strtoul(item.value, NULL, 10);
        return 0;
    }
    if (strcmp(item.name, "pocb") == 0) {
        if (_nc->poc_state_chb_str) free(_nc->poc_state_chb_str);
        _nc->poc_state_chb_str = strdup(item.value);
        _nc->poc_state_chb = strtoul(item.value, NULL, 10);
        return 0;
    }

    return -EINVAL;
}


NCodecConfigItem codec_stat(NCODEC* nc, int32_t* index)
{
    ABCodecInstance* _nc = (ABCodecInstance*)nc;
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
        name = "bus";
        value = _nc->bus;
        break;
    case 3:
        name = "schema";
        value = _nc->schema;
        break;
    case 4:
        name = "bus_id";
        value = _nc->bus_id_str;
        break;
    case 5:
        name = "node_id";
        value = _nc->node_id_str;
        break;
    case 6:
        name = "interface_id";
        value = _nc->interface_id_str;
        break;
    case 7:
        name = "swc_id";
        value = _nc->swc_id_str;
        break;
    case 8:
        name = "ecu_id";
        value = _nc->ecu_id_str;
        break;
    case 9:
        name = "cc_id";
        value = _nc->cc_id_str;
        break;
    case 10:
        name = "name";
        value = _nc->name;
        break;
    case 11:
        name = "model";
        value = _nc->model;
        break;
    case 12:
        name = "mode";
        value = _nc->mode;
        break;
    case 13:
        name = "pwr";
        value = _nc->pwr;
        break;
    case 14:
        name = "vcn";
        value = _nc->vcn_count_str;
        break;
    case 15:
        name = "poca";
        value = _nc->poc_state_cha_str;
        break;
    case 16:
        name = "pocb";
        value = _nc->poc_state_chb_str;
        break;
    default:
        *index = -1;
    }

    return (struct NCodecConfigItem){
        .name = name,
        .value = value,
    };
}


void codec_close(NCODEC* nc)
{
    if (nc == NULL) return;

    free_codec((ABCodecInstance*)nc);
    free(nc);
}


NCODEC* ncodec_create(const char* mime_type)
{
    if (mime_type == NULL) return NULL;

    char*            _buf = strdup(mime_type);
    char*            _pos = NULL;
    ABCodecInstance* _nc = NULL;

    /* Check the MIMEtype is correct. */
    char* _codec_name = strtok_r(_buf, "; ", &_pos);
    if (strncmp(_codec_name, CODEC, strlen(CODEC)) != 0) {
        goto create_fail;
    }

    /* Allocate the codec object. */
    _nc = calloc(1, sizeof(ABCodecInstance));
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
    if (_nc->interface == NULL || strcmp(_nc->interface, "stream")) {
        goto create_fail;
    }
    if (_nc->type == NULL) {
        goto create_fail;
    } else {
        if (strcmp(_nc->type, "frame") == 0) {
            if (_nc->bus == NULL || strcmp(_nc->bus, "can")) {
                goto create_fail;
            }
        } else if (strcmp(_nc->type, "pdu") == 0) {
            // NOP
        } else {
            goto create_fail;
        }
    }
    if (_nc->schema == NULL || strcmp(_nc->schema, "fbs")) {
        goto create_fail;
    }

    /* Determine which codec implementation to use. */
    if (strcmp(_nc->type, "frame") == 0 && strcmp(_nc->bus, "can") == 0) {
        _nc->c.codec = (struct NCodecVTable){
            .config = codec_config,
            .stat = codec_stat,
            .write = can_write,
            .read = can_read,
            .flush = can_flush,
            .truncate = can_truncate,
            .close = codec_close,
        };
    } else if (strcmp(_nc->type, "pdu") == 0) {
        _nc->c.codec = (struct NCodecVTable){
            .config = codec_config,
            .stat = codec_stat,
            .write = pdu_write,
            .read = pdu_read,
            .flush = pdu_flush,
            .truncate = pdu_truncate,
            .close = codec_close,
        };
    } else {
        goto create_fail;
    }

    /* Complete the setup of this codec instance. */
    flatcc_builder_init(&_nc->fbs_builder);
    _nc->fbs_builder.buffer_flags |= flatcc_builder_with_size;
    _nc->fbs_stream_initalized = false;
    _nc->fbs_builder_initalized = true;

    /* Create any Bus Model. */
    create_bus_model(_nc);

    return (void*)_nc;

create_fail:
    if (_buf) free(_buf);
    if (_nc) {
        free_codec(_nc);
        free(_nc);
    }
    return NULL;
}
