#version 400 core
layout (location = 0) in vec3 verts;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 verts_UV;

uniform vec2 position;
uniform vec2 scale;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	//vec4 new_verts = vec4(verts + vec3(position, 0.0f), 1.0f);

	//mat4 new_model = model;
	//new_model[0][0] *= scale.x;
	//new_model[1][1] *= scale.y;

	//gl_Position = projection * view * new_model * new_verts;
	//gl_Position = projection * view * model * ( vec4(scale,1.0f,1.0f) * vec4(verts, 1.0f));
	gl_Position = projection * view * model * vec4(verts, 1.0f);

}
