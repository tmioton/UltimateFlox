cmake_minimum_required(VERSION 3.20)
cmake_policy(VERSION 3.20)

set(IMGUI_VERSION "1.92.2")
project(imgui VERSION ${IMGUI_VERSION} LANGUAGES CXX)
message(STATUS "Dear ImGui: Version " ${IMGUI_VERSION})

set(IMGUI_SRC_ROOT "${CMAKE_CURRENT_LIST_DIR}/src")
set(IMGUI_H_ROOT "${CMAKE_CURRENT_LIST_DIR}/include/imgui")

add_library(
    imgui
    include/imgui/imgui.h
    include/imgui/imconfig.h
    include/imgui/imgui_internal.h
    include/imgui/imstb_rectpack.h
    include/imgui/imstb_textedit.h
    include/imgui/imstb_truetype.h

    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
