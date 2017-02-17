#pragma once
#include "VoxelChunk.hpp"
#include "Voxel.hpp"
#include "model.hpp"

class VoxelManager
{
public:
	VoxelManager();
	~VoxelManager();

	void GenerateVoxels();
	void RenderVoxels();

private:
	Model *m_voxelModel;
	Mat4 *m_modelMatrices;/**<array of matrix model positions for instance rendering */

	int m_worldSize;
};