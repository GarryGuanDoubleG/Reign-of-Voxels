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


	VoxelChunk *m_chunk;
	VoxelOctree *m_childNodes[8];

	void Render();
	bool BuildNode();
	void UpdateTree();
	void InitializeOctree(sf::Image *heightmap, int worldSize); // length of each side of the world
private:
	static std::vector<VoxelOctree *> render_list;//list of leaf nodes
	static unsigned int m_chunkCount;

	bool m_active;
	bool m_leaf;
	sf::Uint8 m_childMask;

	VoxelOctree * m_parent;
	CubeRegion m_boundingRegion;
};