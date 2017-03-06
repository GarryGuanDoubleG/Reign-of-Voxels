#pragma once
#include "VoxelChunk.hpp"
#include "VoxelOctree.hpp"
#include "camera.hpp"
#include "model.hpp"

class VoxelManager
{
public:
	VoxelManager();
	~VoxelManager();

	void GenerateVoxels();
	void RenderVoxels(Camera * player_cam);

private:
	std::vector<VoxelChunk*> m_voxelChunks;

	Model *m_voxelModel;
	Mat4 *m_modelMatrices;/**<array of matrix model positions for instance rendering */
	VoxelOctree *m_octree;

	Vec3 m_worldSize;

	void GenerateVoxelChunks(sf::Image *heightmap);
};