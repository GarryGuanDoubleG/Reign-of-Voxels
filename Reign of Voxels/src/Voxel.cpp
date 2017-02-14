#include "Voxel.h"

Model * Voxel::m_voxel;

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

