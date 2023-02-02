#include "pch.hpp"
#include "VirtualMachine.hpp"
#include "Types/LuaVector.hpp"

lua::VirtualMachine &lua::VirtualMachine::get() {
    static VirtualMachine instance;
    return instance;
}

lua::VirtualMachine::VirtualMachine() {
    lua::LuaVector::addToLua(state);
}

void lua::VirtualMachine::addBasicLibraries() const {
    luaL_openlibs(state);
}

int lua::VirtualMachine::run(const std::string &command) const {
    return luaL_dostring(state, command.c_str());
}

int lua::VirtualMachine::run(const char *command) const {
    return luaL_dostring(state, command);
}

void lua::VirtualMachine::validate(int result) const {
#ifdef LUA_DEBUG
    if (result != LUA_OK) {
        throw LuaError(state);
    }
#endif
}

void lua::VirtualMachine::log(int result) const {
    if (result != LUA_OK) {
        // Just cout for now.
        //std::string error_msg {lua_tostring(state, -1)};
        //std::cout << "Lua status " << lua::status(std::cout, result) << error_msg << std::endl;
    }
}

lua::Table lua::VirtualMachine::table(std::string name) const {
    return {state, std::move(name)};
}

lua::Function lua::VirtualMachine::function(std::string name, int arguments, int results) const {
    return {state, std::move(name), arguments, results};
}

void lua::VirtualMachine::pushGlobal(const Table &table) const {
    lua_setglobal(state, table.name().c_str());
}

void lua::VirtualMachine::pushNumber(lua_Number value) const {
    lua_pushnumber(state, value);
}

void lua::VirtualMachine::pushInteger(lua_Integer value) const {
    lua_pushinteger(state, value);
}

void lua::VirtualMachine::pushString(const char *value) const {
    lua_pushstring(state, value);
}

lua_Integer lua::VirtualMachine::toInteger(int* isNum) {
    return toValue<lua_Integer>(isNum, lua_tointegerx);
}

lua_Number lua::VirtualMachine::toNumber(int *isNum) {
    return toValue<lua_Number>(isNum, lua_tonumberx);
}

std::string lua::VirtualMachine::toString() {
    std::size_t length;
    const char* internal = lua_tolstring(state, -1, &length);
    std::string value{internal, length};
    lua_pop(state, 1);
    return value;
}

std::string lua::VirtualMachine::toString(const std::string &backup) {
    std::size_t length;
    const char* internal_string = lua_tolstring(state, -1, &length);
    if (length == 0) {
        lua_pop(state, 1);
        return backup;
    }
    std::string value{internal_string, length};
    lua_pop(state, 1);
    return value;
}
