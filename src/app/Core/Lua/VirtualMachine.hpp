#pragma once

#include "pch.hpp"
#include "Errors.hpp"
#include "Common.hpp"
#include "Table.hpp"
#include "Function.hpp"

namespace lua {
    // Singleton Lua Virtual Machine. I don't see any use for another Lua instance.
    //   Users can only access data pushed to Lua through the first instance.
    class VirtualMachine {
    public:
        static VirtualMachine &get();

    private:
        template<typename T, typename luaT>
        T to_value(int* isNum, luaT(*lua_get)(lua_State*, int, int*)) {
            const T value = static_cast<T>(lua_get(state, -1, isNum));
            lua_pop(state, 1);
            return value;
        }

        template<typename T, typename luaT>
        T to_value_backup(T backup, luaT(*lua_get)(lua_State*, int, int*)) {
            int isNum;
            T value = static_cast<T>(lua_get(state, -1, &isNum));
            if (!isNum) {
                value = backup;
            }
            lua_pop(state, 1);
            return value;
        }

        VirtualMachine();

    public:
        VirtualMachine(VirtualMachine const &) = delete;

        VirtualMachine(VirtualMachine &&) = delete;

        void add_basic_libraries() const;

        template<lua::Code C>
        [[nodiscard]] int run(C& code) {
            return code.run(state);
        }

        [[nodiscard]] int run(std::string const &) const;
        [[nodiscard]] int run(const char*) const;

        [[nodiscard]] Table table(std::string name) const;
        [[nodiscard]] Function function(std::string name, int arguments = 0, int results = 0) const;

        void push_global(Table const& value) const;
        void push_number(lua_Number value) const;
        void push_integer(lua_Integer value) const;
        void push_string(const char* value) const;

        [[nodiscard]] lua_Integer to_integer(int* isNum = nullptr);
        [[nodiscard]] lua_Number to_number(int* isNum = nullptr);
        [[nodiscard]] std::string to_string();

        template<std::integral T>
        [[nodiscard]] T to_integer(T backup) {
            return to_value_backup(backup, lua_tointegerx);
        }

        template<std::floating_point T>
        [[nodiscard]] T to_number(T backup) {
            return to_value_backup(backup, lua_tonumberx);
        }

        [[nodiscard]] std::string to_string(std::string const& backup);

        void validate(int) const;

        void log(int) const;

        lua_State *state = luaL_newstate();
    };
}
