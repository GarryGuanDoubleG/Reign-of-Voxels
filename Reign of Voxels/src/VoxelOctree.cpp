#include "VoxelOctree.hpp"
#include "glm.hpp"
#include "simple_logger.h"

static sf::Image *g_heightMap;
static const sf::Uint8 *g_heightarray;//array of pixel values
static unsigned int vox_count = 0;
unsigned int VoxelOctree::m_chunkCount = 0;
static const int MIN_CHUNK = 16;

VoxelOctree::VoxelOctree()
{
	m_chunkCount = 0;
}

VoxelOctree::~VoxelOctree()
{

}

void VoxelOctree::InitializeOctree(sf::Image *heightmap, int world_side)
{
	g_heightMap = heightmap;
	g_heightarray = g_heightMap->getPixelsPtr();
	//const sf::Uint8 *color_array = g_heightMap->getPixelsPtr();
	m_boundingRegion = new VoxelChunk(m_chunkCount++, world_side, Vec3(0.0f, 0.0f, 0.0f));
	
	sf::Clock build_time;
	BuildNode();

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Childnode calls is " << vox_count << std::endl;
}

bool VoxelOctree::BuildNode()
{
	int size = m_boundingRegion->getRadius();
	Vec3 pos = m_boundingRegion->getPosition();
	bool active = false;

	int child_radius = m_boundingRegion->getRadius() / 2;
	int i = 0;
	vox_count++;
	
	if (size <= MIN_CHUNK)
	{
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
				m_boundingRegion->InsertVoxelAtPos(pos.x + x, height, pos.z + z);
			}
		}
	}

	for (float x = 0; x < 2; x++)
	{
		for (float y = 0; y < 2; y++)
		{
			for (float z = 0; z < 2; z++)
			{
				Vec3 chunk_pos = Vec3(x * child_radius, y * child_radius, z * child_radius);

				m_childNodes[i] = new VoxelOctree();
				m_childNodes[i]->m_boundingRegion = new VoxelChunk(m_chunkCount++, child_radius, chunk_pos);
				m_childNodes[i]->BuildNode();
			}
		}
	}

}

void VoxelOctree::UpdateTree()
{

}