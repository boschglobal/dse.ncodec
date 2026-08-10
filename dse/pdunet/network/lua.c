// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dse/log.h>
#include <dse/pdunet/network/network.h>


typedef struct ctx_t {
    double   phys;
    uint64_t raw;
    uint8_t* payload;
    uint32_t payload_len;

    /* Lua function may return these table fields to indicate an error. */
    int         err;
    const char* errmsg;
} ctx_t;


/* Payload metatable. */

typedef struct {
    uint8_t* data;
    uint32_t len;
} payload_wrapper_t;

#define PAYLOAD_METATABLE "pdu_payload_array"

static int _payload_index(lua_State* L)
{
    payload_wrapper_t* pw =
        (payload_wrapper_t*)luaL_checkudata(L, 1, PAYLOAD_METATABLE);
    lua_Integer idx = luaL_checkinteger(L, 2);

    if (idx < 1 || idx > pw->len) {
        return luaL_error(L,
            "payload index " LUA_INTEGER_FMT " out of bounds (1..%u)", idx,
            (unsigned)pw->len);
    }

    lua_pushinteger(L, pw->data[idx - 1]);
    return 1;
}

static int _payload_newindex(lua_State* L)
{
    payload_wrapper_t* pw =
        (payload_wrapper_t*)luaL_checkudata(L, 1, PAYLOAD_METATABLE);
    lua_Integer idx = luaL_checkinteger(L, 2);
    lua_Integer value = luaL_checkinteger(L, 3);

    if (idx < 1 || idx > pw->len) {
        return luaL_error(
            L, "payload index %d out of bounds (1..%d)", idx, pw->len);
    }
    if (value < 0 || value > 255) {
        return luaL_error(L, "payload value %d out of range (0..255)", value);
    }

    pw->data[idx - 1] = (uint8_t)value;
    return 0;
}

static int _payload_len(lua_State* L)
{
    payload_wrapper_t* pw =
        (payload_wrapper_t*)luaL_checkudata(L, 1, PAYLOAD_METATABLE);
    lua_pushinteger(L, pw->len);
    return 1;
}


static void _create_payload_metatable(lua_State* L)
{
    if (luaL_newmetatable(L, PAYLOAD_METATABLE)) {
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, _payload_index);
        lua_settable(L, -3);

        lua_pushstring(L, "__newindex");
        lua_pushcfunction(L, _payload_newindex);
        lua_settable(L, -3);

        lua_pushstring(L, "__len");
        lua_pushcfunction(L, _payload_len);
        lua_settable(L, -3);
    }
    lua_pop(L, 1);
}


static void _push_payload_userdata(lua_State* L, uint8_t* data, uint32_t len)
{
    payload_wrapper_t* pw =
        (payload_wrapper_t*)lua_newuserdata(L, sizeof(payload_wrapper_t));
    pw->data = data;
    pw->len = len;

    luaL_getmetatable(L, PAYLOAD_METATABLE);
    lua_setmetatable(L, -2);
}


static void _lua_push_pdu_ctx(
    lua_State* L, uint8_t* payload, uint32_t payload_len)
{
    // clang-format off
    lua_newtable(L);                                        // [table]
    lua_pushstring(L, "payload");                           // [table][payload]
    _push_payload_userdata(L, payload, payload_len);        // [table][payload][metatable]
    lua_settable(L, -3);                                    // [table]
    // clang-format on
}

static void lua_model_error(PduNetwork* net, lua_State* L, const char* msg)
{
    if (net->log->level != LOG_QUIET)
        log_error(net->log, "Lua Error: %s (%s)", msg, lua_tostring(L, -1));
}


int pdunet_lua_pdu_call(PduNetwork* net, lua_State* L, int32_t func_ref,
    uint8_t* const payload, uint32_t payload_len, bool no_err_log)
{
    if (L == NULL) return -EINVAL;
    if (payload == NULL) return -EINVAL;

    lua_rawgeti(L, LUA_REGISTRYINDEX, func_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return -EINVAL;
    }
    _lua_push_pdu_ctx(L, payload, payload_len);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        lua_model_error(net, L, "lua_pcall() failed");
        lua_pop(L, 1);
        return -1;
    }
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    int idx = lua_gettop(L);
    // Check the ctx table for an err.
    int err = 0;
    lua_pushstring(L, "err");
    lua_gettable(L, idx);
    if (lua_isinteger(L, -1)) {
        err = (int)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
    if (err) {
        lua_pushstring(L, "errmsg");
        lua_gettable(L, idx);
        if (lua_isstring(L, -1)) {
            const char* msg = lua_tostring(L, -1);
            if (msg) {
                if (no_err_log) {
                    log_trace(
                        net->log, "lua call returned error: %s (%d)", msg, err);
                } else {
                    if (net->log->level != LOG_QUIET) {
                        log_error(net->log, "lua call returned error: %s (%d)",
                            msg, err);
                    }
                }
            }
        }
        lua_pop(L, 2);
        return -1;
    }

    lua_pop(L, 1);
    return 0;
}

static void _lua_push_signal_ctx(lua_State* L, double phys, uint64_t raw,
    uint8_t* payload, uint32_t payload_len)
{
    // clang-format off
    lua_newtable(L);                                        // [table]
    lua_pushstring(L, "phys");                              // [table][phys]
    lua_pushnumber(L, phys);                                // [table][phys][double]
    lua_settable(L, -3);                                    // [table]
    lua_pushstring(L, "raw");                               // [table][raw]
    lua_pushinteger(L, (lua_Integer)raw);                   // [table][raw][uint64]
    lua_settable(L, -3);                                    // [table]
    lua_pushstring(L, "payload");                           // [table][payload]
    _push_payload_userdata(L, payload, payload_len);        // [table][payload][metatable]
    lua_settable(L, -3);                                    // [table]
    // clang-format on
}

int pdunet_lua_signal_call(PduNetwork* net, lua_State* L, int32_t func_ref,
    double* phys, uint64_t* raw, uint8_t* payload, uint32_t payload_len)
{
    if (L == NULL) return -EINVAL;
    if (phys == NULL) return -EINVAL;
    if (raw == NULL) return -EINVAL;
    if (payload == NULL) payload_len = 0;

    lua_rawgeti(L, LUA_REGISTRYINDEX, func_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return -EINVAL;
    }
    _lua_push_signal_ctx(L, *phys, *raw, payload, payload_len);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        lua_model_error(net, L, "lua_pcall() failed");
        lua_pop(L, 1);
        return -1;
    }
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    int idx = lua_gettop(L);

    // Check the ctx table for an err.
    int err = 0;
    lua_pushstring(L, "err");
    lua_gettable(L, idx);
    if (lua_isinteger(L, -1)) {
        err = (int)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
    if (err) {
        lua_pushstring(L, "errmsg");
        lua_gettable(L, idx);
        if (lua_isstring(L, -1)) {
            const char* msg = lua_tostring(L, -1);
            if (msg) {
                if (net->log->level != LOG_QUIET)

                    log_error(
                        net->log, "lua call returned error: %s (%d)", msg, err);
            }
        }
        lua_pop(L, 2);
        return -1;
    }

    // Check if values have changed.
    lua_pushstring(L, "raw");
    lua_gettable(L, idx);
    if (lua_isinteger(L, -1)) {
        *raw = (uint64_t)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
    lua_pushstring(L, "phys");
    lua_gettable(L, idx);
    if (lua_isnumber(L, -1)) {
        *phys = lua_tonumber(L, -1);
    }
    lua_pop(L, 2);
    return 0;
}


void pdunet_load_lua_func(YamlNode* n, const char* path, const char** out)
{
    *out = NULL;
    YamlNode* np = dse_yaml_find_node(n, path);
    if (np) {
        dse_yaml_get_string(np, "lua", out);
    }
}


void pdunet_parse_network_functions(PduNetwork* net)
{
    assert(net);

    // Parse spec/functions.
    YamlNode* n = dse_yaml_find_node(net->doc, "spec/functions");
    if (n != NULL) {
        pdunet_load_lua_func(n, "global", &net->lua.global);
    }
}


static lua_State* __lua_model_create(lua_State* L)
{
    if (L == NULL) {
        L = luaL_newstate();
        luaL_openlibs(L);

#ifdef _WIN32
        lua_getglobal(L, "package");
        if (lua_istable(L, -1)) {
            const char* path = NULL;
            lua_getfield(L, -1, "path");
            path = lua_tostring(L, -1);
            lua_pop(L, 1);

            lua_pushfstring(L,
                "./share/lua/5.4/?.lua;./share/lua/5.4/?/init.lua;%s",
                path ? path : "");
            lua_setfield(L, -2, "path");
        }
        lua_pop(L, 1);
#endif
    } else {
        luaL_openlibs(L);
    }

    return L;
}


static void __lua_model_destroy(lua_State* L)
{
    lua_close(L);
}


int pdunet_lua_install_script(
    PduNetwork* net, lua_State* L, const char* lua_script)
{
    if (lua_script == NULL) return 0;
    assert(L);

    /* Run the Lua script. */
    int top = lua_gettop(L);
    if (lua_script != NULL && luaL_dostring(L, lua_script) != 0) {
        log_error(net->log, "Lua Error: luaL_dostring() failed (%s)",
            lua_tostring(L, -1));
        lua_settop(L, top);
        return 0;
    }

    /* If the script returned a function, store and return a reference. */
    if (lua_isfunction(L, -1)) {
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        if (ref < 0) {
            log_error(net->log,
                "Failed to create reference for returned Lua function");
            lua_settop(L, top);
            return 0;
        }
        log_notice(net->log, "Lua: anonymous function installed: (ref=%d)\n%s",
            ref, lua_script);
        lua_settop(L, top);
        return ref;
    }

    lua_settop(L, top);
    log_notice(net->log, "Lua: script installed: \n%s", lua_script);
    return 0;
}


int pdunet_lua_setup(PduNetwork* net)
{
    assert(net);

    /* Establish the Lua interpreter. */
    net->lua.lua_state = __lua_model_create(net->lua.lua_state);
    assert(net->lua.lua_state);

    /* Setup Lua interpreter for PDU Net. */
    lua_State* L = net->lua.lua_state;
    _create_payload_metatable(L);

    return 0;
}


void pdunet_lua_teardown(PduNetwork* net)
{
    if (net && net->lua.lua_state != NULL) {
        __lua_model_destroy(net->lua.lua_state);
        net->lua.lua_state = NULL;
    }
}
