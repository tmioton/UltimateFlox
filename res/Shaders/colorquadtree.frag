#version 430 core
#define A 0.15
#define B 0.50
#define C 0.10
#define D 0.20
#define E 0.02
#define F 0.30
#define W 11.2

layout(std430, binding=0) buffer depthColors {
    vec4 colors[];
};

in VERTEX_DEPTH {
    flat int depth;
} fs_depth;

out vec4 final;

vec3 Uncharted2Tonemap(vec3 x) {
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() {
//    vec3 gamma = vec3(2.2);
    vec4 full_color = colors[fs_depth.depth];
//    vec3 color = pow(full_color.rgb, gamma);
    vec3 color = full_color.rgb;

//  ****** rec601 NTSC luma ******
//    float luma = (0.299 * color.r) + (0.587 * color.g) + (0.114 * color.b);

//  ****** ITU-R BT.709 HDTV luma ******
//    float luma = (0.2126 * color.r) + (0.7152 * color.g) + (0.0722 * color.b);

//  ****** ITU-R BT.2100 HDR luma ******
//    float luma = (0.2627 * color.r) + (0.6780 * color.g) + (0.0593 * color.b);
//    color = vec3(luma);

//    float exposureBias = 2.0;
//    vec3 curr = exposureBias * Uncharted2Tonemap(color);
//    vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));
//    vec3 ldr = curr * whiteScale;
//    float luma = (0.2627 * ldr.r) + (0.6780 * ldr.g) + (0.0593 * ldr.b);
//    ldr = vec3(luma);

//    vec3 ldr = color / (color + vec3(1.0));
//    final = vec4(pow(ldr, 1.0 / gamma), full_color.a);
    final = vec4(color, full_color.a);
//    final = full_color;
}
