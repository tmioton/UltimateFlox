#pragma once
#include "pch.hpp"
#include "Lua.hpp"


namespace lua {
    class Table {
        lua_State *m_state;
        std::string m_name;

        template<typename T, typename lua_T>
        static std::optional<T> getValue(lua_State* L, const char* name, lua_T(*lua_get)(lua_State*, int, int*)) {
            std::optional<T> value;
            lua_pushstring(L, name);
            lua_gettable(L, -2);
            int success;
            value = static_cast<T>(lua_get(L, -1, &success));
            if (!success) [[unlikely]] {  // ask for forgiveness
                lua_pop(L, 1);
                value = std::nullopt;
            }
            lua_pop(L, 1);
            return value;
        }

        template<typename T, typename lua_T>
        static T getValue(lua_State* L, const char* name, T backup, lua_T(*lua_get)(lua_State*, int, int*)) {
            T value;
            lua_pushstring(L, name);
            lua_gettable(L, -2);
            int success;
            value = static_cast<T>(lua_get(L, -1, &success));
            if (!success) [[unlikely]] {  // ask for forgiveness
                lua_pop(L, 1);
                value = std::move(backup);
            }
            lua_pop(L, 1);
            return value;
        }
    public:
        Table(lua_State*, std::string name);
        Table(Table const&) = default;
        Table(Table&&) = default;

        Table& operator=(Table const&) = default;
        Table& operator=(Table&&) = default;

        void create(int, int);

        // TODO: Make a version of these that accepts a bool reference like lua_to*x(lua_State*, int, int*)

        template<typename T>
        T getNumber(const char* name, T backup) {
            return getValue<T, lua_Number>(m_state, name, backup, lua_tonumberx);
        }

        template<typename T>
        T getInteger(const char* name, T backup) {
            return getValue<T, lua_Integer>(m_state, name, backup, lua_tointegerx);
        }

        std::string getString(const char* name, std::string backup);

        template<typename T>
        std::optional<T> getNumber(const char* name) {
            return getValue<T>(m_state, name, lua_tonumberx);
        }

        template<typename T>
        std::optional<T> getInteger(const char* name) {
            return getValue<T>(m_state, name, lua_tointegerx);
        }

        std::optional<std::string> getString(const char* name);

        void setNumber(const char* name, double);
        void setInteger(const char* name, int);
        void setInteger(const char* name, size_t);
        void setString(const char* name, std::string&);

        bool push();
        void pop();
        std::string& name();
    };
}
