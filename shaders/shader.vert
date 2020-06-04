#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 col;
layout(location = 0) out vec4 color;
layout(location = 1) out vec2 txx;

layout(set = 0, binding = 0) uniform _00_ { mat4 prj; } cam;

void main() {
    gl_Position =  cam.prj * vec4(pos, 0, 1);
    color = vec4(col, 1);
    txx = pos;
}