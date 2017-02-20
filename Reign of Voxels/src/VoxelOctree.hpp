#pragma once
#include "VoxelChunk.hpp"
#include "glm.hpp"
#include "SFML\Graphics.hpp"
#define MAX_QUEUED_INSERTIONS 10000

class VoxelOctree
{
public:
	VoxelOctree();
	~VoxelOctree();

	VoxelChunk *m_boundingRegion;	
	VoxelOctree *m_childNodes[8];
	sf::Int8 m_activeNodes; //bitmask calculation

	//static const int MIN_CHUNK;

	void BuildNode();
	void UpdateTree();
	void InitializeOctree(sf::Image *heightmap, int world_side); // length of each side of the world
private:
	bool is_leaf;

	std::vector<VoxelChunk *> render_list;//list of leaf nodes
	 bool m_pendingInsertions;
	 bool m_treeBuilt;
	 static unsigned int m_chunkCount;
};