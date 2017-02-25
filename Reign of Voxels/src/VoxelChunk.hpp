#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>
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
	void SetActive(bool active);

	void GenerateMesh();
	void InsertVoxelAtPos(int x, int y, int z);
	void SetVoxelActive(int x, int y, int z);
	void Render();

	static const int chunkSize = 16;//size of each side of the chunk
private:
	void AddTrianglesIndices();

	GLuint m_vao, m_vbo, m_ebo;

	bool m_active;
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_tri_indices;

	Vec3 m_position;
	VoxelBlock m_voxels[chunkSize][chunkSize][chunkSize];
};
