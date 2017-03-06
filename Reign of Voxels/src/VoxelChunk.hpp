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

	std::vector<Vertex> m_vertices;

	//dimensions of chunk
	static Vec3 CHUNK_SIZE;

	Vec3 getPosition();
	Vec3 getSize();

	bool isActive();
	void SetActive(bool active);

	void GenerateMesh();
	void BindMesh();

	void InsertVoxelAtPos(int x, int y, int z);

	void SetVoxelActive(int x, int y, int z);

	void Render();

private:
	int GetVoxelIndex(int x, int y, int z);
	void AddTrianglesIndices();

	std::vector<GLuint> m_tri_indices;
	GLuint m_vao, m_vbo, m_ebo;

	bool m_active;

	Vec3 m_position;

	VoxelBlock m_voxels[16][16][16];
};
