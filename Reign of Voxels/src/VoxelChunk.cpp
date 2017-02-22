#include "VoxelChunk.hpp"

//const int VoxelChunk::chunkSize = 16;

VoxelChunk::VoxelChunk(Vec3 position)
{
	m_active = false;	
	m_position = position;
}

VoxelChunk::~VoxelChunk()
{

}

int VoxelChunk::getSize()
{
	return chunkSize;
}

bool VoxelChunk::isActive()
{
	return m_active;
}

void VoxelChunk::GenerateMesh()
{

}

void VoxelChunk::InsertVoxelAtPos(int x, int y, int z)
{
	for (int i = 0; i < chunkSize; i++)
	{
		if (i <= y - (int)m_position.y)
		{
			m_voxels[x - (int)m_position.x][i][z - (int)m_position.z].SetActive(true);//convert world coor to local
		}
	}
}

Vec3 VoxelChunk::getPosition()
{
	return m_position;
}

void VoxelChunk::Render()
{

}