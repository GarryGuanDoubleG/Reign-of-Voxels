#version 400 core

in vec3 FragCoord;

out vec4 color;

uniform sampler2D alphaMask;

void main(void)
{
	float intensity =  2.0 + FragCoord.y / 16.0;

	color = vec4(1.0f, 0.0, 0.0f, intensity);
}
