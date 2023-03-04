#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 center;
layout(location = 2) in vec2 size;
layout(location = 3) in vec4 color;

out vec4 v_color;

uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);

void main() {
    gl_Position = projection * view * vec4(position * size + center, 0.0, 1.0);
    v_color = color;
}
