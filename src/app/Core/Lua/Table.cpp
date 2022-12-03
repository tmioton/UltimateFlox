#include "pch.hpp"
#include "Table.hpp"

lua::Table::Table(lua_State *L, std::string name) : m_state(L), m_name(std::move(name)) {}

void lua::Table::create(int narr, int nrec) {
    lua_createtable(m_state, narr, nrec);
}

std::string lua::Table::getString(const char *name, std::string backup) {
    std::string value;
    lua_pushstring(m_state, name);
    lua_gettable(m_state, -2);
    if (lua_isstring(m_state, -1)) {
        value = lua_tostring(m_state, -1);
    } else {
        lua_pop(m_state, 1);
        value = std::move(backup);
    }

    lua_pop(m_state, 1);
    return value;
}

std::optional<std::string> lua::Table::getString(const char *name) {
    std::optional<std::string> value;
    lua_pushstring(m_state, name);
    lua_gettable(m_state, -2);
    if (lua_isstring(m_state, -1)) {
        value = lua_tostring(m_state, -1);
    } else {
        lua_pop(m_state, 1);
        value = std::nullopt;
    }
    lua_pop(m_state, 1);
    return value;
}

void lua::Table::setNumber(const char* name, double value) {
    lua_pushstring(m_state, name);
    lua_pushnumber(m_state, value);
    lua_settable(m_state, -3);
}

void lua::Table::setInteger(const char *name, int value) {
    lua_pushstring(m_state, name);
    lua_pushinteger(m_state, static_cast<lua_Integer>(value));
    lua_settable(m_state, -3);
}

void lua::Table::setInteger(const char *name, size_t value) {
    lua_pushstring(m_state, name);
    lua_pushinteger(m_state, static_cast<lua_Integer>(value));
    lua_settable(m_state, -3);
}

void lua::Table::setString(const char *name, std::string &value) {
    lua_pushstring(m_state, name);
    lua_pushstring(m_state, value.c_str());
    lua_settable(m_state, -3);
}

std::string &lua::Table::name() {
    return m_name;
}

bool lua::Table::push() {
    lua_getglobal(m_state, m_name.c_str());
    if (lua_istable(m_state, -1)) {
        return true;
    } else {
        lua_pop(m_state, 1);  // Need to pop off the "nil" Lua placed on the stack.
        return false;
    }
}

void lua::Table::pop() {
    lua_pop(m_state, 1);
}
