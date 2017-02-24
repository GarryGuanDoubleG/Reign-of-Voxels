#include "VoxelChunk.hpp"

//const int VoxelChunk::chunkSize = 16;

VoxelChunk::VoxelChunk(Vec3 position)
{
	m_active = false;	
	m_position = position;

	glGenVertexArrays(1, &this->m_vao);
	glGenBuffers(1, &this->m_vbo);
	glGenBuffers(1, &this->m_ebo);
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

void VoxelChunk::SetActive(bool active)
{
	m_active = active;
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

void VoxelChunk::AddTrianglesIndices()
{
	m_tri_indices.push_back(m_vertices.size() - 4);
	m_tri_indices.push_back(m_vertices.size() - 3);
	m_tri_indices.push_back(m_vertices.size() - 2);
	m_tri_indices.push_back(m_vertices.size() - 4);
	m_tri_indices.push_back(m_vertices.size() - 2);
	m_tri_indices.push_back(m_vertices.size() - 1);
}

void VoxelChunk::GenerateMesh()
{
	if (!m_active)
		return;
	for (int x = 0; x < chunkSize; x++)
	{
		for (int y = 0; y < chunkSize; y++)
		{
			for (int z = 0; z < chunkSize; z++)
			{
				if (m_voxels[x][y][z].IsActive() == false)
					continue;
				if ((x > 0 && !m_voxels[x - 1][y][z].IsActive()) || x == 0)
				{
					m_vertices.push_back(Vec3(x - 0.5f, y - 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y + 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y + 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y - 0.5f, z - 0.5f));
					AddTrianglesIndices();
				}

				if ((x < chunkSize - 1 && !m_voxels[x + 1][y][z].IsActive()) || x == chunkSize - 1)
				{
					m_vertices.push_back(Vec3(x + 0.5f, y - 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y + 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y + 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y - 0.5f, z + 0.5f));
					AddTrianglesIndices();
				}				
				if ((y > 0 && !m_voxels[x][y - 1][z].IsActive()) || y == 0)
				{
					m_vertices.push_back(Vec3(x - 0.5f, y - 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y - 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y - 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y - 0.5f, z + 0.5f));
					AddTrianglesIndices();
				}
				if ((y < chunkSize - 1 && !m_voxels[x][y + 1][z].IsActive()) || y == chunkSize - 1)
				{
					m_vertices.push_back(Vec3(x - 0.5f, y + 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y + 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y + 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y + 0.5f, z - 0.5f));
					AddTrianglesIndices();
				}

				if ((z > 0 && !m_voxels[x][y][z - 1].IsActive()) || z == 0)
				{
					m_vertices.push_back(Vec3(x - 0.5f, y - 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y + 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y + 0.5f, z - 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y - 0.5f, z - 0.5f));
					AddTrianglesIndices();
				}
				if ((z < chunkSize - 1 && !m_voxels[x][y][z + 1].IsActive()) || z == chunkSize - 1)
				{
					m_vertices.push_back(Vec3(x + 0.5f, y - 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x + 0.5f, y + 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y + 0.5f, z + 0.5f));
					m_vertices.push_back(Vec3(x - 0.5f, y - 0.5f, z + 0.5f));
					AddTrianglesIndices();
				}

			}
		}
	}

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vec3), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_tri_indices.size() * sizeof(GLuint), &m_tri_indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (GLvoid*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VoxelChunk::SetVoxelActive(int x, int y, int z)
{
	m_voxels[x - (int)m_position.x][y - (int)m_position.y][z - (int)m_position.z].SetActive(true);//convert world coor to local
}

Vec3 VoxelChunk::getPosition()
{
	return m_position;
}

void VoxelChunk::Render()
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_tri_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}