target_sources(
    ${PROJECT_NAME}
    PRIVATE
        Application.cpp
        Common.hpp

        # ALGORITHM
        Algorithm/Algorithm.hpp
#        Algorithm/DirectLoopAlgorithm.hpp
#        Algorithm/DirectLoopAlgorithm.cpp
        Algorithm/QuadtreeAlgorithm.hpp
        Algorithm/QuadtreeAlgorithm.cpp

         MATH
        Math/Rectangle.hpp
        Math/Rectangle.cpp

        # RENDER
        Render/Object.hpp
        Render/Object.cpp
        Render/Controls.hpp
        Render/Controls.cpp
        Render/Models.hpp
        Render/Models.cpp
        Render/FlockRenderer.hpp
        Render/FlockRenderer.cpp
        Render/QuadtreeRenderer.hpp
        Render/QuadtreeRenderer.cpp

        # STRUCTURES
        Structures/DoubleBuffer.hpp
        Structures/Quadtree.hpp

        # WORLD
        World/Boid.hpp
        World/Boid.cpp
        World/Flock.hpp
        World/Flock.cpp
        World/World.hpp

        # CORE
        Core/Window.hpp
        Core/Window.cpp
        Core/Event.hpp
        Core/Event.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp pch.cpp)

# Link in glad and glfw libraries
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL32)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

# Link in LWVL
target_link_libraries(${PROJECT_NAME} PRIVATE lwvl)
