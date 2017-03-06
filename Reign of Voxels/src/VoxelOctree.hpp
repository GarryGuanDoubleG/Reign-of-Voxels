#pragma once
#include "VoxelChunk.hpp"
#include "glm.hpp"
#include "SFML\Graphics.hpp"
#define MAX_QUEUED_INSERTIONS 10000

typedef struct CubeRegion
{
	Vec3 position;
	int size;
};

class VoxelOctree
{
public:
	VoxelOctree(VoxelOctree * parent);
	~VoxelOctree();

	static std::vector<VoxelChunk *> render_list;//list of leaf nodes
		
	VoxelChunk *m_chunk;
	VoxelOctree *m_childNodes[8];

	void Render();
	
	void InitializeOctree(sf::Image *heightmap, Vec3 size); // length of each side of the world
	bool BuildNode();

	//updates nodes when user interacts
	void UpdateTree();
	
	//inserts voxel and actives all voxels below it
	void InsertVoxelAtHeight(int x, int y, int z);

	//create vertices for world
	void GenerateMesh(Model *cube, int i, int length);
private:
	static unsigned int m_chunkCount;

	bool m_active;
	bool m_leaf;

	//mask for which nodes are active
	sf::Uint8 m_childMask;
	
	VoxelOctree * m_parent;

	//region this node encompasses
	CubeRegion m_region;
};