#version 460

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 offset;
out vec4 vColor;
out vec4 pos;

uniform mat4 modelviewprojection;

void main(void)
{
    gl_Position = modelviewprojection * vec4(position.x + offset.x, position.y + offset.y, 0 + offset.z, 1);
    vColor = color;
    pos = modelviewprojection * vec4(position.x, position.y, 0 + offset.z, 1);
}
