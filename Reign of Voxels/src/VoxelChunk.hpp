#pragma once
#include "Voxel.hpp"
#include "glm.hpp"

class VoxelChunk
{
public:
	VoxelChunk(int index, int mooreRadius, Vec3 position);
	~VoxelChunk();

	Vec3 getPosition();
	Vec3 getCenterPos();

private:
	int m_index; /**<index into model matrix*/
	int m_side;

	Vec3 m_centerVoxel; /**<Voxel in the center */
	Vec3 m_gridCoordinate;
};
