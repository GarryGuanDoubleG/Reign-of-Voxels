#version 400 core
layout (location = 0) in vec4 verts;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 verts_UV;
layout (location = 3) in mat4 instanceMatrix;

out vec2 UV;
//uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * instanceMatrix * verts;	
	UV = verts_UV;	
}
