#pragma once
#include "pch.hpp"

// Taken from http://lua-users.org/wiki/ErrorHandlingBetweenLuaAndCplusplus
namespace lua {
    class LuaError : public std::exception
    {
    private:
        lua_State * m_L;
        // resource for error object on Lua stack (is to be popped
        // when no longer used)
        std::shared_ptr<lua_State> m_lua_resource;
        LuaError & operator=(const LuaError & other) = default; // prevent
    public:
        // Construct using top-most element on Lua stack as error.
        explicit LuaError(lua_State * L);
        LuaError(LuaError const& other) = default;
        ~LuaError() override = default;
        [[nodiscard]] const char * what() const noexcept override;
    };
}
