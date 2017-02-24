#version 400 core
layout (location = 0) in vec4 verts;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 verts_UV;

out vec4 vs_normal;
out vec4 FragPos;
//out vec2 UV;
//uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * model * verts;
	//UV = verts_UV;
	vs_normal = normal;	
	FragPos = vec4(model * verts);
}
