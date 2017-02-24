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
	bool BuildNode();
	void UpdateTree();
	void InitializeOctree(sf::Image *heightmap, int worldSize); // length of each side of the world
	void InsertVoxelAtHeight(int x, int y, int z);
private:
	static unsigned int m_chunkCount;

	bool m_active;
	bool m_leaf;
	sf::Uint8 m_childMask;

	VoxelOctree * m_parent;
	CubeRegion m_boundingRegion;
};