#pragma once
#include "pch.hpp"


namespace lua {
    class Table {
        lua_State *m_state;
        std::string m_name;
    public:
        Table(lua_State*, std::string name);
        Table(Table const&) = default;
        Table(Table&&) = default;

        Table& operator=(Table const&) = default;
        Table& operator=(Table&&) = default;

        void create(int, int);

        double getNumber(const char* name, double backup);
        int getInteger(const char* name, int backup);
        size_t getInteger(const char* name, size_t backup);
        std::string getString(const char* name, std::string backup);

        void setNumber(const char* name, double);
        void setInteger(const char* name, int);
        void setString(const char* name, std::string&);

        bool get();
        std::string& name();
    };
}
