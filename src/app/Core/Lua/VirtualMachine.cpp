#include "pch.hpp"
#include "VirtualMachine.hpp"

std::ostream &lua::VirtualMachine::type(std::ostream &out, int type) {
    // Return a string representation of a Lua type
    // ex: ... returned ________ type value
    switch (type) {
        case LUA_TNIL:
            out << "nil";
            break;
        case LUA_TBOOLEAN:
            out << "boolean";
            break;
        case LUA_TLIGHTUSERDATA:
            out << "pointer";
            break;
        case LUA_TNUMBER:
            out << "number";
            break;
        case LUA_TSTRING:
            out << "string";
            break;
        case LUA_TTABLE:
            out << "table";
            break;
        case LUA_TFUNCTION:
            out << "function";
            break;
        case LUA_TUSERDATA:
            out << "userdata";
            break;
        case LUA_TTHREAD:
            out << "thread";
            break;
        default:  // Shouldn't happen, but might as well have it
            out << "unknown";
            break;
    }
    return out;
}

std::ostream &lua::VirtualMachine::status(std::ostream &out, int status) {
    // Return a string representation of a Lua status
    // ex: ... resulted in ________
    switch (status) {
        case LUA_OK:
            out << "success";
            break;
        case LUA_YIELD:
            out << "yield";
            break;
        case LUA_ERRRUN:
            out << "runtime error";
            break;
        case LUA_ERRSYNTAX:
            out << "syntax error";
            break;
        case LUA_ERRMEM:
            out << "memory allocation error";
            break;
        case LUA_ERRERR:
            out << "message handler error";
            break;
        default:  // Shouldn't happen, but might as well have it
            out << "unknown status";
            break;
    }
    return out;
}

lua::VirtualMachine::VirtualMachine() : m_state(luaL_newstate()) {}

lua::VirtualMachine::~VirtualMachine() {
    lua_close(m_state);
}

int lua::VirtualMachine::runString(const std::string &command) {
    return luaL_dostring(m_state, command.c_str());
}

int lua::VirtualMachine::runFile(const std::string &file) {
    return luaL_dofile(m_state, file.c_str());
}

int lua::VirtualMachine::runBuffer(const unsigned char *buffer, size_t length) {
    return luaL_loadbufferx(m_state, (const char*)buffer, length, "", nullptr) || lua_pcall(m_state, 0, LUA_MULTRET, 0);
}

lua::VirtualMachine::operator lua_State*() {
    return m_state;
}

void lua::VirtualMachine::addCommonLibraries() {
    luaL_openlibs(m_state);
}

bool lua::VirtualMachine::validate(int r_code, std::ostream& out) {
    if (r_code != LUA_OK) {
        std::string error_msg {lua_tostring(m_state, -1)};
        //out << status(r_code) << std::endl;
        out << error_msg << std::endl;
        return false;
    } else {
        return true;
    }
    return false;
}

void lua::VirtualMachine::setGlobal(lua::Table &table) {
    lua_setglobal(m_state, table.name().c_str());
}

void lua::VirtualMachine::setGlobal(std::string &name) {
    lua_setglobal(m_state, name.c_str());
}

void lua::VirtualMachine::setGlobal(const char *name) {
    lua_setglobal(m_state, name);
}

lua::Table lua::VirtualMachine::table(const char *name) {
    return {m_state, name};
}

lua::Function lua::VirtualMachine::function(const char *name, int nargs, int nres) {
    return {m_state, name, nargs, nres};
}

void lua::VirtualMachine::pushInteger(int value) {
    lua_pushinteger(m_state, static_cast<lua_Integer>(value));
}

void lua::VirtualMachine::pushInteger(size_t value) {
    lua_pushinteger(m_state, static_cast<lua_Integer>(value));
}

void lua::VirtualMachine::pushNumber(double value) {
    lua_pushnumber(m_state, value);
}

void lua::VirtualMachine::pushString(std::string &value) {
    lua_pushstring(m_state, value.c_str());
}

void lua::VirtualMachine::pushString(const char *value) {
    lua_pushstring(m_state, value);
}

void lua::VirtualMachine::pop(int n) {
    lua_pop(m_state, n);
}

lua_State *lua::raw(lua::VirtualMachine &lvm) {
    return static_cast<lua_State*>(lvm);
}
