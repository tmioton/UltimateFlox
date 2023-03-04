#pragma once

#include "pch.hpp"
#include "../Common.hpp"


namespace lua {
    class VirtualMachine;

    struct LuaVector {
        // Vector using double because Lua operates in doubles
        using Vector_t = glm::vec<Vector::length(), double, glm::defaultp>;

        static Vector to_vector(Vector_t *val);

        static void add_to_lua(lua_State *L);
    private:
        friend VirtualMachine;

        static int create(lua_State *);

        static int destroy(lua_State *);

        static int add(lua_State *);

        static int sub(lua_State *);

        static int mul(lua_State *);

        static int div(lua_State *);

        //static int unm(lua_State*);

        //static int pow(lua_State*);

        static int binary_operation(lua_State *L, Vector_t(f)(Vector_t, Vector_t));
    };
}
