#version 460

in vec4 vColor;
in vec4 pos;
layout(location = 0) out vec4 fColor;

void main(void)
{
    if(length(gl_FragCoord - pos) > 40)
        discard;
    fColor = vColor;
}
