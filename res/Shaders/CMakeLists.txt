# Handle watching and exporting shader files.
set(
    SHADER_SRCS

    # List shader files to watch and export below.
    boid.vert
    boid.frag
)

list(TRANSFORM SHADER_SRCS PREPEND "${CMAKE_BINARY_DIR}/bin/Data/Shaders/" OUTPUT_VARIABLE SHADERS_OUT)
list(TRANSFORM SHADER_SRCS PREPEND "${CMAKE_SOURCE_DIR}/res/Shaders/" OUTPUT_VARIABLE SHADERS_IN)

# A custom target that watches the shader files for changes.
add_custom_target(. ALL DEPENDS ${SHADERS_OUT})
add_dependencies(flox .)

add_custom_command(
    DEPENDS ${SHADERS_IN}
    OUTPUT ${SHADERS_OUT}

    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Data/Shaders/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${SHADERS_IN}
    ${CMAKE_BINARY_DIR}/bin/Data/Shaders/

    # A nice output message to show whether this command had to be run.
    COMMENT "Copied shaders to output"
)
