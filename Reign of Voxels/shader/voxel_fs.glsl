#version 400 core
in vec2 UV;
out vec4 color;

void main(void)
{
	//color = vec4(texture(texture_diffuse1, UV));
	color = vec4(0.5, 0.5, 1.0, 1.0);
}
