out vec4 color;

in vec2 UV;

in vec3 vs_normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform vec3 viewPos;
uniform vec3 lightPos; 
uniform vec3 lightColor;

void main(void)
{
	//color = vec4((texture(texture_diffuse1, UV)) *lightColor);
	vec3 lightDir = normalize(lightPos - FragPos); 
	
	float specularStrength = 0.69f;
	vec3 norm = normalize(vs_normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

    vec3 result = (specular + ambient + diffuse) * vec3(.5f,.5f,.5f);
	color = vec4(result, 1.0f);
}