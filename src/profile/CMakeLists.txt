if (NOT MSVC)  # szc, coz-profiler gcc/clang only
    add_executable(
        profile-${CMAKE_PROJECT_NAME}
        main.cpp
    )
endif ()
