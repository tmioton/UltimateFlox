#version 430 core

out vec4 final;

uniform vec3 color = vec3(1.0, 1.0, 1.0);
uniform float alpha = 1.0;

void main() {
    final = vec4(color, alpha);
}
