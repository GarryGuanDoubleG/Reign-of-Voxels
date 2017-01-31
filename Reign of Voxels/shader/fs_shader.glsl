#version 450 core

in vec2 UV;
out vec4 color;

uniform sampler2D myTextureSampler;

void main(void)
{
	color = texture(myTextureSampler, UV);
}
