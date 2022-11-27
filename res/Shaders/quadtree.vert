#version 330 core

layout(location = 0) in vec4 positions;
uniform mat4 projection = mat4(1.0);

void main() {
    gl_Position = projection * positions;
}
