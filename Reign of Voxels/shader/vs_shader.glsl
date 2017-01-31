#version 450 core
layout (location = 0) in vec4 verts;
layout (location = 1) in vec2 verts_UV;

out vec2 UV;
//uniforms
uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp * verts;
	
	UV = verts_UV;	
}
