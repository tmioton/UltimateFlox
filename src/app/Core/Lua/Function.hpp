#pragma once
#include "pch.hpp"


namespace lua {
    class Function {
        lua_State* m_state;
        std::string m_name;
        int m_args;
        int m_results;
    public:
        Function(lua_State*, std::string, int, int);
        Function(Function const&) = default;
        Function(Function&&) = default;

        Function &operator=(Function const&) = default;
        Function &operator=(Function&&) = default;

        bool push();
        void pop();
        int call(int idxMsgHandler = 0);
    };
}
