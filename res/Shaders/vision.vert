#version 430 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 offset;

uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);
uniform float scale = 10.0;

void main() {
    mat4 model = mat4(
    scale,    0.0,      0.0, 0.0,
    0.0,      scale,    0.0, 0.0,
    0.0,      0.0,      1.0, 0.0,
    offset.x, offset.y, 0.0, 1.0
    );

    gl_Position = projection * view * model * position;
}
