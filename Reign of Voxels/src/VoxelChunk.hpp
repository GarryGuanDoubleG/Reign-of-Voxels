#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "model.hpp"
#include "glm.hpp"

#define VOXEL_TYPE_AIR 1
#define VOXEL_TYPE_GRASS 2
#define VOXEL_TYPE_DIRT 4
#define VOXEL_TYPE_WOOD 8
#define VOXEL_TYPE_WATER 16
#define VOXEL_TYPE_MINERAL 32
#define VOXEL_TYPE_ENERGY 64

#define CHUNK_FLAG_INUSE 1
#define CHUNK_FLAG_ACTIVE 2
#define CHUNK_FLAG_FULL 4

typedef struct
{
	sf::Uint8 position[3]; /**< vertex postion in ndc */
	sf::Uint8 normal[3]; /**< normal value of vertex */
	sf::Uint8 uv[2]; /**< texture coordinates */
}VoxelVertex;

class VoxelChunk
{
	friend class VoxelManager;
public:
	VoxelChunk();

	//dimensions of chunk
	static const int CHUNK_SIZE = 16;
	static const int CHUNK_SIZE_SQ = CHUNK_SIZE * CHUNK_SIZE;
	static const int CHUNK_SIZE_CUBED = CHUNK_SIZE_SQ * CHUNK_SIZE;

	void Init(Vec3 position);
	void Destroy();

	Vec3 getPosition();
	Vec3 getSize();

	bool isActive();
	void SetActive(bool active);

	void VoxelChunk::AddLeftFace(int x, int y, int z);
	void VoxelChunk::AddBottomFace(int x, int y, int z);
	void VoxelChunk::AddFrontFace(int x, int y, int z);
	void VoxelChunk::AddRightFace(int x, int y, int z);
	void VoxelChunk::AddTopFace(int x, int y, int z);
	void VoxelChunk::AddBackFace(int x, int y, int z);

	void GenerateMesh(Model *cube);
	void BindMesh();

	void ClearVertices();

	void InsertVoxelAtPos(int x, int y, int z);

	void SetVoxelActive(int x, int y, int z);

	void Render();
	void RenderMinimap();

	std::vector<Vertex> m_vertices;
	std::vector<Vertex> m_top_verts;

	VoxelChunk * m_next;
private:
	void AddTrianglesIndices();
	void AddMinimapIndices();

	int GetIndex(int x, int y, int z);

	std::vector<GLuint> m_tri_indices;//order to draw vertices
	std::vector<GLuint> m_mp_indices;

	GLuint	m_vao, 
			m_vbo, 
			m_ebo;

	//minimap
	GLuint m_mp_vao,
		m_mp_vbo,
		m_mp_ebo;

	Vec3 m_position;

	sf::Uint8 m_flag;//bool for active

	sf::Uint8 m_voxels[CHUNK_SIZE_CUBED];
};
