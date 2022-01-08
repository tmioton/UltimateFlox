add_library(
    lwvl

    include/lwvl/lwvl.hpp
    src/Common.cpp
    src/Framebuffer.cpp
    src/Debug.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/VertexArray.cpp
    src/WorldBlock.cpp
)

target_include_directories(lwvl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(lwvl PRIVATE OpenGL32)
target_link_libraries(lwvl PRIVATE glad)
