#include "VoxelOctree.hpp"
#include "glm.hpp"
#include "simple_logger.h"

static sf::Image *g_heightMap;
static const sf::Uint8 *g_heightarray;//array of pixel values
static unsigned int vox_count = 0;
unsigned int VoxelOctree::m_chunkCount = 0;
static const int MIN_CHUNK = 16;
std::vector<VoxelOctree *> VoxelOctree::render_list; //list of leaf nodes
int leaf_count = 0;

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

	for (int x = 0; x < worldSize; x++)
	{
		for (int z = 0; z < worldSize; z++)
		{
			int height = g_heightMap->getPixel(x, z).r;
			InsertVoxelAtHeight(x, height, z);
		}
	}

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Childnode calls is " << vox_count << std::endl;
	std::cout << "leaf nodes " << leaf_count << std::endl;
}

bool VoxelOctree::BuildNode()
{
	/*int size = m_boundingRegion.size;
	Vec3 pos = m_boundingRegion.position;
	bool active = false;

	int i = 0;
	vox_count++;
	
	if (size <= MIN_CHUNK)
	{
		if(!m_chunk)
			m_chunk = new VoxelChunk(m_boundingRegion.position);

		for (int x = 0; x < size; x++)
		{
			for (int z = 0; z < size; z++)
			{
				float height = g_heightMap->getPixel(x, z).r;
				if (height >= pos.y)
				{
					active = true;
					m_leaf = true;
				}
				int y = height - pos.y;
				m_chunk->InsertVoxelAtPos(pos.x + x, pos.y + y, pos.z + z);
			}
		}
		if(active) leaf_count++;
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
	return m_childMask;*/
	return false;
}

void VoxelOctree::InsertVoxelAtHeight(int x, int height, int z)
{
	for (int y = 0; y <= height; y++)
	{
		Vec3 position = Vec3(x, y, z);
		bool is_leaf = false;
		VoxelOctree * node = (VoxelOctree *)this;

		while (!is_leaf)
		{
			int size = node->m_boundingRegion.size;
			if (size <= MIN_CHUNK)
			{
				is_leaf = true;
				break;
			}
			int local_x = (position.x - node->m_boundingRegion.position.x) / size;
			int local_y = (position.y - node->m_boundingRegion.position.y) / size;
			int local_z = (position.z - node->m_boundingRegion.position.z) / size;
			Vec3 local_octant_pos = Vec3(local_x, local_y, local_z);
			local_octant_pos += node->m_boundingRegion.position;
			local_octant_pos *= size;

			int child_index = local_octant_pos.x / size 
								+ 2 * (local_octant_pos.y / size) 
								+ 4 * (local_octant_pos.z / size);

			if (!node->m_childNodes[child_index])
			{
				node->m_childNodes[child_index] = new VoxelOctree(node);
				node->m_childNodes[child_index]->m_boundingRegion.size = node->m_boundingRegion.size / 2;
				node->m_childNodes[child_index]->m_boundingRegion.position = local_octant_pos;
			}
			node = node->m_childNodes[child_index];
		}

		node->m_leaf = is_leaf;
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