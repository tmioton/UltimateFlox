// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once
#include "pch.hpp"
#ifndef NDEBUG
#define LUA_DEBUG
#endif

// Taken from http://lua-users.org/wiki/ErrorHandlingBetweenLuaAndCplusplus
namespace lua {
    inline void error(lua_State* L, const char* message) {
        lua_pushstring(L, message);
        lua_error(L);
    }

    std::ostream &type(std::ostream &_t, int type);

    std::ostream &status(std::ostream &_s, int status);

    // Concepts example
    //template<typename T>
    //concept Processor = requires(T p, lua_State* L) {
    //    { p.argument_count } -> std::convertible_to<int>;
    //    { p.result_count } -> std::convertible_to<int>;
    //    p.pre_process(L);
    //    p.post_process(L);
    //};

    template<typename T>
    concept Code = requires(T c, lua_State* L) {
        { c.run(L) } -> std::convertible_to<int>;
    };

    struct CodeFile {
        explicit CodeFile(std::string);
        explicit CodeFile(const char*);
        int run(lua_State*);

        std::string filename;
    };

    // Can add a concept-base to control whether this manages the buffer.
    struct CodeBuffer {
        CodeBuffer(const char*, std::size_t);
        int run(lua_State*);

        const char* buffer;
        std::size_t size; // Length and size are synonyms when used with an array of bytes.
    };
}
