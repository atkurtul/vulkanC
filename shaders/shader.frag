#version 450

layout(location = 0) in vec4 color;
layout(location = 0) out vec4 outColor;
layout(location = 1) in vec2 txx;
layout(set = 1, binding = 0) uniform sampler2D tex;

void main() {
    outColor = texture(tex, txx);
}