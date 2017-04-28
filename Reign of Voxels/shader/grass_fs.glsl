#version 400 core

in vec2 UV;

out vec4 color;

uniform sampler2D grassTexture;

void main(void)
{
	color = vec4(texture(grassTexture, UV));
}