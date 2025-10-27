target_sources(
    ${PROJECT_NAME}
    PRIVATE
        Application.cpp

        # EXT
        ext/imgui_demo.cpp
        ext/imgui_impl_glfw.cpp
        ext/imgui_impl_glfw.h
        ext/imgui_impl_opengl3.cpp
        ext/imgui_impl_opengl3.h
        ext/imgui_impl_opengl3_loader.h
    PRIVATE FILE_SET CXX_MODULES FILES
        # CORE
        core/input/Event.cppm
        core/input/Input.cppm
        core/Window.cppm
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp pch.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
