// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "Common.hpp"

std::ostream &lua::type(std::ostream &out, int type) {
    // Return a string representation of a Lua type
    // ex: ... returned ________ type value
    out << [](int _t) {
        switch (_t) {
            case LUA_TNIL:           return "nil";
            case LUA_TBOOLEAN:       return "boolean";
            case LUA_TLIGHTUSERDATA: return "pointer";
            case LUA_TNUMBER:        return "number";
            case LUA_TSTRING:        return "string";
            case LUA_TTABLE:         return "table";
            case LUA_TFUNCTION:      return "function";
            case LUA_TUSERDATA:      return "userdata";
            case LUA_TTHREAD:        return "thread";
            default:  // Shouldn't happen, but might as well have it
                return "unknown";
        }
    }(type);
    return out;
}

std::ostream &lua::status(std::ostream &out, int status) {
    // Return a string representation of a Lua status
    // ex: ... resulted in ________
    out << [](int _s) {
        switch (_s) {
            case LUA_OK:        return "success";
            case LUA_YIELD:     return "yield";
            case LUA_ERRRUN:    return "runtime error";
            case LUA_ERRSYNTAX: return "syntax error";
            case LUA_ERRMEM:    return "memory allocation error";
            case LUA_ERRERR:    return "message handler error";
            default:  // Shouldn't happen, but might as well have it
                return "unknown status";
        }
    }(status);
    return out;
}

lua::CodeFile::CodeFile(std::string f) : filename(std::move(f)) {}

lua::CodeFile::CodeFile(const char *f) : filename(f) {}

int lua::CodeFile::run(lua_State *L) {
    return luaL_dofile(L, filename.c_str());
}

lua::CodeBuffer::CodeBuffer(const char* b, std::size_t s): buffer(b), size(s) {}

int lua::CodeBuffer::run(lua_State *L) {
    return luaL_loadbufferx(L, (const char*)buffer, size, "", nullptr) || lua_pcall(L, 0, LUA_MULTRET, 0);
}
