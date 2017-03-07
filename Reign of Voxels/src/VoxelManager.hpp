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

	VoxelChunk *createChunk(Vec3 worldPosition);
	void		destroyChunk(VoxelChunk * chunk);

	void		createOctreeRoot(VoxelOctree * parent);
	void		 destroyOctreeNode(VoxelOctree * node);

	//gets the child of a node at a specific index
	VoxelOctree *getOctreeChild(VoxelOctree * caller, int child_index);
	VoxelOctree *createOctreeChild();

private:
	int m_worldSize; //size of one side of cubic world region

	VoxelChunk * m_chunkPool;//array of chunks
	VoxelChunk * m_freeChunkHead;//head ptr to free chunk list

	VoxelOctree *m_octreeRoot; // ptr to octree root node

	VoxelOctree * m_octreePool;//array of octrees
	VoxelOctree * m_freeOctreeHead;//ptr to first available octree node
};