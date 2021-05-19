add_library(
    lwvl

    Buffer.hpp
    Common.hpp
    Common.cpp
    Framebuffer.hpp
    Framebuffer.cpp
    GLDebug.hpp
    GLDebug.cpp
    Shader.hpp
    Shader.cpp
    Texture.hpp
    Texture.cpp
    VertexArray.hpp
    VertexArray.cpp
    WorldBlock.hpp
    WorldBlock.cpp
)

target_include_directories(lwvl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(lwvl PRIVATE pch.hpp pch.cpp)

target_link_libraries(lwvl PRIVATE OpenGL32)
target_link_libraries(lwvl PRIVATE glad)
