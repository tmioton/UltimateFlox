#pragma once
#include "pch.hpp"
#include "Table.hpp"

namespace lua {
    class VirtualMachine {
        lua_State *m_state;

        static std::ostream &type(std::ostream &, int);

        static std::ostream &status(std::ostream &, int);

    public:
        VirtualMachine();

        ~VirtualMachine();

        int runString(const std::string &command);

        int runFile(const std::string &file);

        void addCommonLibraries();

        bool validate(int code, std::ostream& = std::cout);

        Table table(const char* name);
        void setGlobal(Table&);
        void pop(int n);

        explicit operator lua_State*();
    };

    lua_State *raw(VirtualMachine &lvm);
}
