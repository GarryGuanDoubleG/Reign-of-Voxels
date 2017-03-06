#include "VoxelChunk.hpp"
#include "model.hpp"

VoxelChunk::VoxelChunk(Vec3 position)
{
	m_active = false;
	
	m_position = position;
	
	m_voxel_count = 0;

	m_vao = 0;
	m_vbo = 0;
	m_ebo = 0;
}

VoxelChunk::~VoxelChunk()
{
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);

	if(m_ebo)
		glDeleteBuffers(1, &m_ebo);
}

Vec3 VoxelChunk::getSize()
{
	return Vec3(CHUNK_SIZE);
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
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		if (i <= y - (int)m_position.y)
		{
			m_voxels[x - (int)m_position.x][i][z - (int)m_position.z].SetActive(true);//convert world coor to local
			++m_voxel_count;
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
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_tri_indices.size() * sizeof(GLuint), &m_tri_indices[0], GL_STATIC_DRAW);

	//location 0 should be verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	//now normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	//now textures
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VoxelChunk::GenerateMesh(Model *cube)
{
	if (!m_active)
		return;
	int checks = 0;

	if (m_voxel_count >= CHUNK_SIZE_CUBED)
	{
		sf::Clock timer;

		Mesh cube_mesh = (*cube->GetMesh())[0];
		
		m_vertices = cube_mesh.vertices;
		m_tri_indices = cube_mesh.indices;

		for (int i = 0; i < m_vertices.size(); i++)
		{
			m_vertices[i].position *= CHUNK_SIZE;
		}

		std::cout << "Cube Mesh " << timer.getElapsedTime().asMicroseconds() << std::endl;
		timer.restart();
		return;
	}

	sf::Clock timer;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (m_voxels[x][y][z].IsActive() == false)
					continue;
				//check if the voxel on the left is active
				checks += 18;
				if ((x > 0 && !m_voxels[x - 1][y][z].IsActive()) || x == 0)
				{
					Vertex vertex;
					vertex.position = Vec3(x, y, z + 1.0f);
					vertex.normal = Vec3(-1.0f, 0, 0);
					vertex.uv = Vec2(.5f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y + 1.0f, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y + 1.0f, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y, z);
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}

				if ((x < CHUNK_SIZE - 1 && !m_voxels[x + 1][y][z].IsActive()) || x == CHUNK_SIZE - 1)
				{
					Vertex vertex;
					vertex.normal = Vec3(1.0f, 0, 0);
					vertex.uv = Vec2(.5f);

					vertex.position = Vec3(x + 1.0f, y, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y + 1.0f, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y + 1.0f, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y, z + 1.0f);
					m_vertices.push_back(vertex);
					AddTrianglesIndices();
				}
				if ((y > 0 && !m_voxels[x][y - 1][z].IsActive()) || y == 0)
				{
					Vertex vertex;
					vertex.normal = Vec3(0.0f, -1, 0);
					vertex.uv = Vec2(.5f);

					vertex.position = Vec3(x, y, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y, z + 1.0f);
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}
				if ((y < CHUNK_SIZE - 1 && !m_voxels[x][y + 1][z].IsActive()) || y == CHUNK_SIZE - 1)
				{
					Vertex vertex;
					vertex.normal = Vec3(0.0f, 1.0f, 0);
					vertex.uv = Vec2(.5f);

					vertex.position = Vec3(x, y + 1.0f, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y + 1.0f, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y + 1.0f, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y + 1.0f, z);
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}

				if ((z > 0 && !m_voxels[x][y][z - 1].IsActive()) || z == 0)
				{
					Vertex vertex;
					vertex.normal = Vec3(0.0f, 0, -1.0f);
					vertex.uv = Vec2(.5f);

					vertex.position = Vec3(x, y, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y + 1.0f, z);
					m_vertices.push_back(vertex);
					vertex.position = Vec3(x + 1.0f, y + 1.0f, z);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y, z);
					m_vertices.push_back(vertex);

					AddTrianglesIndices();
				}
				if ((z < CHUNK_SIZE - 1 && !m_voxels[x][y][z + 1].IsActive()) || z == CHUNK_SIZE - 1)
				{
					Vertex vertex;
					vertex.normal = Vec3(0.0f, 0, 1.0f);
					vertex.uv = Vec2(.5f);

					vertex.position = Vec3(x + 1.0f, y, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x + 1.0f, y + 1.0f, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y + 1.0f, z + 1.0f);
					m_vertices.push_back(vertex);

					vertex.position = Vec3(x, y, z + 1.0f);
					m_vertices.push_back(vertex);
					AddTrianglesIndices();
				}

			}
		}
	}
	std::cout << "Time to generate is " << timer.getElapsedTime().asMicroseconds() << std::endl;
	//std::cout << "comparisons " << checks << std::endl;
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
	if ((m_vertices.size() == 0) || m_tri_indices.size() == 0)
		return;
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_tri_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}