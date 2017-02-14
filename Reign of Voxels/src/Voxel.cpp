#include "Voxel.hpp"
/**
* Constructor
* sets active flag to false and type to default
*/
Voxel::Voxel()
{
	m_active = false;
	m_voxelType = Default;
}

Voxel::~Voxel()
{

}
/**
* @brief returns active flag
* @return active flag
*/
bool Voxel::IsActive()
{
	return m_active;
}
/**
* @brief sets the active flag
* @param active flag to set to
*/
void Voxel::SetActive(bool active)
{
	m_active = active;
}

