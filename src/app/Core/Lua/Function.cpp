// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "Function.hpp"

lua::Function::Function(lua_State *L, std::string name, int nargs, int nres)
        : m_state(L), m_name(std::move(name)), m_args(nargs), m_results(nres)
{}

bool lua::Function::push() {
    lua_getglobal(m_state, m_name.c_str());
    if (lua_isfunction(m_state, -1)) {
        return true;
    } else {
        lua_pop(m_state, 1);
        return false;
    }
}

int lua::Function::call(int msgh) {
    return lua_pcall(m_state, m_args, m_results, msgh);
}

void lua::Function::pop() {
    lua_pop(m_state, 1);
}
