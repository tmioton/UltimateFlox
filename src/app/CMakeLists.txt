target_sources(
    flox
    PRIVATE
        Application.cpp
        Flock.hpp
        Flock.cpp

        # CORE
        Core/Window.hpp
        Core/Window.cpp
        Core/Event.hpp
        Core/Event.cpp

        # MATH
)

target_include_directories(flox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(flox PRIVATE pch.hpp pch.cpp)

# Link in glad and glfw libraries
target_link_libraries(flox PRIVATE OpenGL32)
target_link_libraries(flox PRIVATE glad)
target_link_libraries(flox PRIVATE glfw)
target_link_libraries(flox PRIVATE glm)

# Link in LWVL
target_link_libraries(flox PRIVATE lwvl)
