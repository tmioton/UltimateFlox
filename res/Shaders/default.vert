#version 430 core

layout(location = 0) in vec4 position;
uniform mat4 projection = mat4(1.0);
uniform vec3 offset = vec3(0.0);
uniform float scale = 1.0f;

void main() {
    mat4 model = mat4(
        scale, 0.0, 0.0, 0.0,
        0.0, scale, 0.0, 0.0,
        0.0, 0.0, scale, 0.0,
        offset.x, offset.y, offset.z, 1.0
    );

    gl_Position = projection * model * position;
}