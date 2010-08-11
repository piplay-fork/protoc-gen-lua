/*
 * =====================================================================================
 *
 *      Filename:  pb.c
 *
 *      Description: protoc-gen-lua
 *      Google's Protocol Buffers project, ported to lua.
 *      https://code.google.com/p/protoc-gen-lua/
 *
 *      Copyright (c) 2010 , 林卓毅 (Zhuoyi Lin) netsnail@gmail.com
 *      All rights reserved.
 *
 *      Use, modification and distribution are subject to the "New BSD License"
 *      as listed at <url: http://www.opensource.org/licenses/bsd-license.php >.
 *
 *      Created:  2010年08月02日 18时04分21秒
 *
 *      Company:  NetEase
 *
 * =====================================================================================
 */
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN
#endif

static void pack_varint(luaL_Buffer *b, uint64_t value)
{
    if (value >= 0x80)
    {
        luaL_addchar(b, value | 0x80);
        value >>= 7;
        if (value >= 0x80)
        {
            luaL_addchar(b, value | 0x80);
            value >>= 7;
            if (value >= 0x80)
            {
                luaL_addchar(b, value | 0x80);
                value >>= 7;
                if (value >= 0x80)
                {
                    luaL_addchar(b, value | 0x80);
                    value >>= 7;
                    if (value >= 0x80)
                    {
                        luaL_addchar(b, value | 0x80);
                        value >>= 7;
                        if (value >= 0x80)
                        {
                            luaL_addchar(b, value | 0x80);
                            value >>= 7;
                            if (value >= 0x80)
                            {
                                luaL_addchar(b, value | 0x80);
                                value >>= 7;
                                if (value >= 0x80)
                                {
                                    luaL_addchar(b, value | 0x80);
                                    value >>= 7;
                                    if (value >= 0x80)
                                    {
                                        luaL_addchar(b, value | 0x80);
                                        value >>= 7;
                                    } 
                                } 
                            } 
                        } 
                    } 
                } 
            } 
        } 
    }
    luaL_addchar(b, value);
} 

static int varint_encoder(lua_State *L)
{
    lua_Number l_value = luaL_checknumber(L, 2);
    uint64_t value = (uint64_t)l_value;

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    
    pack_varint(&b, value);

    lua_settop(L, 1);
    luaL_pushresult(&b);
    lua_call(L, 1, 0);
    return 0;
}

static int signed_varint_encoder(lua_State *L)
{
    lua_Number l_value = luaL_checknumber(L, 2);
    int64_t value = (int64_t)l_value;
    
    luaL_Buffer b;
    luaL_buffinit(L, &b);

    if (value < 0)
    {
        pack_varint(&b, *(uint64_t*)&value);
    }else{
        pack_varint(&b, value);
    }
    
    lua_settop(L, 1);
    luaL_pushresult(&b);
    lua_call(L, 1, 0);
    return 0;
}

static int pack_fixed32(lua_State *L, uint8_t* value){
#ifdef IS_LITTLE_ENDIAN
    lua_pushlstring(L, value, 4);
#else
    uint32_t v = htole32(*(uint32_t*)value);
    lua_pushlstring(L, (char*)&v, 4);
#endif
}

static int pack_fixed64(lua_State *L, uint8_t* value){
#ifdef IS_LITTLE_ENDIAN
    lua_pushlstring(L, value, 8);
#else
    uint64_t v = htole32(*(uint64_t*)value);
    lua_pushlstring(L, (char*)&v, 8);
#endif
}

static int struct_pack(lua_State *L)
{
    uint8_t format = luaL_checkinteger(L, 1);
    lua_Number value = luaL_checknumber(L, 2);

    switch(format){
        case 'i':
            {
                int32_t v = (int32_t)value;
                pack_fixed32(L, (uint8_t*)&v);
                break;
            }
        case 'q':
            {
                int64_t v = (int64_t)value;
                pack_fixed64(L, (uint8_t*)&v);
                break;
            }
        case 'f':
            {
                float v = (float)value;
                pack_fixed32(L, (uint8_t*)&v);
                break;
            }
        case 'd':
            {
                double v = (double)value;
                pack_fixed64(L, (uint8_t*)&v);
                break;
            }
        case 'I':
            {
                uint32_t v = (uint32_t)value;
                pack_fixed32(L, (uint8_t*)&v);
                break;
            }
        case 'Q':
            {
                uint64_t v = (uint64_t) value;
                pack_fixed64(L, (uint8_t*)&v);
                break;
            }
        default:
            luaL_error(L, "Unknown, format");
    }
    return 1;
}

static size_t size_varint(const char* buffer, size_t len)
{
    size_t pos = 0;
    while(buffer[pos] & 0x80){
        ++pos;
        if(pos > len){
            return -1;
        }
    }
    return pos+1;
}

static uint64_t unpack_varint(const char* buffer, size_t len)
{
    uint64_t value = buffer[0] & 0x7f;
    size_t shift = 7;
    for(size_t pos = 1; pos < len; ++pos)
    {
        value |= ((uint64_t)(buffer[pos] & 0x7f)) << shift;
        shift += 7;
    }
    return value;
}

static int varint_decoder(lua_State *L)
{
    size_t len;
    const char* buffer = luaL_checklstring(L, 1, &len);
    size_t pos = luaL_checkinteger(L, 2);
    
    buffer += pos;
    len = size_varint(buffer, len);
    if(len == -1){
        luaL_error(L, "error data %s, len:%d", buffer, len);
    }else{
        lua_pushnumber(L, (lua_Number)unpack_varint(buffer, len));
        lua_pushinteger(L, len + pos);
    }
    return 2;
}

static int signed_varint_decoder(lua_State *L)
{
    size_t len;
    const char* buffer = luaL_checklstring(L, 1, &len);
    size_t pos = luaL_checkinteger(L, 2);
    buffer += pos;
    len = size_varint(buffer, len);
    
    if(len == -1){
        luaL_error(L, "error data %s, len:%d", buffer, len);
    }else{
        lua_pushnumber(L, (lua_Number)(int64_t)unpack_varint(buffer, len));
        lua_pushinteger(L, len + pos);
    }
    return 2;
}

static int read_tag(lua_State *L)
{
    size_t len;
    const char* buffer = luaL_checklstring(L, 1, &len);
    size_t pos = luaL_checkinteger(L, 2);
    
    buffer += pos;
    len = size_varint(buffer, len);
    if(len == -1){
        luaL_error(L, "error data %s, len:%d", buffer, len);
    }else{
        lua_pushlstring(L, buffer, len);
        lua_pushinteger(L, len + pos);
    }
    return 2;
}

static const uint8_t* unpack_fixed32(const uint8_t* buffer, uint8_t* cache)
{
#ifdef IS_LITTLE_ENDIAN
    return buffer;
#else
    *(uint32_t*)cache = le32toh(*(uint32_t*)buffer);
    return cache;
#endif
}

static const uint8_t* unpack_fixed64(const uint8_t* buffer, uint8_t* cache)
{
#ifdef IS_LITTLE_ENDIAN
    return buffer;
#else
    *(uint64_t*)cache = le64toh(*(uint64_t*)buffer);
    return cache;
#endif
}

static int struct_unpack(lua_State *L)
{
    uint8_t format = luaL_checkinteger(L, 1);
    size_t len;
    const char* buffer = luaL_checklstring(L, 2, &len);
    size_t pos = luaL_checkinteger(L, 3);

    buffer += pos;
    uint8_t out[8];
    switch(format){
        case 'i':
            {
                lua_pushinteger(L, *(int32_t*)unpack_fixed32(buffer, out));
                break;
            }
        case 'q':
            {
                lua_pushnumber(L, (lua_Number)*(int64_t*)unpack_fixed64(buffer, out));
                break;
            }
        case 'f':
            {
                lua_pushnumber(L, *(lua_Number*)unpack_fixed32(buffer, out));
                break;
            }
        case 'd':
            {
                lua_pushnumber(L, (lua_Number)*(double*)unpack_fixed64(buffer, out));
                break;
            }
        case 'I':
            {
                lua_pushnumber(L, *(lua_Number*)unpack_fixed32(buffer, out));
                break;
            }
        case 'Q':
            {
                lua_pushnumber(L, (lua_Number)*(uint64_t*)unpack_fixed64(buffer, out));
                break;
            }
        default:
            luaL_error(L, "Unknown, format");
    }
    return 1;
}

static const struct luaL_reg _pb [] = {
    {"varint_encoder", varint_encoder},
    {"signed_varint_encoder", signed_varint_encoder},
    {"read_tag", read_tag},
    {"struct_pack", struct_pack},
    {"struct_unpack", struct_unpack},
    {"varint_decoder", varint_decoder},
    {"signed_varint_decoder", signed_varint_decoder},
    {NULL, NULL}
};


int luaopen_pb (lua_State *L)
{
    luaL_register(L, "pb", _pb);
    return 1;
}