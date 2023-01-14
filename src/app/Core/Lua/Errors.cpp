#include "pch.hpp"
#include "Errors.hpp"

lua::exception::exception(const std::string &message) noexcept : ExceptionBase(message.c_str()) {}

lua::exception::exception(const char *message) noexcept: ExceptionBase(message) {}

static void LuaError_lua_resource_delete(lua_State *L) {
    lua_pop(L, 1);
}

lua::LuaError::LuaError(lua_State *L) : m_L(L), m_lua_resource(L, LuaError_lua_resource_delete) {}

const char *lua::LuaError::what() const noexcept {
    const char *s = lua_tostring(m_L, -1);
    if (s == nullptr) { s = "unrecognized Lua error"; }
    return s;
}
