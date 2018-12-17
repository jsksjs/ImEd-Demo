#version 460

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 offset;
out vec4 vColor;

uniform mat4 viewprojection;
uniform mat4 modelview;

void main(void)
{
    gl_Position = viewprojection * modelview * vec4(position.x + offset.x, position.y + offset.y, 0 + offset.z, 1);
    vColor = color;
}
