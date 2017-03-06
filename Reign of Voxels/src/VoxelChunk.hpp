#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "model.hpp"
#include "VoxelBlock.hpp"
#include "glm.hpp"

class VoxelChunk
{
public:
	VoxelChunk(Vec3 position);
	~VoxelChunk();

	std::vector<Vertex> m_vertices;

	//dimensions of chunk
	static const int CHUNK_SIZE = 16;
	static const int CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

	Vec3 getPosition();
	Vec3 getSize();

	bool isActive();
	void SetActive(bool active);

	void GenerateMesh(Model *cube);
	void BindMesh();

	void InsertVoxelAtPos(int x, int y, int z);

	void SetVoxelActive(int x, int y, int z);

	void Render();

private:
	void AddTrianglesIndices();


	std::vector<GLuint> m_tri_indices;

	GLuint	m_vao, 
			m_vbo, 
			m_ebo;

	bool m_active;

	Vec3 m_position;

	int m_voxel_count;

	VoxelBlock m_voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
};
