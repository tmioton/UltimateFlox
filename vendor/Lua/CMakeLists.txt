cmake_minimum_required(VERSION 3.1)
project(Lua VERSION 5.4.4 LANGUAGES C)

add_library(
    lua INTERFACE
    include/lua/lauxlib.h
    include/lua/lua.h
    include/lua/lua.hpp
    include/lua/luaconf.h
    include/lua/lualib.h
)

add_compile_definitions(LUA_USE_WINDOWS)  # needed?
set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)
target_include_directories(lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (WIN32)
    target_link_libraries(lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Lua54.lib")
else()
    target_link_libraries(lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Lua54.a")
endif()
