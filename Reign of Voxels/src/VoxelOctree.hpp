#pragma once
#include "VoxelChunk.hpp"
#include "glm.hpp"
#include "SFML\Graphics.hpp"
#define MAX_QUEUED_INSERTIONS 10000

//octree flags
#define Octree_INUSE 1
#define OCTREE_ACTIVE 2
#define OCTREE_LEAF 4

typedef struct CubeRegion
{
	Vec3 position;
	int size;
};

class VoxelOctree
{
	friend class VoxelManager;
public:
	VoxelOctree();
	~VoxelOctree();

	void Render();
	
	void InitNode(VoxelOctree * parent);
	void DestroyNode();

	void InitializeOctree(sf::Image *heightmap, int size, VoxelManager *manager); // length of each side of the world
	bool BuildTree();

	//updates nodes when user interacts
	void UpdateTree();

	//create vertices for world
	void GenerateMesh(Model *cube, int i, int length);

	static const int maxHeight = 32;
	static std::vector<VoxelChunk *> render_list;//list of leaf nodes

	VoxelChunk	*m_chunk;
	VoxelOctree *m_childNodes[8];
private:
	static unsigned int m_chunkCount;
	static VoxelManager * voxelManager;

	//flag for octree state
	sf::Uint8 m_flag;

	//mask for which nodes are active
	sf::Uint8 m_childMask;
	
	union
	{
		VoxelOctree * m_parent;
		VoxelOctree * m_nextFree;
	};

	//region this node encompasses
	CubeRegion m_region;
};