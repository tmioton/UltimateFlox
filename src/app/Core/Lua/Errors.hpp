#pragma once
#include "pch.hpp"

namespace lua {

    class exception : public std::exception {
    public:
        using ExceptionBase = std::exception;
        exception() noexcept = default;
        explicit exception(std::string const& message) noexcept;
        explicit exception(const char* message) noexcept;
    };

    class LuaError : public exception {
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
