#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "model.hpp"
#include "3dmath.hpp"

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
	static const int CHUNK_SIZE = 32;
	static const int CHUNK_SIZE_SQ = CHUNK_SIZE * CHUNK_SIZE;
	static const int CHUNK_SIZE_CUBED = CHUNK_SIZE_SQ * CHUNK_SIZE;

	void Init(glm::vec3 position);
	void Destroy();

	glm::vec3 getPosition();
	glm::vec3 getSize();

	bool IsActive();
	void SetActive(bool active);

	void AssignNeighbor(VoxelChunk * neighbor, int side);

	void GenerateMesh();
	void ClearMeshData();
	void BindMesh();

	void InsertVoxelAtPos(int x, int y, int z);
	void SetVoxelActive(int x, int y, int z);

	void Render();
	void RenderMinimap();

	sf::Uint8 GetVoxel(glm::ivec3 local_pos);

	void ClearPlayerStart(float height);
	void SmoothTerrain(float scale, glm::vec3 origin);

	std::vector<Vertex> m_vertices;
	std::vector<Vertex> m_top_verts;

	VoxelChunk * m_next;

	//used for rendering closer chunks first
	float distToCam;
	//used for sorting chunks
	bool operator < (const VoxelChunk other) const {
		return distToCam < other.distToCam;
	}
private:
	void AddTrianglesIndices();
	void AddMinimapIndices();

	void AddLeftFace(int x, int y, int z);
	void AddBottomFace(int x, int y, int z);
	void AddFrontFace(int x, int y, int z);
	void AddRightFace(int x, int y, int z);
	void AddTopFace(int x, int y, int z);
	void AddBackFace(int x, int y, int z);

	int GetIndex(int x, int y, int z);

	std::vector<GLuint> m_tri_indices;//order to draw vertices
	std::vector<GLuint> m_mp_indices;//indices for minimap

	int m_indices_count; /**< number of indices for world */
	int m_mp_indices_count; /**< number of indices for minimap */

	GLuint	m_vao, 
			m_vbo, 
			m_ebo;

	//minimap
	GLuint m_mp_vao,
		m_mp_vbo,
		m_mp_ebo;

	glm::vec3 m_position;

	sf::Uint8 m_flag;//bool for active
	sf::Uint8 m_voxels[CHUNK_SIZE_CUBED];

	VoxelChunk *m_neighbor[6];
};
