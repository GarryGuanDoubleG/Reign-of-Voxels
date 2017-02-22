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
	BuildNode();

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Childnode calls is " << vox_count << std::endl;
	std::cout << "leaf nodes " << leaf_count << std::endl;
}

bool VoxelOctree::BuildNode()
{
	int size = m_boundingRegion.size;
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
	return m_childMask;
}

void VoxelOctree::UpdateTree()
{

}

void VoxelOctree::Render()
{
	m_chunk->Render();
}