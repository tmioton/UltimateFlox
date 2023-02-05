target_sources(
    ${PROJECT_NAME} PRIVATE
        Application.cpp
        Common.hpp

        # ALGORITHM
        Algorithm/Algorithm.hpp
#        Algorithm/DirectLoopAlgorithm.hpp
#        Algorithm/DirectLoopAlgorithm.cpp
#        Algorithm/QuadtreeAlgorithm.hpp
#        Algorithm/QuadtreeAlgorithm.cpp
        Algorithm/ThreadedAlgorithm.hpp
        Algorithm/ThreadedAlgorithm.cpp
        Algorithm/ThreadWork.cpp

        # MATH
        Math/Rectangle.hpp
        Math/Rectangle.cpp

        # RENDER
        Render/Boid/Object.hpp
        Render/Boid/Object.cpp
        Render/Boid/Controls.hpp
        Render/Boid/Controls.cpp
        Render/Boid/Models.hpp
        Render/Boid/Models.cpp
        Render/Boid/FlockRenderer.hpp
        Render/Boid/FlockRenderer.cpp
        Render/Geometry/Geometry.hpp
        Render/Geometry/QuadtreeGeometry.hpp
        Render/Geometry/QuadtreeGeometry.cpp
        Render/QuadtreeRenderer.hpp
        Render/QuadtreeRenderer.cpp

        # STRUCTURES
        Structures/DoubleBuffer.hpp
        Structures/Quadtree.hpp
        Structures/RawArray.hpp

        # WORLD
        World/Boid.hpp
        World/Boid.cpp
        World/Boidtree.hpp
        World/Flock.hpp
        World/Flock.cpp

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
