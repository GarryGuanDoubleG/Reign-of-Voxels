#version 450 core
in vec4 verts;
in vec4 in_color;

out vec4 vs_color;

uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp * verts;
	vs_color = in_color;	
}
