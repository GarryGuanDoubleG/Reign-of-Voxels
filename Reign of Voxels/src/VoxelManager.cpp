#include "VoxelManager.hpp"
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
	int voxel_amount = 512 * 512 * 512 / VoxelChunk::CHUNK_SIZE_CB;
	int total_chunks = m_worldSize / VoxelChunk::CHUNK_SIZE;
	if (m_voxelModel)
	{
		m_modelMatrices = new Mat4[voxel_amount];
		unsigned int count = 0;

		for (GLfloat x = 0; x < total_chunks; x++)
		{
			for (GLfloat y = 0; y < total_chunks; y++)
			{
				for (GLfloat z = 0; z < total_chunks; z++)
				{
					Mat4 model;
					model = glm::scale(model, Vec3(VoxelChunk::CHUNK_SIZE, VoxelChunk::CHUNK_SIZE, VoxelChunk::CHUNK_SIZE));
					model = glm::translate(model, Vec3(x, y, z));

					m_modelMatrices[count++] = model;
				}
			}
		}
		slog("Count is %u", count);
	}

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, voxel_amount * sizeof(glm::mat4), &m_modelMatrices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_voxelModel->SetInstanceRendering(buffer, voxel_amount);
}

void VoxelManager::RenderVoxels()
{
	glPolygonMode(GL_FRONT, GL_LINE);
	m_voxelModel->Draw(g_shader_prog);
	glPolygonMode(GL_FRONT, GL_FILL);
}