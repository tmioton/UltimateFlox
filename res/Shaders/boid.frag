#version 410 core
#define pi 3.1415926538

layout(location = 0) in vec2 rotation;
out vec4 final;

uniform vec3 color = vec3(1.0, 1.0, 1.0);

void main() {
    float a = atan(rotation.y, rotation.x) / pi;
    if (a < 0.0) {
        a = -a;
    }

//    final = vec4(mix(vec3(0.91765, 0.88235, 0.31765), color, a), 1.0);
    final = vec4(color, 1.0);
}
