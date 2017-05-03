#version 400 core
layout (location = 0) in vec3 verts;
layout (location = 1) in vec3 normal;
layout (location = 2) in int textureID;

out vec4 clipSpace;

//out vec2 UV;
//uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	clipSpace = projection * view  * vec4(verts, 1.0f);
	gl_Position = clipSpace;
}

