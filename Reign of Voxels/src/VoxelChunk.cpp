#include "VoxelChunk.hpp"

VoxelChunk::VoxelChunk(int index, int side, Vec3 position)
{
	m_index = index;
	m_side = side;
	m_gridCoordinate = position;
}

VoxelChunk::~VoxelChunk()
{

}

