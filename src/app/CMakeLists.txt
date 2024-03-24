target_sources(
    ${PROJECT_NAME}
    PRIVATE
        Application.cpp
        Common.hpp

        # WINDOW
        Core/Window/Window.hpp
        Core/Window/Window.cpp
        Core/Window/GLFWState.cpp
        Core/Window/GLFWStateEmpty.cpp
        Core/Window/Event.hpp
        Core/Window/Event.cpp

        # LUA
        Core/Lua/Common.hpp
        Core/Lua/Common.cpp
        Core/Lua/Function.hpp
        Core/Lua/Function.cpp
        Core/Lua/Lua.hpp
        Core/Lua/Lua.cpp
        Core/Lua/Table.hpp
        Core/Lua/Table.cpp
        Core/Lua/VirtualMachine.hpp
        Core/Lua/VirtualMachine.cpp
        Core/Lua/Errors.hpp
        Core/Lua/Errors.cpp

        # LUA TYPES
        Core/Lua/Types/LuaVector.hpp
        Core/Lua/Types/LuaVector.cpp
    PRIVATE FILE_SET CXX_MODULES FILES
        # ALGORITHM
        Algorithm/Algorithm.cppm
        Algorithm/ThreadedAlgorithm.cppm

        # MATH
        Math/Camera.cppm
        Math/Rectangle.cppm

        # RENDER
        Render/FlockRenderer.cppm
        Render/Geometry/Geometry.cppm
        Render/Geometry/QuadtreeGeometry.cppm
        Render/QuadtreeRenderer.cppm
        Render/RectangleRenderer.cppm

        # STRUCTURES
        Structures/DoubleBuffer.cppm
        Structures/Quadtree.cppm
        Structures/RawArray.cppm

        # WORLD
        World/Boid.cppm
        World/Boidtree.cppm
        World/Flock.cppm
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp pch.cpp)

# Link in glad and glfw libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE thread-pool)
target_link_libraries(${PROJECT_NAME} PRIVATE lua)

# Link in LWVL
target_link_libraries(${PROJECT_NAME} PRIVATE lwvl)
