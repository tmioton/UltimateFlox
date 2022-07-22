#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 offset;
layout(location = 2) in vec2 velocity;

uniform mat4 projection = mat4(1.0);
uniform float scale = 10.0;

layout(location = 0) out float v_VelocityLength;

void main() {
    float velocityLength = length(velocity);
    vec2 rotation = velocity / velocityLength;
    v_VelocityLength = velocityLength;

    mat4 model = mat4(
        scale * rotation.x,  scale * rotation.y, 0.0, 0.0,
        -scale * rotation.y, scale * rotation.x, 0.0, 0.0,
        0.0,                 0.0,                1.0, 0.0,
        offset.x,            offset.y,           0.0, 1.0
    );

    gl_Position = projection * model * position;
}
