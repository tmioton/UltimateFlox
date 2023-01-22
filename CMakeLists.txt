cmake_minimum_required(VERSION 3.17)
project(UltimateFlox VERSION 1.5.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
#if (MSVC)
#    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
##    set(BUILD_SHARED_LIBS TRUE)
#endif()

add_executable(${PROJECT_NAME})
#if (CMAKE_BUILD_TYPE MATCHES "Debug")
#    add_executable(${PROJECT_NAME})
#else()
#    add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE)
#endif()

# Mark the project as C++ 20 and set the output directory.
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO

    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
)

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(res)
