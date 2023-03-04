#pragma once
#include "pch.hpp"
#include "Common.hpp"


namespace lua {
    class VirtualMachine;

    class Table {
        template<typename T, typename luaT>
        T to_value(const char* key, int* isNum, luaT(*lua_get)(lua_State*, int, int*)) {
            lua_pushstring(m_state, key);
            lua_gettable(m_state, m_index);
            const T value = static_cast<T>(lua_get(m_state, -1, isNum));
            lua_pop(m_state, 1);
            return value;
        }

        template<typename T, typename luaT>
        T to_value_backup(const char* key, T backup, luaT(*lua_get)(lua_State*, int, int*)) {
            lua_pushstring(m_state, key);
            lua_gettable(m_state, m_index);
            int isNum;
            T value = static_cast<T>(lua_get(m_state, -1, &isNum));
            if (!isNum) {
                value = backup;
            }
            lua_pop(m_state, 1);
            return value;
        }

        friend VirtualMachine;
        Table(lua_State*, std::string name);
    public:
        Table(Table const&) = default;
        Table(Table&&) = default;

        Table& operator=(Table const&) = default;
        Table& operator=(Table&&) = default;

        // Lua Operations
        void create(int sequence_length = 0, int other_elements = 0);
        bool push();
        bool pop();

        void push_integer(const char *key, lua_Integer value);
        void push_number(const char *key, lua_Number value);
        void push_string(const char *key, const char* value);

        [[nodiscard]] lua_Integer to_integer(const char *key, int* isNum = nullptr);
        [[nodiscard]] lua_Number to_number(const char *key, int* isNum = nullptr);
        [[nodiscard]] std::string to_string(const char *key);

        template<std::integral T>
        [[nodiscard]] T to_integer(const char *key, T backup) {
            return to_value_backup(key, backup, lua_tointegerx);
        }

        template<std::floating_point T>
        [[nodiscard]] T to_number(const char *key, T backup) {
            return to_value_backup(key, backup, lua_tonumberx);
        }

        [[nodiscard]] std::string to_string(const char *key, std::string const& backup);

        [[nodiscard]] std::string const& name() const;

    private:
        lua_State* m_state;
        std::string m_name;
        int m_index = 0;
    };
}
