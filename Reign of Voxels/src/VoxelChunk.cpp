#include "VoxelChunk.hpp"
#include "model.hpp"


VoxelChunk::VoxelChunk() : m_flag(0)
{

}

void VoxelChunk::Init(Vec3 position)
{
	if (m_flag & CHUNK_FLAG_INUSE)
		std::cout << "Error already using chunk" << std::endl;

	m_flag = CHUNK_FLAG_INUSE;

	m_position = position;

	memset(m_voxels, VOXEL_TYPE_AIR, sizeof(m_voxels));

	std::vector<GLuint>empty_indices;
	m_tri_indices  = empty_indices;

	std::vector<VoxelVertex> empty_vertices;
	m_vertices = empty_vertices;
}

void VoxelChunk::Destroy()
{
	m_flag = 0;
}


Vec3 VoxelChunk::getSize()
{
	return Vec3(CHUNK_SIZE);
}

bool VoxelChunk::isActive()
{
	return m_flag & CHUNK_FLAG_ACTIVE;
}

void VoxelChunk::SetActive(bool active)
{
	if (active)
		m_flag |= CHUNK_FLAG_ACTIVE;
	else
		m_flag &= ~(CHUNK_FLAG_ACTIVE);
}

//assumes coordinates are in world position
void VoxelChunk::InsertVoxelAtPos(int x, int y, int z)
{
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		if (i <= y - (int)m_position.y)
		{
			m_voxels[GetIndex(x - (int)m_position.x, i, z - (int)m_position.z)] = VOXEL_TYPE_GRASS;
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

void VoxelChunk::BindMesh()
{
	if ((m_vertices.size() == 0) || m_tri_indices.size() == 0)
		return;

	glGenVertexArrays(1, &this->m_vao);
	glGenBuffers(1, &this->m_vbo);
	glGenBuffers(1, &this->m_ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VoxelVertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_tri_indices.size() * sizeof(GLuint), &m_tri_indices[0], GL_STATIC_DRAW);

	//location 0 should be verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(VoxelVertex), (GLvoid*)0);
	//now normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_BYTE, GL_FALSE, sizeof(VoxelVertex), (GLvoid*)offsetof(VoxelVertex, normal));
	//now textures
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_BYTE, GL_FALSE, sizeof(VoxelVertex), (GLvoid*)offsetof(VoxelVertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VoxelChunk::ClearVertices()
{
	std::vector<VoxelVertex> empty;
	m_vertices.swap(empty);
}

int inline VoxelChunk::GetIndex(int x, int y, int z)
{
	return x * CHUNK_SIZE_SQ + y * CHUNK_SIZE + z;
}

void VoxelChunk::GenerateMesh(Model *cube)
{
	if (!(m_flag & CHUNK_FLAG_ACTIVE))
		return;

	int checks = 0;

	sf::Clock timer;
	for (GLbyte x = 0; x < CHUNK_SIZE; x++)
	{
		for (GLbyte y = 0; y < CHUNK_SIZE; y++)
		{
			for (GLbyte z = 0; z < CHUNK_SIZE; z++)
			{
				if (m_voxels[GetIndex(x, y, z)] & VOXEL_TYPE_AIR)
					continue;
				//check if the voxel on the left is active
				checks += 18;
				if ((x > 0 && m_voxels[GetIndex(x, y, z)] & VOXEL_TYPE_AIR) || x == 0)
				{
					VoxelVertex vertex = { (x,y,z + 1), 
											(-1,0,0), 
											(0,0) };

					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y + 1, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y + 1, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y, z) };
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}

				if ((x < CHUNK_SIZE - 1 && m_voxels[GetIndex(x + 1, y, z)] & VOXEL_TYPE_AIR) || x == CHUNK_SIZE - 1)
				{
					VoxelVertex vertex = { (x,y,z + 1),
											(1,0,0),
											(0,0) };

					vertex.position = Vec3byte{ (x + 1, y, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y + 1, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y + 1, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y, z + 1) };
					m_vertices.push_back(vertex);
					AddTrianglesIndices();
				}
				if ((y > 0 && (m_voxels[GetIndex(x, y - 1, z)] & VOXEL_TYPE_AIR)) || y == 0)
				{
					VoxelVertex vertex = { (x,y,z + 1),
										(0,-1,0),
										(0,0) };

					vertex.position = Vec3byte{ (x, y, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y, z + 1) };
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}
				if ((y < CHUNK_SIZE - 1 && m_voxels[GetIndex(x, y + 1, z)] & VOXEL_TYPE_AIR) || y == CHUNK_SIZE - 1)
				{
					VoxelVertex vertex = { (x,y,z + 1),
											(0,1,0),
											(0,0) };

					vertex.position = Vec3byte{ (x, y + 1, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y + 1, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y + 1, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y + 1, z) };
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}

				if ((z > 0 && m_voxels[GetIndex(x, y, z - 1)] & VOXEL_TYPE_AIR) || z == 0)
				{
					VoxelVertex vertex= { (x,y,z + 1),
									(0,0,-1),
									(0,0) };

					vertex.position = Vec3byte{ (x, y, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y + 1, z) };
					m_vertices.push_back(vertex);
					vertex.position = Vec3byte{ (x + 1, y + 1, z) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y, z) };
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}
				if ((z < CHUNK_SIZE - 1 && m_voxels[GetIndex(x, y, z + 1)] & VOXEL_TYPE_AIR) || z == CHUNK_SIZE - 1)
				{
					VoxelVertex vertex = { (x,y,z + 1),
											(0, 0,1),
											(0,0) };

					vertex.position = Vec3byte{ (x + 1, y, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x + 1, y + 1, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y + 1, z + 1) };
					m_vertices.push_back(vertex);

					vertex.position = Vec3byte{ (x, y, z + 1) };
					m_vertices.push_back(vertex);
					AddTrianglesIndices();
				}

			}
		}
	}
	//std::cout << "Time to generate is " << timer.getElapsedTime().asMicroseconds() << std::endl;
	//std::cout << "comparisons " << checks << std::endl;
}

void VoxelChunk::SetVoxelActive(int x, int y, int z)
{
	//m_voxels[x - (int)m_position.x][y - (int)m_position.y][z - (int)m_position.z].SetActive(true);//convert world coor to local
}

Vec3 VoxelChunk::getPosition()
{
	return m_position;
}

void VoxelChunk::Render(GLuint shader)
{
	if ((m_vertices.size() == 0) || m_tri_indices.size() == 0)
		return;
	glBindVertexArray(m_vao);

	glUniform3fv(glGetUniformLocation(shader, "chunkPosition"), 1, &m_position[0]);
	glDrawElements(GL_TRIANGLES, m_tri_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}