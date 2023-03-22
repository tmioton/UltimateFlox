// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once
#include "pch.hpp"


namespace lua {
    class VirtualMachine;

    class Function {
        friend VirtualMachine;

        Function(lua_State*, std::string, int, int);
    public:
        Function(Function const&) = default;
        Function(Function&&) = default;

        Function &operator=(Function const&) = default;
        Function &operator=(Function&&) = default;

        bool push();
        void pop();
        int call(int idxMsgHandler = 0);
    private:
        lua_State* m_state;
        std::string m_name;
        int m_args;
        int m_results;
    };
}
