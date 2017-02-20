#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

VoxelManager::VoxelManager()
{
	m_voxelModel = new Model("Resources\\models\\cube.obj");
	if (m_voxelModel)
	{
		GenerateVoxels();
	}
}
VoxelManager::~VoxelManager()
{

}
void VoxelManager::GenerateVoxels()
{
	m_worldSize = 512;
	int chunk_size = 16;
	int voxel_amount = 512 * 512 * 512 / chunk_size;
	int chunk_dimen = m_worldSize / chunk_size;

	//LoadTexture(GeneratePerlin());
}

void VoxelManager::RenderVoxels()
{
	glPolygonMode(GL_FRONT, GL_LINE);
	m_voxelModel->Draw(g_shader_prog);
	glPolygonMode(GL_FRONT, GL_FILL);
}