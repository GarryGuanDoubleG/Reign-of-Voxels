#include "VoxelBlock.hpp"
/**
* Constructor
* sets active flag to false and type to default
*/
VoxelBlock::VoxelBlock()
{
	m_active = false;
	m_voxelType = Default;
}

VoxelBlock::~VoxelBlock()
{

}
/**
* @brief returns active flag
* @return active flag
*/
bool VoxelBlock::IsActive()
{
	return m_active;
}
/**
* @brief sets the active flag
* @param active flag to set to
*/
void VoxelBlock::SetActive(bool active)
{
	m_active = active;
}

