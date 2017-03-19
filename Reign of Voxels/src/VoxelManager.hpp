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
	void RenderMinimap(GLuint shader, glm::vec2 &scale, glm::vec2 &position);

	VoxelChunk *CreateChunk(glm::vec3 worldPosition);
	void		destroyChunk(VoxelChunk * chunk);

	//gets the child of a node at a specific index
	VoxelOctree *getOctreeChild(VoxelOctree * currentNode, int child_index);
	VoxelOctree *createOctreeChild(VoxelOctree * currentNode, int child_index, CubeRegion &region);
	void		 destroyOctreeNode(VoxelOctree * node);

private:
	int m_worldSize; //size of one side of cubic world region
	int m_maxChunks;
	int m_maxOctNodes;

	VoxelChunk * m_chunkPool;//array of chunks
	VoxelChunk * m_freeChunkHead;//head ptr to free chunk list

	VoxelOctree *m_octreeRoot; // ptr to octree root node

	VoxelOctree * m_octreePool;//array of octrees

	Model *m_cube;
};