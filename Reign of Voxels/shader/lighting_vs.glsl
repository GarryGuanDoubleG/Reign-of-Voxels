#version 400 core
layout (location = 0) in vec4 verts;
//uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * model * verts;

}
