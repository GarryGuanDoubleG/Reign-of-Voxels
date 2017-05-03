#version 400 core

in vec4 clipSpace;

in vec3 vs_normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;


vec3 getTriPlanarBlend(vec3 _wNorm){
	// in wNorm is the world-space normal of the fragment
	vec3 blending = abs( _wNorm );
	blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	return blending;
}

void main(void)
{
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;

	vec2 refractUV = ndc;
	vec2 reflectUV = vec2(ndc.x, -ndc.y);
	
	vec4 reflectColor = texture(reflectionTex, reflectUV);
	vec4 refractColor = texture(refractionTex, refractUV);

	color = mix(reflectColor, refractColor, 0.15);
	color.a = 1.0f;
}