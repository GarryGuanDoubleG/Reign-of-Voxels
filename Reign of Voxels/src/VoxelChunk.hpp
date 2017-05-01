#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "model.hpp"
#include "3dmath.hpp"
//#include "VoxelOctree.hpp"

#define VOXEL_TYPE_AIR 0
#define VOXEL_TYPE_GRASS 2
#define VOXEL_TYPE_DIRT 4
#define VOXEL_TYPE_WOOD 8
#define VOXEL_TYPE_WATER 16
#define VOXEL_TYPE_MINERAL 32
#define VOXEL_TYPE_ENERGY 64

#define CHUNK_FLAG_INUSE 1
#define CHUNK_FLAG_ACTIVE 2
#define CHUNK_FLAG_FULL 4

class VoxelOctree;

typedef struct VoxelVertex_S
{
	VoxelVertex_S() {};
	VoxelVertex_S(glm::vec3 pos, glm::vec3 norm) : position(pos), normal(norm) {};
	VoxelVertex_S(glm::vec3 pos, glm::vec3 norm, int type) : position(pos), normal(norm), textureID(type) {};

	glm::vec3 position;
	glm::vec3 normal;
	GLint textureID;
}VoxelVertex;

class VoxelChunk
{
	friend class VoxelManager;
	friend class VoxelOctree;

private:
	std::vector<GLuint> m_tri_indices;//order to draw vertices
	std::vector<VoxelVertex> m_vertices;

	//used to stitch neighboring chunks
	std::vector<VoxelOctree *> m_seam_nodes;

	std::vector<glm::vec3> m_csgOpPos;

	GLuint	m_vao,
		m_vbo,
		m_ebo;

	sf::Uint8 m_flag;//bool for active

	VoxelChunk *m_neighbor[7];
	
public:
	bool m_render_mode;

	VoxelChunk();

	//dimensions of chunk
	static const int CHUNK_SIZE = 16;
	static const int CHUNK_SIZE_SQ = CHUNK_SIZE * CHUNK_SIZE;
	static const int CHUNK_SIZE_CUBED = CHUNK_SIZE_SQ * CHUNK_SIZE;

	void Init(glm::vec3 position, VoxelOctree * node);
	void Destroy();

	bool IsActive();
	void SetActive(bool active);

	void AssignNeighbor(VoxelChunk * neighbor, int side);

	void Render();

	void FindSeamNodes();
	void GenerateSeam();
	void DeleteSeamTree(VoxelOctree *node);

	void RebuildTree(glm::ivec3 world_pos);
	void VoxelChunk::DestroyVoxel(glm::ivec3 world_pos, glm::ivec3 face);
	void DestroyVoxel(glm::ivec3 world_pos, VoxelOctree *node);
	//free chunks have pointer to next free chunk
	//active chunks have pointer to octree node
	union
	{
		VoxelChunk * m_next;
		VoxelOctree *m_node;
	};

	//used for rendering closer chunks first
	float distToCam;
	//used for sorting chunks
	bool operator < (const VoxelChunk other) const {
		return distToCam < other.distToCam;
	}
private:
	void AddTrianglesIndices(GLuint indice);
	void AddVertices(VoxelVertex voxelVert);
	void BindMesh();

	int GetIndex(int x, int y, int z);
};
