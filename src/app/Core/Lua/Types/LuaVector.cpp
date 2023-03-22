// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "LuaVector.hpp"

void lua::LuaVector::add_to_lua(lua_State* L) {
    lua_pushcfunction(L, create);
    lua_setglobal(L, "CreateVector");
    luaL_newmetatable(L, "Vector");
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, destroy);
    lua_settable(L, -3);
    lua_pushstring(L, "__add");
    lua_pushcfunction(L, add);
    lua_settable(L, -3);
    lua_pushstring(L, "__sub");
    lua_pushcfunction(L, sub);
    lua_settable(L, -3);
    lua_pushstring(L, "__mul");
    lua_pushcfunction(L, mul);
    lua_settable(L, -3);
    lua_pushstring(L, "__div");
    lua_pushcfunction(L, div);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

Vector lua::LuaVector::to_vector(lua::LuaVector::Vector_t *val) {
    return Vector{static_cast<float>(val->x), static_cast<float>(val->y)};
}

int lua::LuaVector::create(lua_State *L) {
    int numArgs = lua_gettop(L);
    void* reserved = lua_newuserdata(L, sizeof(Vector_t));  // This is what's at index -1
    if (numArgs == 0) {
        new (reserved) Vector_t{0.0, 0.0};
    } else if (numArgs == 1) {
        int success;
        double value = lua_tonumberx(L, 1, &success);
        if (!success) { lua::error(L, "Invalid argument in call to CreateVector(v)."); }
        new (reserved) Vector_t{value, value};
    } else if (numArgs == 2) {
        int success;
        double y = lua_tonumberx(L, 2, &success);
        if (!success) { lua::error(L, "Invalid argument for x in call to CreateVector(x, y)."); }
        double x = lua_tonumberx(L, 1, &success);
        if (!success) { lua::error(L, "Invalid argument for y in call to CreateVector(x, y)."); }
        new (reserved) Vector_t{x, y};
    } else {
        lua::error(L, "Invalid number of arguments in call to CreateVector([x,[y]]).");
    }

    // We'll just assume this metatable exists.
    // This function shouldn't be accessible from Lua without the addToLua method that creates this metatable.
    luaL_getmetatable(L, "Vector");
    lua_setmetatable(L, -2);
    return 1;
}
int lua::LuaVector::destroy(lua_State *L) {
    auto vec = (Vector_t*) lua_touserdata(L, -1);
    vec->~Vector_t();
    return 0;
}

int lua::LuaVector::add(lua_State *L) { return binary_operation(L, [](Vector_t lhs, Vector_t rhs) { return lhs + rhs; }); }
int lua::LuaVector::sub(lua_State *L) { return binary_operation(L, [](Vector_t lhs, Vector_t rhs) { return lhs - rhs; }); }
int lua::LuaVector::mul(lua_State *L) { return binary_operation(L, [](Vector_t lhs, Vector_t rhs) { return lhs * rhs; }); }
int lua::LuaVector::div(lua_State *L) { return binary_operation(L, [](Vector_t lhs, Vector_t rhs) { return lhs / rhs; }); }

int lua::LuaVector::binary_operation(lua_State *L, lua::LuaVector::Vector_t (*f)(Vector_t, Vector_t)) {
    if (lua_gettop(L) != 2) {
        // This is possible with getmetatable(CreateVector()).__add(1, 1, 1)
        lua::error(L, "Invalid number of arguments in Vector addition.");
    }

    Vector_t returnValue;
    if (lua_isuserdata(L, 1)) {
        void* sanity = luaL_testudata(L, 1, "Vector");
        if (sanity == nullptr) {
            // Some userdata object other than a vector.
            lua::error(L, "Unsupported left operand in Vector addition.");
        }

        returnValue = *(Vector_t*)sanity;
        if (lua_isnumber(L, 2)) {
            returnValue = f(returnValue, Vector_t{lua_tonumber(L, 2)});
        } else if (lua_isuserdata(L, 2)) {
            sanity = luaL_testudata(L, 2, "Vector");
            if (sanity == nullptr) {
                // Some userdata object other than a vector.
                lua::error(L, "Unsupported right operand in Vector addition.");
            }

            returnValue = f(returnValue, *(Vector_t*)sanity);
        } else {
            // Possible with CreateVector() + {}
            lua::error(L, "Unsupported right operand in Vector addition.");
        }
    } else if (lua_isnumber(L, 1)) {
        returnValue = Vector_t{lua_tonumber(L, 1)};
        if (lua_isnumber(L, 2)) {
            // Possible with getmetatable(CreateVector()).__add(1, 1)
            returnValue = f(returnValue, Vector_t{lua_tonumber(L, 2)});
        } else if (lua_isuserdata(L, 2)) {
            void* sanity = luaL_testudata(L, 2, "Vector");
            if (sanity == nullptr) {
                // Some userdata object other than a vector.
                lua::error(L, "Unsupported right operand in Vector addition.");
            }
            returnValue = f(returnValue, *(Vector_t*)sanity);
        } else {
            // Possible with getmetatable(CreateVector()).__add(3, {})
            lua::error(L, "Unsupported right operand in Vector addition.");
        }
    } else {
        // Possible with {} + CreateVector()
        lua::error(L, "Unsupported left operand in Vector addition.");
    }

    lua_pop(L, 2);
    lua_pushnumber(L, returnValue.x);
    lua_pushnumber(L, returnValue.y);
    create(L);

    return 1;
}
