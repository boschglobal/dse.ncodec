// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <yaml.h>
#include <dse/testing.h>
#include <dse/log.h>
#include <dse/clib/util/yaml.h>
#include <dse/pdunet/network/network.h>


#define UNUSED(x) ((void)x)


void* pdunet_schema_object_enumerator(PdunetSchemaObject* object,
    const char* path, uint32_t* index, PdunetSchemaObjectGenerator generator)
{
    assert(object);
    assert(object->doc);
    if (object == NULL) return NULL;
    if (object->doc == NULL) return NULL;

    YamlNode* doc = object->doc;
    YamlNode* node = dse_yaml_find_node(doc, path);
    if (node == NULL || node->node_type != YAML_SEQUENCE_NODE) return NULL;
    if (*index >= hashlist_length(&node->sequence)) return NULL;

    /* Generate the object, and return. */
    void* o = generator(hashlist_at(&node->sequence, *index));
    *index = *index + 1;
    return o; /* Caller to free. */
}


void pdunet_schema_load_object(PduNetworkDesc* net, void* node, void* object,
    const PdunetSchemaFieldSpec* spec, size_t count)
{
    YamlNode* n = (YamlNode*)node;
    uint8_t*  o = (uint8_t*)object;
    int       _;

    log_debug(net->log, "PdunetSchema load object:");

    for (size_t i = 0; i < count; i++) {
        const PdunetSchemaFieldSpec* s = &spec[i];
        switch (s->type) {
        case PdunetSchemaFieldTypeU8: {
            if (s->map) {
                const char* v = NULL;
                dse_yaml_get_string(n, s->path, &v);
                if (v == NULL) continue;
                for (const PdunetSchemaFieldMapSpec* m = s->map; m && m->key;
                    m++) {
                    if (strcmp(v, m->key) == 0) {
                        *(uint8_t*)(o + s->offset) = (uint8_t)m->val;
                        log_debug(net->log, "  load field: %s=%u (%s)", s->path,
                            (uint8_t)m->val, m->key);
                        break;
                    }
                }
                continue;
            } else {
                unsigned int _uint = *(uint8_t*)(o + s->offset);
                _ = dse_yaml_get_uint(n, s->path, &_uint);
                *(uint8_t*)(o + s->offset) = (uint8_t)_uint;
                if (!_)
                    log_debug(net->log, "  load field: %s=%u", s->path,
                        (uint8_t)_uint);
            }
            break;
        }
        case PdunetSchemaFieldTypeU16: {
            unsigned int _uint = *(uint16_t*)(o + s->offset);
            _ = dse_yaml_get_uint(n, s->path, &_uint);
            *(uint16_t*)(o + s->offset) = (uint16_t)_uint;
            if (!_)
                log_debug(
                    net->log, "  load field: %s=%u", s->path, (uint8_t)_uint);
            break;
        }
        case PdunetSchemaFieldTypeU32: {
            unsigned int _uint = *(uint32_t*)(o + s->offset);
            _ = dse_yaml_get_uint(n, s->path, &_uint);
            *(uint32_t*)(o + s->offset) = (uint32_t)_uint;
            if (!_)
                log_debug(
                    net->log, "  load field: %s=%u", s->path, (uint32_t)_uint);
            break;
        }
        case PdunetSchemaFieldTypeD: {
            _ = dse_yaml_get_double(n, s->path, (double*)(o + s->offset));
            if (!_)
                log_debug(net->log, "  load field: %s=%f", s->path,
                    *(double*)(o + s->offset));
            break;
        }
        case PdunetSchemaFieldTypeB: {
            _ = dse_yaml_get_bool(n, s->path, (bool*)(o + s->offset));
            if (!_)
                log_debug(net->log, "  load field: %s=%u", s->path,
                    *(bool*)(o + s->offset));
            break;
        }
        case PdunetSchemaFieldTypeS: {
            _ = dse_yaml_get_string(n, s->path, (const char**)(o + s->offset));
            if (!_)
                log_debug(net->log, "  load field: %s=%s", s->path,
                    *(const char**)(o + s->offset));
            break;
        }
        default:
            log_debug(net->log, "  Warning, unsupported field type (type=%u)",
                s->type);
        }
    }
}
