#include "VoxelOctree.hpp"
#include "glm.hpp"
#include "simple_logger.h"

static sf::Image *g_heightMap;
static const sf::Uint8 *g_heightarray;//array of pixel values
static unsigned int vox_count = 0;
unsigned int VoxelOctree::m_chunkCount = 0;
std::vector<VoxelChunk *> VoxelOctree::render_list; //list of leaf nodes
int leaf_count = 0;
unsigned long long iterations = 0;

VoxelOctree::VoxelOctree(VoxelOctree * parent)
{
	m_parent = parent;
	m_childMask = 0;
	m_chunkCount = 0;
}

VoxelOctree::~VoxelOctree()
{
	if(m_chunk)
		delete m_chunk;
}

void VoxelOctree::InitializeOctree(sf::Image *heightmap, int worldSize)
{
	g_heightMap = heightmap;
	g_heightarray = g_heightMap->getPixelsPtr();

	m_boundingRegion.position = Vec3(0, 0, 0);
	m_boundingRegion.size = worldSize;
	
	sf::Clock build_time;
	BuildNode();

	//for (int x = 0; x < worldSize; x++)
	//{
	//	for (int z = 0; z < worldSize; z++)
	//	{
	//		int height = g_heightMap->getPixel(x, z).r;
	//		InsertVoxelAtHeight(x, height, z);
	//	}
	//}

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Childnode calls is " << vox_count << std::endl;
	std::cout << "leaf nodes " << leaf_count << std::endl;
	std::cout << "Iterations " << iterations << std::endl;
	
	build_time.restart();
	int vertices = 0;
	for (int i = 0; i < render_list.size(); i++)
	{
		render_list[i]->GenerateMesh();
		vertices += render_list[i]->m_vertices.size();
	}

	std::cout << "Generate Mesh time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Vertices: " << vertices << std::endl;
}

bool VoxelOctree::BuildNode()
{
	int size = m_boundingRegion.size;
	Vec3 pos = m_boundingRegion.position;
	bool active = false;

	int i = 0;
	vox_count++;
	
	if (size <= VoxelChunk::chunkSize)
	{
		if(!m_chunk)
			m_chunk = new VoxelChunk(m_boundingRegion.position);

		for (int x = m_boundingRegion.position.x; x < m_boundingRegion.position.x + size; x++)
		{
			for (int z = m_boundingRegion.position.z; z < m_boundingRegion.position.z + size; z++)
			{
				float height = g_heightMap->getPixel(x, z).r;
				if (height >= pos.y)
				{
					active = true;
					m_leaf = true;
				}
				m_chunk->InsertVoxelAtPos(x, height,z);
			}
		}
		if (active)
		{
			m_chunk->SetActive(active);
			leaf_count++;
			render_list.push_back(m_chunk);
		}
		return active;
	}

	for (float x = 0; x < 2; x++)
	{
		for (float y = 0; y < 2; y++)
		{
			for (float z = 0; z < 2; z++)
			{
				int childSize = m_boundingRegion.size / 2;

				m_childNodes[i] = new VoxelOctree(this);
				m_childNodes[i]->m_boundingRegion.position = m_boundingRegion.position + Vec3(x * childSize, y * childSize, z * childSize);
				m_childNodes[i]->m_boundingRegion.size = childSize;

				if (m_childNodes[i]->BuildNode())
				{
					m_childMask |= 1 << i;
					m_active = true;
				}
				else
				{
					m_childMask &= ~(1 << i);
					delete m_childNodes[i];
				}
			}
		}
	}
	return m_childMask;
}

bool inline RegionContains(Vec3 region_pos, int region_size, Vec3 object_pos)
{
	if ((object_pos.x >= region_pos.x && object_pos.x < region_pos.x + region_size)
		&& (object_pos.y >= region_pos.y && object_pos.y < region_pos.y + region_size)
		&& (object_pos.z >= region_pos.z && object_pos.z < region_pos.z + region_size))
	{
		return true;
	}

	return false;
}

void VoxelOctree::InsertVoxelAtHeight(int x, int height, int z)
{
	for (int y = 0; y <= height; y++)
	{
		VoxelOctree * node = (VoxelOctree *)this;
		Vec3 position = Vec3(x, y, z);
		bool is_leaf = false;
		while (!is_leaf)
		{
			iterations++;
			if (node->m_boundingRegion.size <= VoxelChunk::chunkSize)
			{
				is_leaf = true;
				break;
			}
			int child_index = 0;
			bool found_child = false;
			
			VoxelOctree * new_node = NULL;
			for (int i = 0; i < 2 && !found_child; i++)
			{
				for (int j = 0; j < 2 && !found_child; j++)
				{
					for (int k = 0; k < 2 && !found_child; k++)
					{						
						VoxelOctree *child = node->m_childNodes[child_index];
						if (!child)
						{
							child = new VoxelOctree(node);							
							int size = node->m_boundingRegion.size / 2;

							Vec3 child_pos = node->m_boundingRegion.position;
							child_pos.x += i * size;
							child_pos.y += j * size;
							child_pos.z += k * size;

							child->m_boundingRegion.position = child_pos;
							child->m_boundingRegion.size = size;

							node->m_childNodes[child_index] = child;
						}

						if (RegionContains(child->m_boundingRegion.position, child->m_boundingRegion.size, position))
						{							
							node = node->m_childNodes[child_index];
							found_child = true;
						}

						child_index++;
					}
				}
			}
		}

		node->m_leaf = true;
		if (!node->m_chunk)
		{
			leaf_count++;
			node->m_chunk = new VoxelChunk(node->m_boundingRegion.position);
		}
		node->m_chunk->SetVoxelActive(x, y, z);

		vox_count++;
	}
}

void VoxelOctree::UpdateTree()
{

}

void VoxelOctree::Render()
{
	m_chunk->Render();
}