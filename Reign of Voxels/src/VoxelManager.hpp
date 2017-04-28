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

	void RenderWorld(bool draw_textured, Camera * player_cam);
	void RenderGrass(Camera * player_cam);
	void RenderVoxels(bool draw_textured, Camera * player_cam);
	void RenderMinimap(GLuint shader, glm::vec2 &scale, glm::vec2 &position);
	void RenderVoxelTextured(Camera *player_cam);

	VoxelChunk *CreateChunk(glm::vec3 worldPosition);
	void		destroyChunk(VoxelChunk * chunk);

	//gets the child of a node at a specific index
	VoxelOctree *getRootNode();

	VoxelOctree *InitNode(glm::ivec3 minPos, int size);

	//free octree node
	void		 destroyOctreeNode(VoxelOctree * node);

	bool BlockWorldPosActive(glm::vec3 world_pos);

private:

	int m_resolution; //size of one side of cubic world region
	int m_maxChunks;
	int m_maxOctNodes;

	VoxelChunk * m_chunkPool;//array of chunks
	VoxelChunk * m_freeChunkHead;//head ptr to free chunk list

	VoxelOctree *m_octreeRoot; // ptr to octree root node
	VoxelOctree *m_nextFreeNode;//linked list of inactive nodes

	Model *m_cube;
};