// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>


typedef struct CodecHooks {
    NCODEC* instance;
    NCodecRead read;
    NCodecWrite write;
    NCodecClose close;
} CodecHooks;

static CodecHooks codec_hooks = {0};


static int32_t custom_read(NCODEC* nc, NCodecMessage* msg)
{
    if (nc == NULL || codec_hooks.instance != nc) return -EINVAL;

    printf("Custom DLL: read\n");
    return codec_hooks.read(nc, msg);
}


static int32_t custom_write(NCODEC* nc, NCodecMessage* msg)
{
    if (nc == NULL || codec_hooks.instance != nc) return -EINVAL;

    printf("Custom DLL: write\n");
    return codec_hooks.write(nc, msg);
}


static void custom_close(NCODEC* nc)
{
    if (nc == NULL || codec_hooks.instance != nc) return;

    NCodecClose original_close = codec_hooks.close;
    codec_hooks = (CodecHooks){0};
    original_close(nc);
}


DLL_PUBLIC NCODEC* ncodec_create(const char* mime_type)
{
    if (codec_hooks.instance != NULL) {
        errno = EBUSY;
        return NULL;
    }

    NCODEC* nc = ab_codec_create(mime_type);
    if (nc == NULL) return NULL;

    NCodecInstance* instance = (NCodecInstance*)nc;
    codec_hooks = (CodecHooks){
        .instance = nc,
        .read = instance->codec.read,
        .write = instance->codec.write,
        .close = instance->codec.close,
    };

    instance->codec.read = custom_read;
    instance->codec.write = custom_write;
    instance->codec.close = custom_close;
    return nc;
}
