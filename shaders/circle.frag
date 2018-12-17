#version 460

in vec4 vColor;
in vec2 center;
in vec2 bounds;
in float rad;
layout(location = 0) out vec4 fColor;

void main(void)
{
    vec2 pos = bounds - center;
    if (dot(pos, pos) > rad)
        discard;
    fColor = vColor;
}
