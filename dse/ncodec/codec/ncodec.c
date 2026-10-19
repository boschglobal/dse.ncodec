// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dse/ncodec/codec.h>
#include <dse/ncodec/codec/ab/codec.h>
#include <dse/ncodec/codec/diag/codec.h>


#define UNUSED(x) ((void)x)

typedef NCODEC* (*NCodecCreateFunc)(const char* mime_type);



/* Dynamic Hooking support. */
static NCODEC* _ncodec_create(const char* mime_type)
{
    if (mime_type == NULL) return NULL;

    NCODEC*     nc = NULL;
    const char* interface = NULL;
    char*       _buf = strdup(mime_type);
    char*       _pos = NULL;

    /* Route on the "interface" parameter (last value, as codec_config). */
    char* _param = strtok_r(_buf, "; ", &_pos); /* Codec name. */
    while (_param && (_param = strtok_r(NULL, "; ", &_pos)) != NULL) {
        char* value = strchr(_param, '=');
        if (value == NULL) continue;
        *value++ = '\0';
        if (strcmp(_param, "interface") == 0) interface = value;
    }
    if (interface && strcmp(interface, "stream") == 0) {
        /* AB Codec. */
        nc = ab_codec_create(mime_type);
    } else if (interface && strcmp(interface, "diagnostic") == 0) {
        /* Diagnostic Codec. */
        nc = diag_codec_create(mime_type);
    }

    free(_buf);
    return nc;
}


static NCodecCreateFunc  active_ncodec_create = _ncodec_create;
static void* ncodec_handle = NULL;


__attribute__((destructor)) static void _ncodec_unload(void)
{
    if (ncodec_handle != NULL) {
        dlclose(ncodec_handle);
        ncodec_handle = NULL;
    }
}



/* NCodec API methods. */

NCODEC* ncodec_create(const char* mime_type)
{
    return active_ncodec_create(mime_type);
}


int32_t ncodec_load(const char* filename, const char* hint)
{
    UNUSED(hint);
    errno = 0;

    /* Restrict: one load only. */
    if (ncodec_handle != NULL) {
        errno = EBUSY;
        return -1;
    }
    /* Restrict: only load from DLL (no self-ref for load). */
    if (filename == NULL || filename[0] == '\0') {
        errno = EINVAL;
        return -1;
    }

    /* Load ncodec_create() from DLL. */
    void* handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL) {
        int load_errno = errno;
        printf("Error opening library: %s\n", dlerror());
        errno = load_errno ? load_errno : ENOENT;
        return -1;
    }
    NCodecCreate* create_func = dlsym(handle, "ncodec_create");
    if (create_func == NULL) {
        printf("Error library: %s\n", dlerror());
        dlclose(handle);
        errno = ENOSYS;
        return -1;
    } else if (create_func == ncodec_create) {
        // Self load, _don't_ set active_ncodec_create (cyclic), consume.
        dlclose(handle);
        errno = 0;
        return 0;
    }

    ncodec_handle = handle;
    active_ncodec_create = create_func;
    errno = 0;
    return 0;
}
