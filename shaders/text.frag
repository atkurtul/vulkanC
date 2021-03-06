#version 450

layout(location = 0) in vec2 txx;
layout(location = 0) out vec4 color;

layout(set = 1, binding = 0) uniform sampler2D tex;

void main() {
    color = texture(tex, txx);
}