#version 410 core
#define pi 3.1415926538

layout(location = 0) in vec2 velocity;
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
    float speedPercent = length(velocity) / maxSpeed;
    float a;
    vec3 mixedColor;
    if (speedPercent >= 0.5) {
        a = speedPercent * 2 - 1;
        mixedColor = mix(vec3(0.91765, 0.88235, 0.31765), vec3(0.19216, 0.91373, 0.50588), a);
//        mixedColor = mix(vec3(0.91765, 0.88235, 0.31765), color, a);
    } else {
        a = speedPercent * 2;
        mixedColor = mix(color, vec3(0.91765, 0.88235, 0.31765), a);
//        mixedColor = mix(vec3(0.19216, 0.91373, 0.50588), vec3(0.91765, 0.88235, 0.31765), a);
    }

    final = vec4(mixedColor, 1.0);
}
