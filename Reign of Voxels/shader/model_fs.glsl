#version 400 core
in vec2 UV;
in vec4 vs_normal;
in vec4 FragPos;
out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main(void)
{
	//color = vec4((texture(texture_diffuse1, UV)) *lightColor);
	vec4 lightDir = normalize(vec4(lightPos,0.0f) - FragPos); 
	float diff = max(dot(vs_normal, lightDir), 0.0);

	float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColor;
	vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse)* objectColor;
	color = vec4(result, 1.0f);
	//color = vec4(0.5, 0.5, 1.0, 1.0);
}
