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

	Vec3 getSize();
	int getRadius();
	bool isActive();
	void InsertVoxelAtPos(int x, int y, int z);
private:
	bool m_active;

	int m_index; /**<index into model matrix*/
	Vec3 m_size;
	int m_mooreRadius;

	Vec3 m_centerVoxel; /**<Voxel in the center */
	Vec3 m_gridCoordinate;

	Voxel *m_voxels;
};
