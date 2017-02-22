#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

VoxelManager::VoxelManager()
{
	m_voxelModel = new Model("Resources\\models\\cube.obj");
}
VoxelManager::~VoxelManager()
{

}

void VoxelManager::GenerateVoxelChunks(sf::Image *heightmap)
{
	int chunk_size = 16;
	int scale = 10;
	int max_height = 512;
	int world_size = heightmap->getSize().x;
	int chunk_count = 0;

}
void VoxelManager::GenerateVoxels()
{
	m_worldSize = 512;
	int chunk_size = 16;
	int voxel_amount = 512 * 512 * 512 / chunk_size;
	int chunk_dimen = m_worldSize / chunk_size;

	sf::Image *heightmap = new sf::Image();
	if (!heightmap->loadFromFile(GenerateTerrainMap(m_worldSize)))
		slog("Failed to Load or Generate HeightMap");
	else
	{
		//GenerateVoxelChunks(heightmap);
		m_octree = new VoxelOctree(NULL);
		m_octree->InitializeOctree(heightmap, m_worldSize);
	}
	delete heightmap;
}

void VoxelManager::RenderVoxels()
{
	glPolygonMode(GL_FRONT, GL_LINE);
	m_voxelModel->Draw(g_shader_prog);
	glPolygonMode(GL_FRONT, GL_FILL);
}