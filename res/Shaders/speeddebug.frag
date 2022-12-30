#version 430 core
#define pi 3.1415926538

layout(location = 0) in float speed;
out vec4 final;

uniform float maxSpeed;
uniform vec3 topColor = vec3(0.19216, 0.91373, 0.50588);
uniform vec3 midColor = vec3(1.00000, 0.87843, 0.40000);
uniform vec3 lowColor = vec3(0.76471, 0.04314, 0.30588);

void main() {
    // ****** Color change based on rotation. ******
    //    float a = atan(rotation.y, rotation.x) / pi;
    //    if (a < 0.0) {
    //        a = -a;
    //    }
    //    final = vec4(mix(vec3(0.91765, 0.88235, 0.31765), color, a), 1.0);

    // ****** Color change based on speed. ******
    float speedPercent = speed / maxSpeed;
    float a;
    vec3 mixedColor;
    if (speedPercent >= 0.5) {
        a = speedPercent * 2 - 1;
        mixedColor = mix(midColor, topColor, a);
//        mixedColor = mix(vec3(0.91765, 0.88235, 0.31765), vec3(0.19216, 0.91373, 0.50588), a);
    } else {
        a = speedPercent * 2;
//        mixedColor = mix(color, vec3(0.91765, 0.88235, 0.31765), a);
        mixedColor = mix(lowColor, midColor, a);
    }

    final = vec4(mixedColor, 1.0);
}
