#include "Voxel.h"

Voxel::Voxel()
{
	m_active = false;
	m_voxelType = Default;

	//set vao
	if (!m_vao)
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	}
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

