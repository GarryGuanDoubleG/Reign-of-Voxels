#include "VoxelChunk.hpp"

VoxelChunk::VoxelChunk(int index, int mooreRadius, Vec3 position)
{
	m_index = index;
	m_radius = mooreRadius;
	m_gridCoordinate = position;
}

VoxelChunk::~VoxelChunk()
{

}

