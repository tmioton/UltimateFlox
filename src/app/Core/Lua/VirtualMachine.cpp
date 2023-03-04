#include "pch.hpp"
#include "VirtualMachine.hpp"
#include "Types/LuaVector.hpp"

lua::VirtualMachine &lua::VirtualMachine::get() {
    static VirtualMachine instance;
    return instance;
}

lua::VirtualMachine::VirtualMachine() {
    lua::LuaVector::add_to_lua(state);
}

void lua::VirtualMachine::add_basic_libraries() const {
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
        std::cout << "Lua status ";
        lua::status(std::cout, result);
        std::cout << lua_tostring(state, -1) << '\n';
    }
}

lua::Table lua::VirtualMachine::table(std::string name) const {
    return {state, std::move(name)};
}

lua::Function lua::VirtualMachine::function(std::string name, int arguments, int results) const {
    return {state, std::move(name), arguments, results};
}

void lua::VirtualMachine::push_global(const Table &value) const {
    lua_setglobal(state, value.name().c_str());
}

void lua::VirtualMachine::push_number(lua_Number value) const {
    lua_pushnumber(state, value);
}

void lua::VirtualMachine::push_integer(lua_Integer value) const {
    lua_pushinteger(state, value);
}

void lua::VirtualMachine::push_string(const char *value) const {
    lua_pushstring(state, value);
}

lua_Integer lua::VirtualMachine::to_integer(int* isNum) {
    return to_value<lua_Integer>(isNum, lua_tointegerx);
}

lua_Number lua::VirtualMachine::to_number(int *isNum) {
    return to_value<lua_Number>(isNum, lua_tonumberx);
}

std::string lua::VirtualMachine::to_string() {
    std::size_t length;
    const char* internal = lua_tolstring(state, -1, &length);
    std::string value{internal, length};
    lua_pop(state, 1);
    return value;
}

std::string lua::VirtualMachine::to_string(const std::string &backup) {
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
