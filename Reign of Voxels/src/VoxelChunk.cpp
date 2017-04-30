#include "VoxelChunk.hpp"
#include "VoxelOctree.hpp"
#include "model.hpp"


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

void VoxelChunk::Render()
{
	if (~m_flag & CHUNK_FLAG_ACTIVE || !m_render_mode)
		return;

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_tri_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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
	glm::ivec3 pos = m_node->m_min;

	for (int x = 0; x < CHUNK_SIZE - 1; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			int z = max;
			
			VoxelOctree * node = m_node->FindLeafNode(pos + glm::ivec3(x, y, z));

			if (node && node->m_flag & OCTREE_ACTIVE)
			{
				m_seam_nodes.push_back(node);
			}
		}
	}

	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			int x = max;
			VoxelOctree * node = m_node->FindLeafNode(pos + glm::ivec3(x, y, z));

			if (node && node->m_flag & OCTREE_ACTIVE)
			{
				m_seam_nodes.push_back(node);
			}
		}
	}

	for (int z = 0; z < CHUNK_SIZE - 1; z++)
	{
		for (int x = 0; x < CHUNK_SIZE - 1; x++)
		{
			int y = max;
			VoxelOctree * node = m_node->FindLeafNode(pos + glm::ivec3(x, y, z));

			if (node && node->m_flag & OCTREE_ACTIVE)
			{
				m_seam_nodes.push_back(node);
			}
		}
	}

	if (m_neighbor[FRONT])
	{
		//find nodes on min border of neighbors
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int y = 0; y < CHUNK_SIZE; y++)
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

	if (m_neighbor[TOP])
	{
		//find nodes on min border of neighbors
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
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

	//min nodes of right neighbor
	if (m_neighbor[RIGHT])
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = 0; y < CHUNK_SIZE; y++)
			{
				int x = min;

				VoxelOctree * node = m_neighbor[RIGHT]->m_node->FindLeafNode(m_neighbor[RIGHT]->m_node->m_min + glm::ivec3(x, y, z));
				if (z == 0 && node)
				{
					int a = 2;
				}
				if (node )
				{
					m_seam_nodes.push_back(node);
				}

			}
		}
	}

	//finally find xz diagonal neighbor
	for (int y = 0; y < CHUNK_SIZE; y++)
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


void VoxelChunk::DestroyVoxel(glm::ivec3 world_pos, VoxelOctree *node)
{
	if (node->m_type == Node_Leaf)
	{
		node->DestroyNode();
		return;
	}

	int child_size = node->m_size >> 1;

	int x = world_pos.x >= node->m_min.x && world_pos.x < node->m_min.x + child_size ? 0 : 1;
	int y = world_pos.y >= node->m_min.y && world_pos.y < node->m_min.y + child_size ? 0 : 1;
	int z = world_pos.z >= node->m_min.z && world_pos.z < node->m_min.z + child_size ? 0 : 1;

	int index = 4 * x + 2 * y + z;

	DestroyVoxel(world_pos, node->m_children[index]);
	node->m_childMask &= ~(1 << index);

	if (node->m_childMask == 0)
	{
		node->DestroyNode();
	}
}
/***Edit world ***/
void VoxelChunk::DestroyVoxel(glm::ivec3 world_pos)
{
	//DestroyVoxel(world_pos, m_node);

	if (world_pos.y <= 1)
		return;

	m_csgOpPos.clear();
	m_csgOpPos.push_back(world_pos);

	//VoxelOctree *targetNode = m_node->FindLeafNode(world_pos);
	//if (!targetNode)
	//{
	//	slog("No node hit");
	//	return;
	//}
	//targetNode->BuildLeafNode();
	m_node->BuildTree(m_csgOpPos);

	m_vertices.clear();
	m_tri_indices.clear();

	m_node->GenerateVertexIndices(m_vertices);
	m_node->ContourCellProc(m_tri_indices);

	//GenerateSeam();

	BindMesh();
}

void VoxelChunk::RebuildTree(glm::ivec3 world_pos)
{
	//VoxelOctree *node = this;

	//traverse tree, find chunk
	bool found = false;

	//while (!found)
	//{
	//	int index = 0;

	//	//check which child the next node is in 
	//	int child_size = node->m_size >> 1;

	//	//octal search
	//	int x = pos.x >= node->m_min.x && pos.x < node->m_min.x + child_size ? 0 : 1;
	//	int y = pos.y >= node->m_min.y && pos.y < node->m_min.y + child_size ? 0 : 1;
	//	int z = pos.z >= node->m_min.z && pos.z < node->m_min.z + child_size ? 0 : 1;

	//	//calculate index 
	//	index = 4 * x + 2 * y + z;

	//	node = node->m_children[index];

	//	if (!node || ~node->m_flag & OCTREE_ACTIVE || ~node->m_flag & OCTREE_INUSE)
	//		return NULL;

	//	if (node->m_type == Node_Leaf)
	//		found = true;
	//}
}