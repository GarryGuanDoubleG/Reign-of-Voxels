#pragma once
#include "VoxelChunk.hpp"
#include "VoxelOctree.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "Physics.hpp"

#define GRASS_RGB 200
#define WATER_RGB 159.75
#define DIRT_RBG 128

#define WATER_HEIGHT 10.0f

class VoxelManager
{
private:
	int m_worldSize; //size of one side of cubic world region
	int m_maxChunks;
	int m_maxOctNodes;

	VoxelChunk * m_chunkPool;//array of chunks
	VoxelChunk * m_freeChunkHead;//head ptr to free chunk list

	std::vector<VoxelChunk *> m_water_chunks;

	VoxelOctree *m_octreeRoot; // ptr to octree root node
	VoxelOctree *m_nextFreeNode;//linked list of inactive nodes

	std::vector<VoxelVertex> m_water_verts;
	std::vector<GLuint> m_water_tri;

	VoxelOctree *m_waterRoot;

public:
	VoxelManager(int worldSize);
	~VoxelManager();

	int GetWorldSize();

	void GenerateVoxels(Physics *physics);
	void GenerateChunks();
	void GenerateRigidBody(Physics *physics);

	bool IsWaterChunk(int x, int z);
	void GenerateWater();

	void RenderWorld(bool draw_textured, Camera * player_cam);
	void RenderWaterTexture(glm::vec4 water_plane, Camera *player_cam);
	void RenderWater(GLuint reflectionTex, GLuint refractionTex, Camera*player_cam);
	void RenderGrass(Camera * player_cam);
	void RenderVoxels(bool draw_textured, Camera * player_cam);
	void RenderVoxelTextured(Camera *player_cam);
	void RenderMinimap(Camera * player_cam);


	VoxelChunk *CreateChunk(glm::vec3 worldPosition, VoxelOctree *node);
	void		destroyChunk(VoxelChunk * chunk);

	//gets the child of a node at a specific index
	VoxelOctree *getRootNode();

	VoxelOctree *InitNode(glm::ivec3 minPos, int size);

	//free octree node
	void		 destroyOctreeNode(VoxelOctree * node);


	bool BlockWorldPosActive(glm::vec3 world_pos);
	void DestroyVoxel(glm::ivec3 world_pos, glm::ivec3 face);
};