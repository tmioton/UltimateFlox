#include "pch.hpp"
#include "Table.hpp"
#include "Errors.hpp"

lua::Table::Table(lua_State* s, std::string n) : m_state(s), m_name(std::move(n)) {}

void lua::Table::create(int sl, int ol) {
    // The error handling for this is checking the stack before calling this.
    lua_createtable(m_state, sl, ol);
    m_index = lua_gettop(m_state);
}

bool lua::Table::push() {
    lua_getglobal(m_state, m_name.c_str());
    if (!lua_istable(m_state, -1)) [[unlikely]] {
        lua_pop(m_state, 1);
        return false;
    }
    m_index = lua_gettop(m_state);
    return true;
}

bool lua::Table::pop() {
    bool ret = lua_istable(m_state, -1) || lua_isnil(m_state, -1);
    lua_pop(m_state, 1);
#ifdef LUA_DEBUG
    if (!ret) {
        throw lua::exception("Attempt to pop unrelated value from lua stack.");
    }
#endif
    return ret;
}

std::string const &lua::Table::name() const {
    return m_name;
}

void lua::Table::push_integer(const char *key, lua_Integer value) {
    lua_pushstring(m_state, key);
    lua_pushinteger(m_state, value);
    lua_settable(m_state, m_index);
}

void lua::Table::push_number(const char *key, lua_Number value) {
    lua_pushstring(m_state, key);
    lua_pushnumber(m_state, value);
    lua_settable(m_state, m_index);
}

void lua::Table::push_string(const char *key, const char *value) {
    lua_pushstring(m_state, key);
    lua_pushstring(m_state, value);
    lua_settable(m_state, m_index);
}

lua_Integer lua::Table::to_integer(const char *key, int* isNum) {
    return to_value<lua_Integer>(key, isNum, lua_tointegerx);
}

lua_Number lua::Table::to_number(const char *key, int *isNum) {
    return to_value<lua_Number>(key, isNum, lua_tonumberx);
}

std::string lua::Table::to_string(const char *key) {
    lua_pushstring(m_state, key);
    lua_gettable(m_state, m_index);
    std::size_t length;
    const char* internal = lua_tolstring(m_state, -1, &length);
    std::string value{internal, length};
    lua_pop(m_state, 1);
    return value;
}

std::string lua::Table::to_string(const char *key, const std::string &backup) {
    lua_pushstring(m_state, key);
    lua_gettable(m_state, m_index);
    std::size_t length;
    const char* internal_string = lua_tolstring(m_state, -1, &length);
    if (length == 0) {
        lua_pop(m_state, 1);
        return backup;
    }
    std::string value{internal_string, length};
    lua_pop(m_state, 1);
    return value;
}
