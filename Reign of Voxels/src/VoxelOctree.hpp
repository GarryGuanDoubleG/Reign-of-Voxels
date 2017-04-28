#pragma once
#include "VoxelChunk.hpp"
#include "3dmath.hpp"
#include "qef.hpp"
#include "SFML\Graphics.hpp"
#define MAX_QUEUED_INSERTIONS 10000

//octree flags
#define OCTREE_INUSE 1
#define OCTREE_ACTIVE 2
#define OCTREE_LEAF 4

#define GRASS 0
#define WATER 1
#define SNOW 2
#define DIRT 3

enum OctreeNodeType
{
	Node_None,
	Node_Internal,
	Node_Psuedo,
	Node_Leaf,
};


typedef struct VoxelVertex_S
{
	VoxelVertex_S() {};
	VoxelVertex_S(glm::vec3 pos, glm::vec3 norm) : position(pos), normal(norm) {};
	VoxelVertex_S(glm::vec3 pos, glm::vec3 norm, int type) : position(pos), normal(norm), textureID(type) {};

	glm::vec3 position;
	glm::vec3 normal;
	GLint textureID;
}VoxelVertex;


struct OctreeDrawInfo
{
	OctreeDrawInfo()
		: index(-1)
		, corners(0)
	{
	}

	int				index;
	int				corners;
	GLint			type;
	glm::vec3		position;
	glm::vec3		averageNormal;
	svd::QefData	qef;
};

class VoxelOctree
{
	friend class VoxelManager;
private:
	static unsigned int m_chunkCount;
	static VoxelManager * voxelManager;

	//flag for octree state
	sf::Uint8 m_flag;

	//mask for which nodes are active
	sf::Uint8 m_childMask;

	//region this node encompasses
	glm::ivec3 m_min;
	int m_size;

	//leaf node
	VoxelChunk	*m_chunk;
	OctreeNodeType m_type;

	OctreeDrawInfo * m_drawInfo;

	//active nodes have children
	//inactive are added to free node linked list
	union
	{
		VoxelOctree *m_children[8];
		VoxelOctree *next_free; 
	};
public:
	VoxelOctree();
	
	void InitNode(glm::ivec3 minPos, int size);
	void DestroyNode();
	
	void InitChildren();
	void InitOctree(int size, VoxelManager *manager); // length of each side of the world
	
	void AssignNeighbors();//assign neighbors to chunks

	//create vertices for world
	void GenerateMesh(int i, int length);
	void GenerateWorldMesh();

	VoxelOctree * FindLeafNode(glm::vec3 pos);
	VoxelChunk *  FindLeafChunk(glm::vec3 pos);

	static const int maxHeight = 64;
	static std::vector<VoxelChunk *> render_list;//list of leaf nodes
	static void SortRenderList(glm::vec3 camera_pos);//closest chunks render first

private:
	int AddTerrainType(const OctreeDrawInfo *drawInfo);
	void GenerateMeshFromOctree();
	void GenerateVertexIndices();
	void ContourCellProc();
	void ContourProcessEdge(VoxelOctree* node[4], int dir);
	void ContourEdgeProc(VoxelOctree* node[4], int dir);
	void ContourFaceProc(VoxelOctree* node[2], int dir);
	
	void UploadMesh();
	void UploadGrass();

	void Draw();
	void DrawGrass();

	glm::vec3 CalculateSurfaceNormal(const glm::vec3 &pos);

	bool BuildTree();
	bool BuildLeafNode();
};