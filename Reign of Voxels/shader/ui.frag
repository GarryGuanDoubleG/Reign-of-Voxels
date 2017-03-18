#version 400 core
in vec2 UV;

out vec4 color;

uniform sampler2D hudTexture;

void main()
{
	color = texture(hudTexture, UV);
}