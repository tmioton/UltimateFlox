add_executable(
    flox Application.cpp

    # CORE
    Core/Window.hpp
    Core/Window.cpp
    Core/Event.hpp
    Core/Event.cpp
)

target_include_directories(flox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(flox PRIVATE pch.hpp pch.cpp)

# Link in glad and glfw libraries
target_link_libraries(flox PRIVATE OpenGL32)
target_link_libraries(flox PRIVATE glad)
target_link_libraries(flox PRIVATE glfw)

# Link in LWVL
target_link_libraries(flox PRIVATE lwvl)

# Mark the project as C++ 17 and set the output directory.
set_target_properties(
    flox PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO

    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
)

# Add shaders here
add_custom_target(
    shaders ALL
    DEPENDS

    ${CMAKE_BINARY_DIR}/bin/Data/Shaders/boid.vert
    ${CMAKE_BINARY_DIR}/bin/Data/Shaders/boid.frag
)
add_dependencies(flox shaders)

add_custom_command(
    DEPENDS
    ${CMAKE_SOURCE_DIR}/res/Shaders/boid.vert
    ${CMAKE_SOURCE_DIR}/res/Shaders/boid.frag

    OUTPUT
    ${CMAKE_BINARY_DIR}/bin/Data/Shaders/boid.vert
    ${CMAKE_BINARY_DIR}/bin/Data/Shaders/boid.frag

    COMMENT "Copied shader folder to output"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res/Shaders/
    ${CMAKE_BINARY_DIR}/bin/Data/Shaders/
)
