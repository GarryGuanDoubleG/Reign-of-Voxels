#version 400 core

in vec2 UV;
out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main(void)
{
	//color = vec4(texture(texture_diffuse1, UV));
	color = vec4(0.5, 0.5, 1.0, 1.0);
}
