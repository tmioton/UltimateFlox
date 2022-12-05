#pragma once

#include "pch.hpp"
#include "Core/Lua/Lua.hpp"


namespace lua {
    struct LuaVector {
        // Vector using double because Lua operates in doubles
        using Vector_t = glm::vec<Vector::length(), double, glm::highp>;

        static void addToLua(lua_State *);

        static Vector toVector(Vector_t *);

    private:
        static int create(lua_State *);

        static int destroy(lua_State *);

        static int add(lua_State *);

        static int sub(lua_State *);

        static int mul(lua_State *);

        static int div(lua_State *);

        //static int unm(lua_State*);

        //static int pow(lua_State*);

        static int operation(lua_State *, Vector_t(f)(Vector_t, Vector_t));

        static Vector_t add(Vector_t lhs, Vector_t rhs);

        static Vector_t sub(Vector_t lhs, Vector_t rhs);

        static Vector_t mul(Vector_t lhs, Vector_t rhs);

        static Vector_t div(Vector_t lhs, Vector_t rhs);
    };
}
