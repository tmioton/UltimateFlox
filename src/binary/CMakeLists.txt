target_sources(
    ${PROJECT_NAME} PRIVATE
    binary_default_lua.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
