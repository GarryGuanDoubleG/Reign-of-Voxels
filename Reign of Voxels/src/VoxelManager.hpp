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

	int GetWorldSize();

	void GenerateVoxels();

	void RenderVoxels(Camera * player_cam);
	void RenderMinimap(GLuint shader, glm::vec2 &scale, glm::vec2 &position);

	VoxelChunk *CreateChunk(glm::vec3 worldPosition);
	void		destroyChunk(VoxelChunk * chunk);

	//gets the child of a node at a specific index
	VoxelOctree *getRootNode();


	VoxelOctree *getOctreeChild(VoxelOctree * currentNode, int child_index);
	VoxelOctree *createOctreeChild(VoxelOctree *currentNode, int child_index, glm::ivec3 minPos, int size);


	//free octree node
	void		 destroyOctreeNode(VoxelOctree * node);

	void CreatePlayerStartAreas();

	bool BlockWorldPosActive(glm::vec3 world_pos);

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