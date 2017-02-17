#pragma once
#include "VoxelChunk.hpp"
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

	const int MIN_SIZE = 1;

	void UpdateTree();
	void InitializeOctree();
private:
	 bool m_pendingInsertions;
	 bool m_treeBuilt;
};