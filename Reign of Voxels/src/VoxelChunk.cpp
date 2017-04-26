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
	m_tri_indices = empty_indices;

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
	{
		return;
	}

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

glm::vec3 VoxelChunk::getPosition()
{
	return m_position;
}

void VoxelChunk::GenerateMesh()
{
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

sf::Uint8 VoxelChunk::GetVoxel(glm::ivec3 local_pos)
{
	return m_voxels[GetIndex(local_pos.x, local_pos.y, local_pos.z)];
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