#version 400 core

layout (location = 0) in vec3 verts;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 worldPos;


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
	//vec3 CameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
	//vec3 CameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);

	//vec3 particleCenterWorldSpace = verts + worldPos + vec3(billboardSize.xy / 2, 0);


	//vec3 vertexPosWorldSpace = particleCenterWorldSpace + 
	//								CameraRight_worldspace * verts.x * billboardSize.x + 
	//								CameraUp_worldspace * verts.y * billboardSize.y;

	mat4 modelMat = model;
	modelMat[3][0] = worldPos.x;
	modelMat[3][1] = worldPos.y + 1;
	modelMat[3][2] = worldPos.z;
	modelMat[3][3] = 1.0f;

	mat4 modelView = view * modelMat;
	modelView[0][0] = 1.0; 
	modelView[0][1] = 0.0; 
	modelView[0][2] = 0.0; 


	// Thrid colunm.
	modelView[2][0] = 0.0; 
	modelView[2][1] = 0.0; 
	modelView[2][2] = 1.0; 

	vec4 P = modelView * vec4(verts, 1.0f);
	gl_Position = projection * P;
	
	UV = uv;
}

