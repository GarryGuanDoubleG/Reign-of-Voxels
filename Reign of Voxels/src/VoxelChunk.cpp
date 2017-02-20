#include "VoxelChunk.hpp"

VoxelChunk::VoxelChunk(int index, int size, Vec3 position)
{
	m_active = false;
	m_index = index;

	m_mooreRadius = size;
	m_size = Vec3(size, size, size);
	m_gridCoordinate = position;
	m_voxels = NULL;
}

VoxelChunk::~VoxelChunk()
{
	if (m_voxels)
		delete m_voxels;
}

Vec3 VoxelChunk::getSize()
{
	return m_size;
}
int VoxelChunk::getRadius()
{
	return m_mooreRadius;
}
bool VoxelChunk::isActive()
{
	return m_active;
}

void VoxelChunk::InsertVoxelAtPos(int x, int y, int z)
{
	for (int i = 0; i < y - m_gridCoordinate.y; i++)
	{

	}
}

Vec3 VoxelChunk::getPosition()
{
	return m_gridCoordinate;
}