
#include <glm/ext.hpp>

#include "density.hpp"
#include "VoxelNoise.hpp"

float Sphere(const glm::vec3& worldPosition, const glm::vec3& origin, float radius)
{
	return glm::length(worldPosition - origin) - radius;
}



float Cuboid(const glm::vec3& worldPosition, const glm::vec3& origin, const glm::vec3& halfDimensions)
{
	const glm::vec3& local_pos = worldPosition - origin;
	const glm::vec3& pos = local_pos;

	const glm::vec3& d = glm::abs(pos) - halfDimensions;
	const float m = glm::max(d.x, glm::max(d.y, d.z));
	return glm::min(m, glm::length(max(d, glm::vec3(0.f))));
}

float FractalNoise(
	const int octaves,
	const float frequency,
	const float lacunarity,
	const float persistence,
	const glm::vec2& position)
{
	const float SCALE = 1.f / 128.f;
	glm::vec2 p = position * SCALE;
	float noise = 0.f;

	float amplitude = 1.f;
	p *= frequency;

	for (int i = 0; i < octaves; i++)
	{
		noise += glm::simplex(p) * amplitude;
		p *= lacunarity;
		amplitude *= persistence;
	}

	// move into [0, 1] range
	return 0.5f + (0.5f * noise);
}

float Density_Func(const glm::vec3& worldPosition)
{
	const float MAX_HEIGHT = 32.0f;
	const float heightScale = 1.0f / MAX_HEIGHT;
	const float perlinMapVal = GetPerlinMapValue(worldPosition.x, worldPosition.z) / 256.0f + .01f;

	const float terrain = worldPosition.y - (perlinMapVal * MAX_HEIGHT);

	return terrain;
}