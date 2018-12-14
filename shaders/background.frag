#version 460

in vec2 uv;

out vec4 color;

uniform sampler2D sampler;

void main(void)
{
    color = texture(sampler, uv);
}
