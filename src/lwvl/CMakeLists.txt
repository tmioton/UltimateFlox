cmake_minimum_required(VERSION 3.17)
project(LWVL VERSION 2.0.0)

add_library(
    lwvl

    include/lwvl/lwvl.hpp
    src/Buffer.cpp
    src/Common.cpp
    src/Framebuffer.cpp
    src/Debug.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/VertexArray.cpp
#    src/WorldBlock.cpp
)

target_include_directories(lwvl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/extern")
target_include_directories(lwvl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(lwvl PUBLIC glad)
