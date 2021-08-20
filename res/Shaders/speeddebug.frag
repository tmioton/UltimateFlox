#version 410 core
#define pi 3.1415926538

layout(location = 1) in vec2 velocity;
out vec4 final;

uniform float maxSpeed;
uniform vec3 color = vec3(1.0, 1.0, 1.0);

void main() {
    // ****** Color change based on rotation. ******
    //    float a = atan(rotation.y, rotation.x) / pi;
    //    if (a < 0.0) {
    //        a = -a;
    //    }
    //    final = vec4(mix(vec3(0.91765, 0.88235, 0.31765), color, a), 1.0);

    // ****** Color change based on speed. ******
    float a = length(velocity) / maxSpeed;
    final = vec4(mix(vec3(0.91765, 0.88235, 0.31765), color, a), 1.0);
}
