
#include "VoxelOctree.hpp"
#include "VoxelNoise.hpp"
#include "density.hpp"

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


float Density_Func(const glm::vec3& worldPosition)
{
	const float heightScale = 1.0f / VoxelOctree::maxHeight;
	float perlinMapVal = GetPerlinMapValue(worldPosition.x, worldPosition.z) / 256.0f;

	perlinMapVal = perlinMapVal == 0.0 ? 1.0f / 256.0f : perlinMapVal;

	const float terrain = worldPosition.y - (perlinMapVal * VoxelOctree::maxHeight);
	
	const float cube = Cuboid(worldPosition, glm::ivec3(48), glm::ivec3(32, 32, 32));
	
	return glm::max(-cube, terrain);
}

float Density_Func(const glm::vec3& worldPosition, const std::vector<glm::vec3> &csgOperationPos)
{
	if (csgOperationPos.size() == 0)
		return Density_Func(worldPosition);
	if (!AABBContainsPoint(glm::vec3(1), glm::vec3(256 - 1), worldPosition))
		return 1;

	const float heightScale = 1.0f / VoxelOctree::maxHeight;
	float perlinMapVal = GetPerlinMapValue(worldPosition.x, worldPosition.z) / 256.0f;

	perlinMapVal = perlinMapVal == 0.0 ? 1.0f / 256.0f : perlinMapVal;

	//float terrain = worldPosition.y - 32.0f;
	const float terrain = worldPosition.y - (perlinMapVal * VoxelOctree::maxHeight);
	float postTerrain = terrain;

	float csgCube = -10000.0f;
	for (int i = 0; i < csgOperationPos.size(); i++)
	{
		csgCube = Cuboid(worldPosition, csgOperationPos[i], glm::vec3(1.0f));
		postTerrain = glm::max(-csgCube, postTerrain);
	}

	return postTerrain;
}
