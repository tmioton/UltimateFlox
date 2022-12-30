#version 430 core
#define A 0.15
#define B 0.50
#define C 0.10
#define D 0.20
#define E 0.02
#define F 0.30
#define W 11.2

out vec4 final;
uniform vec3 color = vec3(A, 1.0, 1.0);

vec3 Uncharted2Tonemap(vec3 x) {
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() {
//    vec3 gamma = vec3(2.2);
//    float exposureBias = 2.0;
//    vec3 curr = exposureBias * Uncharted2Tonemap(pow(color, gamma));
//    vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));
//    vec3 ldr = curr * whiteScale;
//    vec3 ldr = color / (color + vec3(1.0));
//    final = vec4(pow(ldr, 1.0 / gamma), 1.0);
    final = vec4(color, 1.0);
}
