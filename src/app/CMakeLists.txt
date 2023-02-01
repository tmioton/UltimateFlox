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
        Structures/StringArray.hpp
        Structures/StringArray.cpp

        # WORLD
        World/Boid.hpp
        World/Boid.cpp
        World/Boidtree.hpp
        World/Flock.hpp
        World/Flock.cpp

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

        # PLATFORM
        Core/Platform/GLFW/imgui_impl_glfw.hpp
        Core/Platform/GLFW/imgui_impl_glfw.cpp
        Core/Platform/OpenGL/imgui_impl_opengl3.hpp
        Core/Platform/OpenGL/imgui_impl_opengl3.cpp
        Core/Platform/OpenGL/imgui_impl_opengl3_loader.hpp

        # WINDOW
        Core/Window/Window.hpp
        Core/Window/Window.cpp
        Core/Window/Event.hpp
        Core/Window/Event.cpp

        # USER INTERFACE
        Core/UserInterface/UserInterface.hpp
        Core/UserInterface/UserInterface.cpp
        Core/UserInterface/UIComponent.hpp
        Core/UserInterface/UIComponent.cpp
        Core/UserInterface/UIDemoWindow.hpp
        Core/UserInterface/UIDemoWindow.cpp
        Core/UserInterface/UIConsole.hpp
        Core/UserInterface/UIConsole.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp pch.cpp)

# Link in glad and glfw libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE ImGui)
target_link_libraries(${PROJECT_NAME} PRIVATE thread-pool)
target_link_libraries(${PROJECT_NAME} PRIVATE lua)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

# Link in LWVL
target_link_libraries(${PROJECT_NAME} PRIVATE lwvl)
