#version 430 core

layout(location = 0) in vec2 positions;
layout(location = 1) in int depth;

uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);

out VERTEX_DEPTH {
    flat int depth;
} vs_depth;

void main() {
    gl_Position = projection * view * vec4(positions, 0.0, 1.0);
    vs_depth.depth = depth;
}
