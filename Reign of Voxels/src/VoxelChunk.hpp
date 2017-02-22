#pragma once
#include "VoxelBlock.hpp"
#include "glm.hpp"

class VoxelChunk
{
public:
	VoxelChunk(Vec3 position);
	~VoxelChunk();

	Vec3 getPosition();

	int getSize();
	bool isActive();

	void GenerateMesh();
	void InsertVoxelAtPos(int x, int y, int z);

	void Render();
private:
	bool m_active;

	int m_index; /**<index into model matrix*/
	Vec3 m_size;

	static const int chunkSize = 16;//size of each side of the chunk

	Vec3 m_position;
	VoxelBlock m_voxels[chunkSize][chunkSize][chunkSize];
};
