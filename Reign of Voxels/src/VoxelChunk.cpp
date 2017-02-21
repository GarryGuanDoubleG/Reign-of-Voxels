#include "VoxelChunk.hpp"

//const int VoxelChunk::chunkSize = 16;

VoxelChunk::VoxelChunk(int index,  Vec3 position)
{
	m_active = false;
	m_index = index;
	
	m_position = position;
}

VoxelChunk::~VoxelChunk()
{
	if (m_voxels)
		delete m_voxels;
}

int VoxelChunk::getSize()
{
	return chunkSize;
}

bool VoxelChunk::isActive()
{
	return m_active;
}

void VoxelChunk::InsertVoxelAtPos(int x, int y, int z)
{

}

Vec3 VoxelChunk::getPosition()
{
	return m_position;
}