#pragma once
#include "pch.hpp"
#include "Table.hpp"
#include "Function.hpp"

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

        int runBuffer(const unsigned char* buffer, size_t length);

        void addCommonLibraries();

        bool validate(int code, std::ostream& = std::cout);

        Table table(const char* name);
        Function function(const char*, int, int);

        void setGlobal(Table&);
        void setGlobal(std::string&);
        void setGlobal(const char*);

        void pushInteger(int);
        void pushInteger(size_t);
        void pushNumber(double);
        void pushString(std::string&);
        void pushString(const char*);
        void pop(int n);

        explicit operator lua_State*();
    };

    lua_State *raw(VirtualMachine &lvm);
}
