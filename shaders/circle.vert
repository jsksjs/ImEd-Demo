#version 460

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 offset;
out vec4 vColor;
out vec2 center;
out vec2 bounds;
out float rad;

uniform mat4 viewprojection;
uniform mat4 modelview;

void main(void)
{
    bounds = vec2(position.x + offset.x, position.y + offset.y);
    gl_Position = viewprojection * modelview * vec4(position.x + offset.x, position.y + offset.y, 0 + offset.z, 1);
    vColor = color;
    center = position;
    rad = pow(abs(offset.x), 2);
}
