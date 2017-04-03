#pragma once
#include "VoxelChunk.hpp"
#include "3dmath.hpp"
#include "SFML\Graphics.hpp"
#define MAX_QUEUED_INSERTIONS 10000

//octree flags
#define OCTREE_INUSE 1
#define OCTREE_ACTIVE 2
#define OCTREE_LEAF 4

class VoxelOctree
{
	friend class VoxelManager;
public:
	VoxelOctree();
	
	void InitNode(CubeRegion region);
	void DestroyNode();
	
	void InitializeChildren();
	void InitializeOctree(sf::Image *heightmap, int size, VoxelManager *manager); // length of each side of the world
	
	bool BuildTree();

	//create vertices for world
	void GenerateMesh(int i, int length);
	void GenerateWorldMesh();

	VoxelOctree * FindLeafNode(glm::vec3 pos);


	void		  SmoothTerrain(float scale, glm::vec3 pos);
	void		  CreatePlayerStart(glm::vec3 pos);

	static const int maxHeight = 32;
	static std::vector<VoxelChunk *> render_list;//list of leaf nodes
	static void SortRenderList(glm::vec3 camera_pos);//closest chunks render first
	//leaf node
	VoxelChunk	*m_chunk;

	int index;
private:
	static unsigned int m_chunkCount;
	static VoxelManager * voxelManager;

	//flag for octree state
	sf::Uint8 m_flag;

	//mask for which nodes are active
	sf::Uint8 m_childMask;

	//region this node encompasses
	CubeRegion m_region;
};