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


/* Payload implementations.
 *
 * PDU callbacks use a table-backed payload mirror. This makes Lua loops such as
 * checksum calculations fast because payload[i] is a normal Lua table access.
 *
 * Signal callbacks use a cached userdata payload. Signal payload access is
 * expected to be infrequent, so avoid copying the full payload into/out of a
 * Lua table on every signal call.
 */

#define PAYLOAD_TABLE_METATABLE    "pdu_payload_table"
#define PAYLOAD_USERDATA_METATABLE "pdu_payload_array"

static char _payload_table_len_key;


typedef struct {
    uint8_t* data;
    uint32_t len;
} payload_wrapper_t;


/* Table-backed payload implementation, used by PDU callbacks. */

static uint32_t _payload_table_get_len(lua_State* L, int idx)
{
    uint32_t len = 0;

    idx = lua_absindex(L, idx);
    lua_rawgetp(L, idx, &_payload_table_len_key);
    if (lua_isinteger(L, -1)) {
        lua_Integer value = lua_tointeger(L, -1);
        if (value > 0) len = (uint32_t)value;
    }
    lua_pop(L, 1);

    return len;
}


static void _payload_table_set_len(lua_State* L, int idx, uint32_t len)
{
    idx = lua_absindex(L, idx);
    lua_pushinteger(L, (lua_Integer)len);
    lua_rawsetp(L, idx, &_payload_table_len_key);
}


static int _payload_table_index(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    if (lua_type(L, 2) == LUA_TSTRING) {
        lua_pushnil(L);
        return 1;
    }

    lua_Integer idx = luaL_checkinteger(L, 2);
    uint32_t    len = _payload_table_get_len(L, 1);

    if (idx < 1 || idx > len) {
        return luaL_error(L,
            "payload index " LUA_INTEGER_FMT " out of bounds (1..%u)", idx,
            (unsigned)len);
    }

    /*
     * Valid payload entries should already be present in the table. If Lua
     * deleted one, return nil here; copy-back validation will fail later if
     * the callback leaves the payload in that state.
     */
    lua_pushnil(L);
    return 1;
}


static int _payload_table_newindex(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_Integer idx = luaL_checkinteger(L, 2);
    lua_Integer value = luaL_checkinteger(L, 3);
    uint32_t    len = _payload_table_get_len(L, 1);

    if (idx < 1 || idx > len) {
        return luaL_error(L,
            "payload index " LUA_INTEGER_FMT " out of bounds (1..%u)", idx,
            (unsigned)len);
    }
    if (value < 0 || value > 255) {
        return luaL_error(L,
            "payload value " LUA_INTEGER_FMT " out of range (0..255)", value);
    }

    lua_pushvalue(L, 3);
    lua_rawseti(L, 1, idx);
    return 0;
}


static int _payload_table_len(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushinteger(L, (lua_Integer)_payload_table_get_len(L, 1));
    return 1;
}


static void _create_payload_table_metatable(lua_State* L)
{
    if (luaL_newmetatable(L, PAYLOAD_TABLE_METATABLE)) {
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, _payload_table_index);
        lua_settable(L, -3);

        lua_pushstring(L, "__newindex");
        lua_pushcfunction(L, _payload_table_newindex);
        lua_settable(L, -3);

        lua_pushstring(L, "__len");
        lua_pushcfunction(L, _payload_table_len);
        lua_settable(L, -3);
    }
    lua_pop(L, 1);
}


static void _push_payload_table(lua_State* L)
{
    lua_createtable(L, 0, 0);
    luaL_getmetatable(L, PAYLOAD_TABLE_METATABLE);
    lua_setmetatable(L, -2);
}


static void _push_cached_payload_table(
    lua_State* L, void* registry_key, uint8_t* data, uint32_t len)
{
    if (data == NULL) len = 0;

    lua_rawgetp(L, LUA_REGISTRYINDEX, registry_key);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        _push_payload_table(L);
        lua_pushvalue(L, -1);
        lua_rawsetp(L, LUA_REGISTRYINDEX, registry_key);
    }

    int      idx = lua_absindex(L, -1);
    uint32_t old_len = _payload_table_get_len(L, idx);

    /*
     * Clear stale numeric entries when the payload shrinks so out-of-bounds
     * reads hit __index and raise an error instead of seeing old data.
     */
    for (uint32_t i = len + 1; i <= old_len; i++) {
        lua_pushnil(L);
        lua_rawseti(L, idx, i);
    }

    _payload_table_set_len(L, idx, len);

    /*
     * Mirror the C payload into a Lua table. Lua checksum loops can then use
     * fast raw table reads instead of calling a C __index metamethod per byte.
     */
    for (uint32_t i = 0; i < len; i++) {
        lua_pushinteger(L, data[i]);
        lua_rawseti(L, idx, i + 1);
    }
}


/*
 * Existing numeric payload entries are plain Lua table entries, so writes to
 * them do not invoke __newindex. Validate type/range during copy-back.
 */
static int _copy_cached_payload_table_to_buffer(PduNetwork* net, lua_State* L,
    void* registry_key, uint8_t* data, uint32_t len)
{
    if (data == NULL || len == 0) return 0;

    int top = lua_gettop(L);

    lua_rawgetp(L, LUA_REGISTRYINDEX, registry_key);
    if (!lua_istable(L, -1)) {
        if (net->log->level != LOG_QUIET) {
            log_error(net->log, "Lua Error: cached payload is not a table");
        }
        lua_settop(L, top);
        return -1;
    }

    int idx = lua_absindex(L, -1);

    for (uint32_t i = 0; i < len; i++) {
        lua_rawgeti(L, idx, i + 1);
        if (!lua_isinteger(L, -1)) {
            if (net->log->level != LOG_QUIET) {
                log_error(net->log, "Lua Error: payload[%u] must be an integer",
                    i + 1);
            }
            lua_settop(L, top);
            return -1;
        }

        lua_Integer value = lua_tointeger(L, -1);
        if (value < 0 || value > 255) {
            if (net->log->level != LOG_QUIET) {
                log_error(net->log,
                    "Lua Error: payload[%u] value " LUA_INTEGER_FMT
                    " out of range (0..255)",
                    i + 1, value);
            }
            lua_settop(L, top);
            return -1;
        }

        data[i] = (uint8_t)value;
        lua_pop(L, 1);
    }

    lua_settop(L, top);
    return 0;
}


/* Userdata-backed payload implementation, used by signal callbacks. */

static int _payload_userdata_index(lua_State* L)
{
    payload_wrapper_t* pw =
        (payload_wrapper_t*)luaL_checkudata(L, 1, PAYLOAD_USERDATA_METATABLE);
    lua_Integer idx = luaL_checkinteger(L, 2);

    if (idx < 1 || idx > pw->len) {
        return luaL_error(L,
            "payload index " LUA_INTEGER_FMT " out of bounds (1..%u)", idx,
            (unsigned)pw->len);
    }

    lua_pushinteger(L, pw->data[idx - 1]);
    return 1;
}


static int _payload_userdata_newindex(lua_State* L)
{
    payload_wrapper_t* pw =
        (payload_wrapper_t*)luaL_checkudata(L, 1, PAYLOAD_USERDATA_METATABLE);
    lua_Integer idx = luaL_checkinteger(L, 2);
    lua_Integer value = luaL_checkinteger(L, 3);

    if (idx < 1 || idx > pw->len) {
        return luaL_error(L,
            "payload index " LUA_INTEGER_FMT " out of bounds (1..%u)", idx,
            (unsigned)pw->len);
    }
    if (value < 0 || value > 255) {
        return luaL_error(L,
            "payload value " LUA_INTEGER_FMT " out of range (0..255)", value);
    }

    pw->data[idx - 1] = (uint8_t)value;
    return 0;
}


static int _payload_userdata_len(lua_State* L)
{
    payload_wrapper_t* pw =
        (payload_wrapper_t*)luaL_checkudata(L, 1, PAYLOAD_USERDATA_METATABLE);
    lua_pushinteger(L, pw->len);
    return 1;
}


static void _create_payload_userdata_metatable(lua_State* L)
{
    if (luaL_newmetatable(L, PAYLOAD_USERDATA_METATABLE)) {
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, _payload_userdata_index);
        lua_settable(L, -3);

        lua_pushstring(L, "__newindex");
        lua_pushcfunction(L, _payload_userdata_newindex);
        lua_settable(L, -3);

        lua_pushstring(L, "__len");
        lua_pushcfunction(L, _payload_userdata_len);
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

    luaL_getmetatable(L, PAYLOAD_USERDATA_METATABLE);
    lua_setmetatable(L, -2);
}


static void _push_cached_payload_userdata(
    lua_State* L, void* registry_key, uint8_t* data, uint32_t len)
{
    if (data == NULL) len = 0;

    lua_rawgetp(L, LUA_REGISTRYINDEX, registry_key);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        _push_payload_userdata(L, data, len);
        lua_pushvalue(L, -1);
        lua_rawsetp(L, LUA_REGISTRYINDEX, registry_key);
        return;
    }

    payload_wrapper_t* pw =
        (payload_wrapper_t*)luaL_checkudata(L, -1, PAYLOAD_USERDATA_METATABLE);
    pw->data = data;
    pw->len = len;
}


/* Registry keys for cached per-lua_State objects.
 *
 * These caches reduce table/userdata allocation and Lua GC pressure in the hot
 * PDU/signal callback paths. This is safe as long as Lua scripts do not retain
 * ctx or ctx.payload beyond the duration of the call.
 */
static char _pdu_ctx_registry_key;
static char _pdu_payload_registry_key;
static char _signal_ctx_registry_key;
static char _signal_payload_registry_key;


/*
 * Reset only the supported result fields before reusing a cached ctx table.
 *
 * The input fields are overwritten by _lua_push_pdu_ctx() /
 * _lua_push_signal_ctx(). Unsupported/custom Lua fields are not part of the
 * API contract, so we avoid a full table scan here.
 */
static void _clear_ctx_result_fields(lua_State* L, int idx)
{
    idx = lua_absindex(L, idx);

    lua_pushliteral(L, "err");
    lua_pushnil(L);
    lua_rawset(L, idx);

    lua_pushliteral(L, "errmsg");
    lua_pushnil(L);
    lua_rawset(L, idx);
}


static void _lua_push_pdu_ctx(
    lua_State* L, uint8_t* payload, uint32_t payload_len)
{
    lua_rawgetp(L, LUA_REGISTRYINDEX, &_pdu_ctx_registry_key);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_createtable(L, 0, 1);
        lua_pushvalue(L, -1);
        lua_rawsetp(L, LUA_REGISTRYINDEX, &_pdu_ctx_registry_key);
    }

    int idx = lua_absindex(L, -1);
    _clear_ctx_result_fields(L, idx);

    lua_pushliteral(L, "payload");
    _push_cached_payload_table(
        L, &_pdu_payload_registry_key, payload, payload_len);
    lua_rawset(L, idx);
}


static void _lua_push_signal_ctx(lua_State* L, double phys, uint64_t raw,
    uint8_t* payload, uint32_t payload_len)
{
    lua_rawgetp(L, LUA_REGISTRYINDEX, &_signal_ctx_registry_key);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_createtable(L, 0, 3);
        lua_pushvalue(L, -1);
        lua_rawsetp(L, LUA_REGISTRYINDEX, &_signal_ctx_registry_key);
    }

    int idx = lua_absindex(L, -1);
    _clear_ctx_result_fields(L, idx);

    lua_pushliteral(L, "phys");
    lua_pushnumber(L, phys);
    lua_rawset(L, idx);

    lua_pushliteral(L, "raw");
    lua_pushinteger(L, (lua_Integer)raw);
    lua_rawset(L, idx);

    lua_pushliteral(L, "payload");
    _push_cached_payload_userdata(
        L, &_signal_payload_registry_key, payload, payload_len);
    lua_rawset(L, idx);
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

    int top = lua_gettop(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, func_ref);
    if (!lua_isfunction(L, -1)) {
        lua_settop(L, top);
        return -EINVAL;
    }

    _lua_push_pdu_ctx(L, payload, payload_len);

    /*
     * Keep ctx on the stack across lua_pcall(). The function receives the
     * duplicated ctx argument, while C keeps the original stack reference for
     * post-call result inspection. Lua return values are intentionally ignored.
     *
     * Before: ... func ctx
     * After:  ... ctx func ctx
     */
    lua_pushvalue(L, -1);
    lua_insert(L, -3);

    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        lua_model_error(net, L, "lua_pcall() failed");
        lua_settop(L, top);
        return -1;
    }

    int idx = lua_gettop(L);

    if (_copy_cached_payload_table_to_buffer(
            net, L, &_pdu_payload_registry_key, payload, payload_len) != 0) {
        lua_settop(L, top);
        return -1;
    }

    // Check the ctx table for an err.
    int err = 0;
    lua_pushliteral(L, "err");
    lua_rawget(L, idx);
    if (lua_isinteger(L, -1)) {
        err = (int)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    if (err) {
        lua_pushliteral(L, "errmsg");
        lua_rawget(L, idx);
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
        lua_settop(L, top);
        return -1;
    }

    lua_settop(L, top);
    return 0;
}


int pdunet_lua_signal_call(PduNetwork* net, lua_State* L, int32_t func_ref,
    double* phys, uint64_t* raw, uint8_t* payload, uint32_t payload_len)
{
    if (L == NULL) return -EINVAL;
    if (phys == NULL) return -EINVAL;
    if (raw == NULL) return -EINVAL;
    if (payload == NULL) payload_len = 0;

    int top = lua_gettop(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, func_ref);
    if (!lua_isfunction(L, -1)) {
        lua_settop(L, top);
        return -EINVAL;
    }

    _lua_push_signal_ctx(L, *phys, *raw, payload, payload_len);

    /*
     * Keep ctx on the stack across lua_pcall(). The function receives the
     * duplicated ctx argument, while C keeps the original stack reference for
     * post-call result inspection. Lua return values are intentionally ignored.
     *
     * Before: ... func ctx
     * After:  ... ctx func ctx
     */
    lua_pushvalue(L, -1);
    lua_insert(L, -3);

    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        lua_model_error(net, L, "lua_pcall() failed");
        lua_settop(L, top);
        return -1;
    }

    int idx = lua_gettop(L);

    // Check the ctx table for an err.
    int err = 0;
    lua_pushliteral(L, "err");
    lua_rawget(L, idx);
    if (lua_isinteger(L, -1)) {
        err = (int)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    if (err) {
        lua_pushliteral(L, "errmsg");
        lua_rawget(L, idx);
        if (lua_isstring(L, -1)) {
            const char* msg = lua_tostring(L, -1);
            if (msg) {
                if (net->log->level != LOG_QUIET)

                    log_error(
                        net->log, "lua call returned error: %s (%d)", msg, err);
            }
        }
        lua_settop(L, top);
        return -1;
    }

    // Check if values have changed.
    lua_pushliteral(L, "raw");
    lua_rawget(L, idx);
    if (lua_isinteger(L, -1)) {
        *raw = (uint64_t)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_pushliteral(L, "phys");
    lua_rawget(L, idx);
    if (lua_isnumber(L, -1)) {
        *phys = lua_tonumber(L, -1);
    }

    lua_settop(L, top);
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
    if (net->lua.lua_state == NULL) net->lua.owner = true;
    net->lua.lua_state = __lua_model_create(net->lua.lua_state);
    assert(net->lua.lua_state);

    /* Setup Lua interpreter for PDU Net. */
    lua_State* L = net->lua.lua_state;
    _create_payload_table_metatable(L);
    _create_payload_userdata_metatable(L);

    return 0;
}


void pdunet_lua_teardown(PduNetwork* net)
{
    if (net && net->lua.owner && net->lua.lua_state != NULL) {
        __lua_model_destroy(net->lua.lua_state);
    }
    net->lua.lua_state = NULL;
}
