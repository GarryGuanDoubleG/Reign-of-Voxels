#include "Voxel.h"

Voxel::Voxel()
{
	m_active = false;
	m_voxelType = Default;
}

Voxel::~Voxel()
{

}

bool Voxel::IsActive()
{
	return m_active;
}

void Voxel::SetActive(bool active)
{
	m_active = active;
}

