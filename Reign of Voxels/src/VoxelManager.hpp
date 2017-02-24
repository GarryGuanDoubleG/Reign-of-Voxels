#pragma once
#include "VoxelChunk.hpp"
#include "VoxelOctree.hpp"
#include "model.hpp"

class VoxelManager
{
public:
	VoxelManager();
	~VoxelManager();

	void GenerateVoxels();
	void RenderVoxels(GLuint shader);

private:
	std::vector<VoxelChunk*> m_voxelChunks;

	Model *m_voxelModel;
	Mat4 *m_modelMatrices;/**<array of matrix model positions for instance rendering */
	VoxelOctree *m_octree;

	int m_worldSize;

	void GenerateVoxelChunks(sf::Image *heightmap);
};