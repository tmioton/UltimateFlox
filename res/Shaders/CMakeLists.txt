# Handle watching and exporting shader files.
set(SHADER_SRCS

    # List shader files to watch and export below.
    boid.vert
    boid.frag
)

set(SHADER_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Data/Shaders/")
list(TRANSFORM SHADER_SRCS PREPEND ${SHADER_OUTPUT_DIRECTORY} OUTPUT_VARIABLE SHADERS_OUT)
list(TRANSFORM SHADER_SRCS PREPEND "${CMAKE_SOURCE_DIR}/res/Shaders/" OUTPUT_VARIABLE SHADERS_IN)

# A custom target that watches the shader files for changes.
add_custom_target(shaders ALL DEPENDS ${SHADERS_OUT})
add_dependencies(flox shaders)

add_custom_command(
    # This sees that the shader files are connected to a target
    # and runs this command only if the files have changed.
    DEPENDS ${SHADERS_IN}
    OUTPUT ${SHADERS_OUT}

    # Make the output directory to put shader files into.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIRECTORY}

    # Copy the shader source files to the output directory.
    COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS_IN} ${SHADER_OUTPUT_DIRECTORY}

    # A nice output message to show whether this command had to be run.
    COMMENT "Copied shaders to output"
)
