#include "VoxelChunk.hpp"
#include "model.hpp"


VoxelChunk::VoxelChunk() : m_flag(0)
{

}

void VoxelChunk::Init(glm::vec3 position)
{
	if (m_flag & CHUNK_FLAG_INUSE)
		std::cout << "Error already using chunk" << std::endl;

	m_flag = CHUNK_FLAG_INUSE;

	m_position = position;

	memset(m_voxels, VOXEL_TYPE_AIR, sizeof(m_voxels));
	memset(m_neighbor, NULL, sizeof(m_neighbor));

	std::vector<GLuint>empty_indices;
	m_tri_indices  = empty_indices;

	std::vector<Vertex> empty_vertices;
	m_vertices = empty_vertices;
}

void VoxelChunk::Destroy()
{
	m_flag = 0;

	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	glDeleteVertexArrays(1, &m_vao);
}


glm::vec3 VoxelChunk::getSize()
{
	return glm::vec3(CHUNK_SIZE);
}

bool VoxelChunk::IsActive()
{
	return m_flag & CHUNK_FLAG_ACTIVE != 0;
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

void VoxelChunk::AddMinimapIndices()
{
	m_mp_indices.push_back(m_top_verts.size() - 4);
	m_mp_indices.push_back(m_top_verts.size() - 3);
	m_mp_indices.push_back(m_top_verts.size() - 2);
	m_mp_indices.push_back(m_top_verts.size() - 4);
	m_mp_indices.push_back(m_top_verts.size() - 2);
	m_mp_indices.push_back(m_top_verts.size() - 1);
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

	////minimap
	glGenVertexArrays(1, &m_mp_vao);
	glGenBuffers(1, &m_mp_vbo);
	glGenBuffers(1, &m_mp_ebo);

	glBindVertexArray(m_mp_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_mp_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_top_verts.size() * sizeof(Vertex), &m_top_verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mp_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mp_indices.size() * sizeof(GLuint), &m_mp_indices[0], GL_STATIC_DRAW);

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


int inline VoxelChunk::GetIndex(int x, int y, int z)
{
	return x * CHUNK_SIZE_SQ + y * CHUNK_SIZE + z;
}


void VoxelChunk::AddLeftFace(int x, int y, int z)
{
	Vertex vertex;	
	vertex.normal = glm::vec3(-1.0f, 0, 0); //normal reversed because this is the previous voxel's rirght
	vertex.uv = glm::vec2(.5f);

	vertex.position = glm::vec3(x, y, z + 1.0f);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x, y + 1.0f, z + 1.0f);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x, y + 1.0f, z);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x, y, z);
	m_vertices.push_back(vertex);

	AddTrianglesIndices();
}

void VoxelChunk::AddRightFace(int x, int y, int z)
{
	Vertex vertex;	
	vertex.normal = glm::vec3(1.0f, 0, 0);
	vertex.uv = glm::vec2(.5f);

	
	vertex.position = glm::vec3(x + 1, y, z + 1.0f);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x + 1, y + 1.0f, z + 1.0f);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x + 1, y + 1.0f, z);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x + 1, y, z);
	m_vertices.push_back(vertex);

	AddTrianglesIndices();
}

void VoxelChunk::AddBottomFace(int x, int y, int z)
{
	Vertex vertex;
	vertex.normal = glm::vec3(0.0f, -1, 0);
	vertex.uv = glm::vec2(.5f);

	vertex.position = glm::vec3(x, y, z);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x + 1.0f, y, z);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x + 1.0f, y, z + 1.0f);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x, y, z + 1.0f);
	m_vertices.push_back(vertex);

	AddTrianglesIndices();
}

void VoxelChunk::AddTopFace(int x, int y, int z)
{
	Vertex vertex;
	vertex.normal = glm::vec3(0.0f, 1, 0);
	vertex.uv = glm::vec2(.5f);

	vertex.position = glm::vec3(x, y + 1, z);
	m_vertices.push_back(vertex);
	m_top_verts.push_back(vertex);

	vertex.position = glm::vec3(x + 1.0f, y + 1, z);
	m_vertices.push_back(vertex);
	m_top_verts.push_back(vertex);

	vertex.position = glm::vec3(x + 1.0f, y + 1, z + 1.0f);
	m_vertices.push_back(vertex);
	m_top_verts.push_back(vertex);

	vertex.position = glm::vec3(x, y + 1, z + 1.0f);
	m_vertices.push_back(vertex);
	m_top_verts.push_back(vertex);

	AddTrianglesIndices();
	AddMinimapIndices();
}

void VoxelChunk::AddBackFace(int x, int y, int z)
{
	Vertex vertex;
	vertex.normal = glm::vec3(0.0f, 0, -1.0f);
	vertex.uv = glm::vec2(.5f);

	vertex.position = glm::vec3(x, y, z);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x, y + 1.0f, z);
	m_vertices.push_back(vertex);
	vertex.position = glm::vec3(x + 1.0f, y + 1.0f, z);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x + 1.0f, y, z);
	m_vertices.push_back(vertex);

	AddTrianglesIndices();
}

void VoxelChunk::AddFrontFace(int x, int y, int z)
{
	Vertex vertex;
	vertex.normal = glm::vec3(0.0f, 0, 1.0f);
	
	vertex.uv = glm::vec2(0, 0);
	vertex.position = glm::vec3(x, y, z + 1);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x, y + 1.0f, z + 1);
	m_vertices.push_back(vertex);
	vertex.position = glm::vec3(x + 1.0f, y + 1.0f, z + 1);
	m_vertices.push_back(vertex);

	vertex.position = glm::vec3(x + 1.0f, y, z + 1);
	m_vertices.push_back(vertex);

	AddTrianglesIndices();
}

void VoxelChunk::SetVoxelActive(int x, int y, int z)
{
	//m_voxels[x - (int)m_position.x][y - (int)m_position.y][z - (int)m_position.z].SetActive(true);//convert world coor to local
}

glm::vec3 VoxelChunk::getPosition()
{
	return m_position;
}

void VoxelChunk::GenerateMesh()
{
	//generate mesh based on neighboring chunks first

	//generate vertices for each face of the chunk
	//loop through each face rather than each voxel
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			int y = CHUNK_SIZE - 1;

			if (m_voxels[GetIndex(x, y, z)] & VOXEL_TYPE_AIR)
				continue;

			if (m_neighbor[TOP])
			{
				if (m_neighbor[TOP]->m_voxels[GetIndex(x, 0, z)] & VOXEL_TYPE_AIR)
					AddTopFace(x, y, z);
			}
			else
			{
				AddTopFace(x, y, z);
			}
		}
	}

	int max = CHUNK_SIZE - 1;

	for (int x = 0; x <= max; x++)
	{
		for (int y = 0; y <= max; y++)
		{
			for (int z = 0; z <= max; z++)
			{
				if (~m_voxels[GetIndex(x, y, z)] & VOXEL_TYPE_AIR)
				{
					//check left neighbor
					if (x == 0) // left most voxel
					{
						//get the right most voxel of left neighbor and compare
						if (!m_neighbor[LEFT] || m_neighbor[LEFT]->m_voxels[GetIndex(max, y, z)] & VOXEL_TYPE_AIR)
								AddLeftFace(x, y, z);
					}
					else if (m_voxels[GetIndex(x - 1, y, z)] & VOXEL_TYPE_AIR)
					{
						AddLeftFace(x, y, z);
					}

					if (x == max) // right most voxel
					{
						//compare to left voxel of right neighbor
						if (!m_neighbor[RIGHT] || m_neighbor[RIGHT]->m_voxels[GetIndex(0, y, z)] & VOXEL_TYPE_AIR)
							AddRightFace(x, y, z);
					}
					else if (m_voxels[GetIndex(x + 1, y, z)] & VOXEL_TYPE_AIR)
					{
						AddRightFace(x, y, z);
					}

					if (m_voxels[GetIndex(x, y - 1, z)] & VOXEL_TYPE_AIR)
					{
						AddBottomFace(x, y, z);
					}

					if (y  == max)
					{
						//compare bottom voxel of above neighbor	
						if (!m_neighbor[TOP] || m_neighbor[TOP]->m_voxels[GetIndex(x, 0, z)] & VOXEL_TYPE_AIR)
							AddTopFace(x, y, z);
					}
					else if(m_voxels[GetIndex(x, y + 1, z)] & VOXEL_TYPE_AIR)
					{
						AddTopFace(x, y, z);
					}

					//back
					if (z == 0)
					{
						if (!m_neighbor[BACK] || (m_neighbor[BACK]->m_voxels[GetIndex(x, y, max)] & VOXEL_TYPE_AIR))
							AddBackFace(x, y, z);
					}
					else if ( m_voxels[GetIndex(x, y, z - 1)] & VOXEL_TYPE_AIR)
					{
						AddBackFace(x, y, z);
					}

					//front
					if (z == max)
					{
						if (!m_neighbor[FRONT] || (m_neighbor[FRONT]->m_voxels[GetIndex(x, y, 0)] & VOXEL_TYPE_AIR))
							AddFrontFace(x, y, z);
					}
					else if (m_voxels[GetIndex(x, y, z + 1)] & VOXEL_TYPE_AIR)
					{
						AddFrontFace(x, y, z);
					}
				}
			}
		}
	}

}

void VoxelChunk::ClearMeshData()
{
	m_mp_indices_count = m_mp_indices.size();
	m_indices_count = m_tri_indices.size();

	std::vector<Vertex>().swap(m_vertices);
	std::vector<Vertex>().swap(m_top_verts);

	std::vector<GLuint>().swap(m_tri_indices);
	std::vector<GLuint>().swap(m_mp_indices);	
}

void VoxelChunk::AssignNeighbor(VoxelChunk * neighbor, int side)
{
	m_neighbor[side] = neighbor;
}

void VoxelChunk::ClearPlayerStart(float height)
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				//TODO if air get perlin noise map and find a different terrain
				if (y > height)
					m_voxels[GetIndex(x, y, z)] = VOXEL_TYPE_AIR;
				else
					m_voxels[GetIndex(x, y, z)] = VOXEL_TYPE_GRASS;
			}
		}
	}
}

void VoxelChunk::SmoothTerrain(float scale, glm::vec3 origin)
{
	//smooth terrain from the origin
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			//TODO reduce distance calculations
			glm::vec3 position = m_position + glm::vec3(x, 0, z);
			float distance = glm::abs(origin.x - position.x) + glm::abs(origin.z - position.x);
			
			float max_height = distance - CHUNK_SIZE / 2.0f;

			for (int y = max_height; y < CHUNK_SIZE; y++)
			{
				m_voxels[GetIndex(x, y, z)] = VOXEL_TYPE_AIR;
			}
		}
	}

}

void VoxelChunk::Render()
{
	if (~m_flag & CHUNK_FLAG_ACTIVE)
		return; 

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void VoxelChunk::RenderMinimap()
{
	glBindVertexArray(m_mp_vao);
	glDrawElements(GL_TRIANGLES, m_mp_indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}