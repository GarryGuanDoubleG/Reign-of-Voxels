#version 400 core

layout (location = 0) in vec3 verts;
layout (location = 1) in vec2 uv;

out vec2 UV;

//uniforms
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec2 billboardSize;

void main(void)
{
	vec4 worldPos = model * vec4(verts, 1.0f);
	//particleCenter_worldspace = model * vec4(verts,1.0f) + billboardSize.xy /2 ;

	mat4 modelMat = model;
	vec3 scale = vec3(model[0][0], model[1][1], model[2][2]);

	//modelMat[3][0] = worldPos.x;
	//modelMat[3][1] = worldPos.y;
	//modelMat[3][2] = worldPos.z;
	//modelMat[3][3] = 1.0f;

	mat4 modelView = view * model;
	modelView[0][0] = scale.x; 
	modelView[0][1] = 0.0; 
	modelView[0][2] = 0.0; 

	// Thrid colunm.
	modelView[2][0] = 0.0; 
	modelView[2][1] = 0.0; 
	modelView[2][2] = scale.z; 

	vec4 P = modelView * vec4(verts, 1.0f);
	gl_Position = projection * P;
	

	UV = uv;

}

