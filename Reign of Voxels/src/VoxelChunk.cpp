#include "VoxelChunk.hpp"
#include "VoxelOctree.hpp"
#include "model.hpp"

const glm::ivec3 g_neighbors[6] =
{
	glm::ivec3(-1,0,0),
	glm::ivec3(1,0,0),
	glm::ivec3(0,-1,0),
	glm::ivec3(0, 1,0),
	glm::ivec3(0,0,-1),
	glm::ivec3(0,0, 1)
};

VoxelChunk::VoxelChunk() : m_flag(0)
{
	m_render_mode = true;
}

void VoxelChunk::Init(glm::vec3 position, VoxelOctree * node)
{
	if (m_flag & CHUNK_FLAG_INUSE)
		std::cout << "Error already using chunk" << std::endl;

	m_flag |= CHUNK_FLAG_INUSE;
	m_flag |= CHUNK_FLAG_ACTIVE;

	m_node = node;

	memset(m_neighbor, NULL, sizeof(m_neighbor));
}

void VoxelChunk::Destroy()
{
	m_flag = 0;
	m_node = NULL;

	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	glDeleteVertexArrays(1, &m_vao);
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

void VoxelChunk::BindMesh()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);

	if ((m_vertices.size() == 0) || m_tri_indices.size() == 0)
	{
		return;
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelVertex) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_tri_indices.size(), &m_tri_indices[0], GL_STATIC_DRAW);

	//location 0 should be verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (GLvoid*)0);
	//now normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (GLvoid*)offsetof(VoxelVertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(VoxelVertex), (GLvoid*)offsetof(VoxelVertex, textureID));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void VoxelChunk::BindWaterPlanes()
{
	glDeleteVertexArrays(1, &m_water_vao);
	glDeleteBuffers(1, &m_water_vbo);
	glDeleteBuffers(1, &m_water_ebo);

	if ((m_water_vertices.size() == 0) || m_water_indices.size() == 0)
	{
		return;
	}

	glGenVertexArrays(1, &m_water_vao);
	glBindVertexArray(m_water_vao);

	glGenBuffers(1, &m_water_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_water_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelVertex) * m_water_vertices.size(), &m_water_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_water_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_water_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_water_indices.size(), &m_water_indices[0], GL_STATIC_DRAW);

	//location 0 should be verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (GLvoid*)0);
	//now normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (GLvoid*)offsetof(VoxelVertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(VoxelVertex), (GLvoid*)offsetof(VoxelVertex, textureID));

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VoxelChunk::AddTrianglesIndices(GLuint indice)
{
	m_tri_indices.push_back(indice);
}

void VoxelChunk::AddVertices(VoxelVertex voxelVerts)
{
	m_vertices.push_back(voxelVerts);
}

int inline VoxelChunk::GetIndex(int x, int y, int z)
{
	return x * CHUNK_SIZE_SQ + y * CHUNK_SIZE + z;
}


void VoxelChunk::AssignNeighbor(VoxelChunk * neighbor, int side)
{
	m_neighbor[side] = neighbor;
}

void VoxelChunk::GenerateSeam()
{
	FindSeamNodes();

	if (m_seam_nodes.size() == 0) return;

	int currentSize = 1;
	glm::ivec3 chunkPos = m_node->m_min;

	//create an octree to contour
	//delete tree when done
	while (m_seam_nodes.size() > 1)
	{
		int parentSize = currentSize * 2;
		std::vector<VoxelOctree *> parents;

		for (int i = 0; i < m_seam_nodes.size(); i++)
		{
			bool foundParent = false;
			
			VoxelOctree * currNode = m_seam_nodes[i];

			glm::ivec3 childPos = currNode->m_min;
			glm::ivec3 parent_pos = childPos - ((childPos - chunkPos) % parentSize);
			

			int x = childPos.x >= parent_pos.x && childPos.x < parent_pos.x + currentSize ? 0 : 1;
			int y = childPos.y >= parent_pos.y && childPos.y < parent_pos.y + currentSize ? 0 : 1;
			int z = childPos.z >= parent_pos.z && childPos.z < parent_pos.z + currentSize ? 0 : 1;

			int index = 4 * x + 2 * y + z;

			for (int j = 0; j < parents.size(); j++)
			{
				if (parents[j]->m_min == parent_pos)
				{
					foundParent = true;					
					parents[j]->m_children[index] = currNode;
					parents[j]->m_childMask |= 1 << index;
					break;
				}
			}
			if (!foundParent)
			{
				VoxelOctree *parent = new VoxelOctree();

				parent->InitNode(parent_pos, parentSize);

				parent->m_flag |= OCTREE_ACTIVE;
				parent->m_type = Node_Internal;
				parent->m_childMask |= 1 << index;
				parent->m_children[index] = m_seam_nodes[i];

				parents.push_back(parent);
			}
		}

		m_seam_nodes = parents;
		currentSize *= 2;
	}

	m_seam_nodes[0]->GenerateVertexIndices(m_vertices);
	m_seam_nodes[0]->ContourCellProc(m_tri_indices);

	//DeleteSeamTree(NULL);
}


void VoxelChunk::FindSeamNodes()
{
	m_seam_nodes.clear();
	
	//find nodes on max border of this chunk, min of x and z neighbor, and one node on xz diagonal
	int max = CHUNK_SIZE - 1;
	int min = 0;
	
	glm::ivec3 chunkMin = m_node->m_min;
	glm::ivec3 chunkMax = chunkMin + CHUNK_SIZE;

	for (int x = 0; x <= max; x++)
	{
		for (int y = 0; y <= max; y++)
		{
			int z = max;
						
			VoxelOctree * node = m_node->FindLeafNode(chunkMin + glm::ivec3(x, y, z));

			if (node && node->m_flag & OCTREE_ACTIVE)
				m_seam_nodes.push_back(node);
		}
	}

	for (int z = 0; z <= max; z++)
	{
		for (int y = 0; y <= max; y++)
		{
			int x = max;

			VoxelOctree * node = m_node->FindLeafNode(chunkMin + glm::ivec3(x, y, z));

			if (node && node->m_flag & OCTREE_ACTIVE)
				m_seam_nodes.push_back(node);
		}
	}
	for (int z = 0; z <= max; z++)
	{
		for (int x = 0; x <= max; x++)
		{
			int y = max;

			VoxelOctree * node = m_node->FindLeafNode(chunkMin + glm::ivec3(x, y, z));

			if (node && node->m_flag & OCTREE_ACTIVE)
				m_seam_nodes.push_back(node);
		}
	}

	if (m_neighbor[FRONT])
	{
		//find nodes on min border of neighbors
		for (int x = 0; x <= CHUNK_SIZE; x++)
		{
			for (int y = 0; y <= CHUNK_SIZE; y++)
			{
				int z = min;
				VoxelOctree * node = m_neighbor[FRONT]->m_node->FindLeafNode(m_neighbor[FRONT]->m_node->m_min + glm::ivec3(x, y, z));

				if (node)
				{
					m_seam_nodes.push_back(node);
				}
			}
		}
	}

	//min nodes of right neighbor
	if (m_neighbor[RIGHT])
	{
		for (int z = 0; z <= CHUNK_SIZE; z++)
		{
			for (int y = 0; y <= CHUNK_SIZE; y++)
			{
				int x = min;

				VoxelOctree * node = m_neighbor[RIGHT]->m_node->FindLeafNode(m_neighbor[RIGHT]->m_node->m_min + glm::ivec3(x, y, z));
				if (node )
				{
					m_seam_nodes.push_back(node);
				}

			}
		}
	}


	if (m_neighbor[TOP])
	{
		//find nodes on min border of neighbors
		for (int x = 0; x <= CHUNK_SIZE; x++)
		{
			for (int z = 0; z <= CHUNK_SIZE; z++)
			{
				int y = min;
				VoxelOctree * node = m_neighbor[TOP]->m_node->FindLeafNode(m_neighbor[TOP]->m_node->m_min + glm::ivec3(x, y, z));

				if (node)
				{
					m_seam_nodes.push_back(node);
				}
			}
		}
	}

	if (m_neighbor[TOP] && m_neighbor[TOP]->m_neighbor[RIGHT])
	{
		VoxelChunk *top_right = m_neighbor[TOP]->m_neighbor[RIGHT];

		for (int z = 0; z <= max; z++)
		{
			int x = min;
			int y = min;

			VoxelOctree * node = top_right->m_node->FindLeafNode(top_right->m_node->m_min + glm::ivec3(x, y, z));

			if (node)
				m_seam_nodes.push_back(node);
		}

	}

	if (m_neighbor[TOP] && m_neighbor[TOP]->m_neighbor[FRONT])
	{
		VoxelChunk *chunk = m_neighbor[TOP]->m_neighbor[FRONT];

		for (int x = 0; x <= max; x++)
		{
			int z = min;
			int y = min;

			VoxelOctree * node = chunk->m_node->FindLeafNode(chunk->m_node->m_min + glm::ivec3(x, y, z));

			if (node)
				m_seam_nodes.push_back(node);
		}

	}

	if (m_neighbor[TOP] && m_neighbor[TOP]->m_neighbor[FRONT_RIGHT])
	{
		VoxelChunk *chunk = m_neighbor[TOP]->m_neighbor[FRONT_RIGHT];

		for (int y = 0; y <= max; y++)
		{
			int z = min;
			int x = min;

			VoxelOctree * node = chunk->m_node->FindLeafNode(chunk->m_node->m_min + glm::ivec3(x, y, z));

			if (node)
				m_seam_nodes.push_back(node);
		}

	}
	//finally find xz diagonal neighbor

	for (int y = 0; y <= CHUNK_SIZE; y++)
	{
		if (!m_neighbor[FRONT_RIGHT])
			break;

		VoxelOctree * node = m_neighbor[FRONT_RIGHT]->m_node->FindLeafNode(m_neighbor[FRONT_RIGHT]->m_node->m_min + glm::ivec3(0, y, 0));

		if (node)
		{
			m_seam_nodes.push_back(node);
		}
	}

}

void VoxelChunk::DeleteSeamTree(VoxelOctree *node)
{
	if (!node)
	{
		node = m_seam_nodes[0];
	}

	//leaf nodes belong to chunk
	//dont delete
	if (node->m_type == Node_Leaf)
		return;

	for (int i = 0; i < 8; i++)
	{
		if (node->m_childMask & 1 << i)
		{
			DeleteSeamTree(node->m_children[i]);
		}
	}
	
	//can't call node destroy because it's not part of VM node pool
	//TODO create seperate node pool for seam nodes
	delete node;
}

/***Edit world ***/
void VoxelChunk::DestroyVoxel(glm::ivec3 world_pos, glm::ivec3 face)
{
	if (world_pos.y <= 1)
	{
		return;
	}

	//DestroyVoxel(world_pos, m_node);
	glm::ivec3 min = m_node->m_min;
	glm::ivec3 max = min + CHUNK_SIZE - 1;

	m_csgOpPos.push_back(world_pos);
	m_node->BuildTree(m_csgOpPos);

	m_vertices.clear();
	m_tri_indices.clear();

	m_node->GenerateVertexIndices(m_vertices);
	m_node->ContourCellProc(m_tri_indices);
}

void VoxelChunk::Render()
{
	if (~m_flag & CHUNK_FLAG_ACTIVE || !m_render_mode)
		return;

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_tri_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void VoxelChunk::RenderWater()
{
	if (~m_flag & CHUNK_FLAG_ACTIVE || m_water_indices.size() == 0)
		return;

	glBindVertexArray(m_water_vao);
	glDrawElements(GL_TRIANGLES, m_water_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


