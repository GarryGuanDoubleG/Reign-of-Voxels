
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

//axis aligned plane
float Density_PlanarFunc(const glm::vec3 &worldPosition)
{
	const int PLANE = 10;

	const float terrain = Density_Func(worldPosition);
	const float terrain_plane = worldPosition.y - PLANE;

	//return  glm::min(terrain, glm::max(-terrain, terrain_plane));
	return terrain_plane;
}

float Density_Func(const glm::vec3& worldPosition)
{
	const float heightScale = 1.0f / VoxelOctree::maxHeight;
	float perlinMapVal = GetPerlinMapValue(worldPosition.x, worldPosition.z) / 256.0f;

	perlinMapVal = perlinMapVal == 0.0 ? 1.0f / 256.0f : perlinMapVal;

	const float terrain = worldPosition.y - (perlinMapVal * VoxelOctree::maxHeight);
	//return terrain;

	float cuboid = Cuboid(worldPosition, glm::vec3(32, 44, 32), glm::vec3(32.0f));


	return glm::max(-cuboid, terrain);
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
		float csgCube = Cuboid(worldPosition, csgOperationPos[i] + .5f, glm::vec3(.5f));
		postTerrain = glm::max(-csgCube, postTerrain);
	}

	return postTerrain;
}


float Density_Func(const glm::vec3& worldPosition, const glm::vec3 &csgOperationPos)
{
	const float cube = Cuboid(worldPosition, csgOperationPos + .5f, glm::vec3(.5f));

	//return glm::max(-cube, terrain);
	return cube;
}
