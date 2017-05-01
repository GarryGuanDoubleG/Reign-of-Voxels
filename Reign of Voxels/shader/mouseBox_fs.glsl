#version 400 core
in vec2 UV;
out vec4 color;

uniform sampler2D screenTexture;
uniform sampler2D alphaMask;
uniform sampler2D heightmap;


void main()
{ 
    vec4 minimap = texture(screenTexture, UV);

	float alpha = texture(alphaMask, UV).r;
	float height = texture(heightmap, UV).r;

	float topology_line = step(.03125, mod(height, .125));

	vec4 topologyColor = topology_line * vec4(1, 0,0,0);
	vec4 minimapColor = step(.5f, topology_line) * minimap + topology_line * topologyColor;

	color = vec4(minimapColor.rgb, alpha);
}