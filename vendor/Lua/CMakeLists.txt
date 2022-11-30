cmake_minimum_required(VERSION 3.1)
project(Lua VERSION 5.4.2 LANGUAGES C)

add_library(
    lua INTERFACE
    include/lua/lauxlib.h
    include/lua/lua.h
    include/lua/lua.hpp
    include/lua/luaconf.h
    include/lua/lualib.h
)

set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)
target_include_directories(lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lua54.lib")
