#include <thread>
#include <mutex>
#include <math.h>

#include "VoxelOctree.hpp"
#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"

#include "density.hpp"
#include "qef.hpp"
#include "3dmath.hpp"
#include "simple_logger.h"


std::vector<VoxelChunk *> VoxelOctree::render_list; //list of leaf nodes
static std::mutex g_render_list_mutex;

VoxelManager * VoxelOctree::voxelManager;

std::vector<glm::vec3> g_terrainGrassPos;

const int MATERIAL_AIR = 0;
const int MATERIAL_SOLID = 1;

const float QEF_ERROR = 1e-6f;
const int QEF_SWEEPS = 4;

//terrain
static GLuint g_grassVAO, g_grassQuadVBO, g_grassPosVBO;

// ----------------------------------------------------------------------------

const glm::ivec3 CHILD_MIN_OFFSETS[] =
{
	// needs to match the vertMap from Dual Contouring impl
	glm::ivec3(0, 0, 0),
	glm::ivec3(0, 0, 1),
	glm::ivec3(0, 1, 0),
	glm::ivec3(0, 1, 1),
	glm::ivec3(1, 0, 0),
	glm::ivec3(1, 0, 1),
	glm::ivec3(1, 1, 0),
	glm::ivec3(1, 1, 1),
};

// ----------------------------------------------------------------------------
// data from the original DC impl, drives the contouring process

const int edgevmap[12][2] =
{
	{ 0,4 },{ 1,5 },{ 2,6 },{ 3,7 },	// x-axis 
	{ 0,2 },{ 1,3 },{ 4,6 },{ 5,7 },	// y-axis
	{ 0,1 },{ 2,3 },{ 4,5 },{ 6,7 }		// z-axis
};

const int edgemask[3] = { 5, 3, 6 };

const int vertMap[8][3] =
{
	{ 0,0,0 },
	{ 0,0,1 },
	{ 0,1,0 },
	{ 0,1,1 },
	{ 1,0,0 },
	{ 1,0,1 },
	{ 1,1,0 },
	{ 1,1,1 }
};

const int faceMap[6][4] = { { 4, 8, 5, 9 },{ 6, 10, 7, 11 },{ 0, 8, 1, 10 },{ 2, 9, 3, 11 },{ 0, 4, 2, 6 },{ 1, 5, 3, 7 } };
const int cellProcFaceMask[12][3] = { { 0,4,0 },{ 1,5,0 },{ 2,6,0 },{ 3,7,0 },{ 0,2,1 },{ 4,6,1 },{ 1,3,1 },{ 5,7,1 },{ 0,1,2 },{ 2,3,2 },{ 4,5,2 },{ 6,7,2 } };
const int cellProcEdgeMask[6][5] = { { 0,1,2,3,0 },{ 4,5,6,7,0 },{ 0,4,1,5,1 },{ 2,6,3,7,1 },{ 0,2,4,6,2 },{ 1,3,5,7,2 } };

const int faceProcFaceMask[3][4][3] = {
	{ { 4,0,0 },{ 5,1,0 },{ 6,2,0 },{ 7,3,0 } },
	{ { 2,0,1 },{ 6,4,1 },{ 3,1,1 },{ 7,5,1 } },
	{ { 1,0,2 },{ 3,2,2 },{ 5,4,2 },{ 7,6,2 } }
};

const int faceProcEdgeMask[3][4][6] = {
	{ { 1,4,0,5,1,1 },{ 1,6,2,7,3,1 },{ 0,4,6,0,2,2 },{ 0,5,7,1,3,2 } },
	{ { 0,2,3,0,1,0 },{ 0,6,7,4,5,0 },{ 1,2,0,6,4,2 },{ 1,3,1,7,5,2 } },
	{ { 1,1,0,3,2,0 },{ 1,5,4,7,6,0 },{ 0,1,5,0,4,1 },{ 0,3,7,2,6,1 } }
};

const int edgeProcEdgeMask[3][2][5] = {
	{ { 3,2,1,0,0 },{ 7,6,5,4,0 } },
	{ { 5,1,4,0,1 },{ 7,3,6,2,1 } },
	{ { 6,4,2,0,2 },{ 7,5,3,1,2 } },
};

const int processEdgeMask[3][4] = { { 3,2,1,0 },{ 7,5,6,4 },{ 11,10,9,8 } };

VoxelOctree::VoxelOctree()
	: m_flag(0),
	m_childMask(0)
{
}


void VoxelOctree::InitNode(glm::ivec3 minPos, int size)
{
	if (m_flag & OCTREE_INUSE)
	{
		std::cout << "Already using node ?" << std::endl;
	}

	m_flag = OCTREE_INUSE;

	m_childMask = 0;
	m_min = minPos;
	m_size = size;

	m_chunk = NULL;

	memset(m_children, 0, sizeof(m_children));
}


bool VoxelOctree::AssignLeafNode(VoxelOctree *node)
{
	VoxelOctree *currNode = this;

	bool found = false;
	glm::ivec3 pos = node->m_min;

	while (!found)
	{
		int index = 0;

		//check which child the next node is in 
		int child_size = currNode->m_size >> 1;
		//octal search
		int x = pos.x >= currNode->m_min.x && pos.x < currNode->m_min.x + child_size ? 0 : 1;
		int y = pos.y >= currNode->m_min.y && pos.y < currNode->m_min.y + child_size ? 0 : 1;
		int z = pos.z >= currNode->m_min.z && pos.z < currNode->m_min.z + child_size ? 0 : 1;

		//calculate index 
		index = 4 * x + 2 * y + z;

		if (!currNode->m_children[index])
		{
			if (child_size == 1)
			{
				currNode->m_children[index] = node;
				currNode->m_childMask |= 1 << index;
				return true;
			}

			currNode->m_children[index] = voxelManager->InitNode(pos + (child_size * CHILD_MIN_OFFSETS[index]), child_size);
			currNode->m_children[index]->m_type = Node_Internal;
			currNode->m_children[index]->m_flag |= OCTREE_ACTIVE;
			currNode->m_childMask |= 1 << index;
		}

		currNode = currNode->m_children[index];

		if (child_size == 1)
			return false;
	}

	return false;
}

void VoxelOctree::DestroyNode()
{
	if (!this)
	{
		return;
	}

	m_flag = 0;

	voxelManager->destroyOctreeNode(this);

	if (m_chunk)
	{
		m_chunk->Destroy();
		voxelManager->destroyChunk(m_chunk);
		m_chunk = NULL;
	}

	if (m_drawInfo)
	{
		delete m_drawInfo;
		m_drawInfo = NULL;
	}
}

void VoxelOctree::SortRenderList(glm::vec3 camera_pos)
{
	//update chunk's dist to cam
	int threads = 8;

	for (int i = 0; i < render_list.size(); i++)
		render_list[i]->distToCam = glm::distance(camera_pos, glm::vec3(render_list[i]->m_node->m_min));

	//sort by distance
	std::sort(render_list.begin(), render_list.end());
}

void VoxelOctree::InitOctree(int worldSize, VoxelManager *manager)
{
	voxelManager = manager;

	sf::Clock build_time; //profiling

	this->BuildTree();
	//this->SimplifyOctree(-0.5f);
	this->AssignNeighbors();
	this->GenerateMeshFromOctree();
	this->GenerateSeams();
	this->UploadMesh();
	this->UploadGrass();

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
}

void VoxelOctree::InitChildren()
{
	int childSize = m_size >> 1;


	for (int i = 0; i < 8; i++)
	{
		if (!m_children[i])
		{
			glm::ivec3 childMinPos = m_min + CHILD_MIN_OFFSETS[i] * childSize;

			m_children[i] = voxelManager->InitNode(childMinPos, childSize);
		}
	}

}

int VoxelOctree::AddTerrainType(const OctreeDrawInfo *drawInfo)
{
	GLint type = GRASS;
	sf::Color terrainColor = GetPerlinColorValue(drawInfo->position.x, drawInfo->position.z);

	if (terrainColor == sf::Color(224, 224, 0))
	{
		type = DIRT;
		//flat area
		if (drawInfo->averageNormal.y > .95f)
		{
			g_terrainGrassPos.push_back(drawInfo->position);
		}
	}
	else if (terrainColor.b > 200)
	{
		type = WATER;
	}

	return type;
}

bool VoxelOctree::BuildLeafNode()
{
	int corners = 0;

	for (int i = 0; i < 8; i++)
	{
		glm::ivec3 cornerPos = m_min + CHILD_MIN_OFFSETS[i];
		const float density = Density_Func(glm::vec3(cornerPos));
		const int material = density < 0.f ? MATERIAL_SOLID : MATERIAL_AIR;
		corners |= (material << i);

		m_densities[i] = density;
	}

	if (corners == 0 || corners == 255)
	{
		// voxel is full inside or outside the volume
		//m_type = Node_Leaf;
		m_flag &= ~(OCTREE_ACTIVE);

		return false;
	}

	//build leaf node if node contains isosurface
	const int MAX_CROSSINGS = 6;
	int edgeCount = 0;
	glm::vec3 averageNormal(0.f);
	svd::QefSolver qef;

	for (int i = 0; i < 12 && edgeCount < MAX_CROSSINGS; i++)
	{
		const int c1 = edgevmap[i][0];
		const int c2 = edgevmap[i][1];

		const int m1 = (corners >> c1) & 1;
		const int m2 = (corners >> c2) & 1;

		if ((m1 == MATERIAL_AIR && m2 == MATERIAL_AIR) ||
			(m1 == MATERIAL_SOLID && m2 == MATERIAL_SOLID))
		{
			continue; //no change in sign density
		}

		const glm::vec3 p1 = glm::vec3(m_min + CHILD_MIN_OFFSETS[c1]);
		const glm::vec3 p2 = glm::vec3(m_min + CHILD_MIN_OFFSETS[c2]);

		const glm::vec3 p = ApproximateZeroCrossingPosition(p1, p2);
		const glm::vec3 n = CalculateSurfaceNormal(p);

		qef.add(p.x, p.y, p.z, n.x, n.y, n.z);

		averageNormal += n;

		edgeCount++;
	}

	svd::Vec3 qefPosition;
	qef.solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);

	OctreeDrawInfo* drawInfo = new OctreeDrawInfo;
	drawInfo->position = glm::vec3(qefPosition.x, qefPosition.y, qefPosition.z);
	drawInfo->qef = qef.getData();

	const glm::vec3 min = glm::vec3(m_min);
	const glm::vec3 max = glm::vec3(m_min + glm::ivec3(m_size));

	if (drawInfo->position.x < min.x || drawInfo->position.x > max.x ||
		drawInfo->position.y < min.y || drawInfo->position.y > max.y ||
		drawInfo->position.z < min.z || drawInfo->position.z > max.z)
	{
		const auto& mp = qef.getMassPoint();
		drawInfo->position = glm::vec3(mp.x, mp.y, mp.z);
	}

	drawInfo->averageNormal = glm::normalize(averageNormal / (float)edgeCount);
	drawInfo->corners = corners;

	drawInfo->type = AddTerrainType(drawInfo);

	m_type = Node_Leaf;
	
	if (m_drawInfo)
	{
		delete m_drawInfo;
	}

	m_drawInfo = drawInfo;

	m_flag |= OCTREE_ACTIVE;
	m_flag |= OCTREE_LEAF;

	return true;
}

bool VoxelOctree::BuildLeafNode(const glm::vec3 csgOperation)
{
	int corners = 0;

	for (int i = 0; i < 8; i++)
	{
		glm::ivec3 cornerPos = m_min + CHILD_MIN_OFFSETS[i];
		float density = Density_Func(glm::vec3(cornerPos), csgOperation);
		density = glm::max(-density, m_densities[i]);
		const int material = density < 0.f ? MATERIAL_SOLID : MATERIAL_AIR;
		corners |= (material << i);

		m_densities[i] = density;
	}


	if (corners == 0 || corners == 255)
	{
		// voxel is full inside or outside the volume
		//m_type = Node_Leaf;
		m_flag &= ~(OCTREE_ACTIVE);
		if (m_drawInfo)
		{
			delete m_drawInfo;
			m_drawInfo = NULL;
		}

		return false;
	}

	//build leaf node if node contains isosurface
	const int MAX_CROSSINGS = 6;
	int edgeCount = 0;
	glm::vec3 averageNormal(0.f);
	svd::QefSolver qef;

	for (int i = 0; i < 12 && edgeCount < MAX_CROSSINGS; i++)
	{
		const int c1 = edgevmap[i][0];
		const int c2 = edgevmap[i][1];

		const int m1 = (corners >> c1) & 1;
		const int m2 = (corners >> c2) & 1;

		if ((m1 == MATERIAL_AIR && m2 == MATERIAL_AIR) ||
			(m1 == MATERIAL_SOLID && m2 == MATERIAL_SOLID))
		{
			continue; //no change in sign density
		}

		const glm::vec3 p1 = glm::vec3(m_min + CHILD_MIN_OFFSETS[c1]);
		const glm::vec3 p2 = glm::vec3(m_min + CHILD_MIN_OFFSETS[c2]);

		const glm::vec3 p = ApproximateZeroCrossingPosition(p1, p2, csgOperation);
		const glm::vec3 n = CalculateSurfaceNormal(p, csgOperation);

		qef.add(p.x, p.y, p.z, n.x, n.y, n.z);

		averageNormal += n;

		edgeCount++;
	}

	svd::Vec3 qefPosition;
	qef.solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);

	OctreeDrawInfo* drawInfo = new OctreeDrawInfo;

	drawInfo->position = glm::vec3(qefPosition.x, qefPosition.y, qefPosition.z);
	drawInfo->qef = qef.getData();

	const glm::vec3 min = glm::vec3(m_min);
	const glm::vec3 max = glm::vec3(m_min + glm::ivec3(m_size));

	if (drawInfo->position.x < min.x || drawInfo->position.x > max.x ||
		drawInfo->position.y < min.y || drawInfo->position.y > max.y ||
		drawInfo->position.z < min.z || drawInfo->position.z > max.z)
	{
		const auto& mp = qef.getMassPoint();
		drawInfo->position = glm::vec3(mp.x, mp.y, mp.z);
	}

	drawInfo->averageNormal = glm::normalize(averageNormal / (float)edgeCount);
	drawInfo->corners = corners;

	drawInfo->type = AddTerrainType(drawInfo);

	m_type = Node_Leaf;
	m_drawInfo = drawInfo;

	m_flag |= OCTREE_ACTIVE;
	m_flag |= OCTREE_LEAF;

	return true;
}

bool VoxelOctree::BuildLeafNode(VoxelOctree * diffNode, const glm::vec3 csgOperation, glm::vec3 face)
{
	int corners = 0;

	for (int i = 0; i < 8; i++)
	{
		glm::ivec3 cornerPos = m_min + CHILD_MIN_OFFSETS[i];
		bool intersect = false;
		
		for (int j = 0; j < 8; j++)
		{
			glm::ivec3 neighborCorner = diffNode->m_min + CHILD_MIN_OFFSETS[j];

			if(neighborCorner == cornerPos)
			{
				intersect = true;
			}			

		}

		float density;
		int material;

		if (intersect)
		{
			density = Density_Func(glm::vec3(cornerPos), csgOperation);			
		}
		else
		{
			density = Density_Func(glm::vec3(cornerPos));	
		}

		material = density < 0.f ? MATERIAL_SOLID : MATERIAL_AIR;
		corners |= (material << i);
	}

	if (corners == 0 || corners == 255)
	{
		// voxel is full inside or outside the volume
		m_type = Node_Leaf;
		m_flag &= ~(OCTREE_ACTIVE);

		return false;
	}

	//build leaf node if node contains isosurface
	const int MAX_CROSSINGS = 6;
	int edgeCount = 0;
	glm::vec3 averageNormal(0.f);
	svd::QefSolver qef;

	for (int i = 0; i < 12 && edgeCount < MAX_CROSSINGS; i++)
	{
		const int c1 = edgevmap[i][0];
		const int c2 = edgevmap[i][1];

		const int m1 = (corners >> c1) & 1;
		const int m2 = (corners >> c2) & 1;

		if ((m1 == MATERIAL_AIR && m2 == MATERIAL_AIR) ||
			(m1 == MATERIAL_SOLID && m2 == MATERIAL_SOLID))
		{
			continue; //no change in sign density
		}

		const glm::vec3 p1 = glm::vec3(m_min + CHILD_MIN_OFFSETS[c1]);
		const glm::vec3 p2 = glm::vec3(m_min + CHILD_MIN_OFFSETS[c2]);


		const glm::vec3 p = ApproximateZeroCrossingPosition(p1, p2);
		const glm::vec3 n = CalculateSurfaceNormal(p);

		qef.add(p.x, p.y, p.z, n.x, n.y, n.z);

		averageNormal += n;

		edgeCount++;
	}

	svd::Vec3 qefPosition;
	qef.solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);

	OctreeDrawInfo* drawInfo = new OctreeDrawInfo;

	drawInfo->position = glm::vec3(qefPosition.x, qefPosition.y, qefPosition.z);
	drawInfo->qef = qef.getData();

	const glm::vec3 min = glm::vec3(m_min);
	const glm::vec3 max = glm::vec3(m_min + glm::ivec3(m_size));

	if (drawInfo->position.x < min.x || drawInfo->position.x > max.x ||
		drawInfo->position.y < min.y || drawInfo->position.y > max.y ||
		drawInfo->position.z < min.z || drawInfo->position.z > max.z)
	{
		const auto& mp = qef.getMassPoint();
		drawInfo->position = glm::vec3(mp.x, mp.y, mp.z);
	}

	drawInfo->averageNormal = glm::normalize(averageNormal / (float)edgeCount);
	drawInfo->corners = corners;

	drawInfo->type = AddTerrainType(drawInfo);


	m_type = Node_Leaf;
	m_drawInfo = drawInfo;

	m_flag |= OCTREE_ACTIVE;
	m_flag |= OCTREE_LEAF;

	return true;
}

bool VoxelOctree::BuildTree()
{
	bool active = false;

	int i = 0;

	//base case
	//region matches chunk size
	if (m_size == 1)
	{
		return BuildLeafNode();
	}

	//activate 8 children
	InitChildren();

	//recursively descend tree to find the active nodes
	for (int i = 0; i < 8; i++)
	{
		m_type = Node_Internal;

		//skip nodes above the max world height
		if (m_children[i]->m_min.y <= (VoxelOctree::maxHeight)
			&& m_children[i]->BuildTree())
		{
			m_childMask |= 1 << i;
		}
		else
		{
			m_childMask &= ~(1 << i);
			m_children[i]->DestroyNode();
			m_children[i] = NULL;
		}

	}
	if (m_childMask)
	{
		m_flag |= OCTREE_ACTIVE;//if at least one child is active, then active
		
		if (m_size == VoxelChunk::CHUNK_SIZE)
		{
			m_chunk = voxelManager->CreateChunk(m_min, this);
			render_list.push_back(m_chunk);
		}
	}

	return m_childMask;
}


void VoxelOctree::AssignNeighbors()
{
	for (int i = 0; i < render_list.size(); i++)
	{
		glm::vec3 position = render_list[i]->m_node->m_min;

		//find chunk above and below
		render_list[i]->AssignNeighbor(FindChunk(position + glm::vec3(0, VoxelChunk::CHUNK_SIZE, 0)), (int)TOP);
		render_list[i]->AssignNeighbor(FindChunk(position + glm::vec3(0, -VoxelChunk::CHUNK_SIZE, 0)), (int)BOTTOM);

		//assign left and right chunks
		render_list[i]->AssignNeighbor(FindChunk(position + glm::vec3(-VoxelChunk::CHUNK_SIZE, 0, 0)), (int)LEFT);
		render_list[i]->AssignNeighbor(FindChunk(position + glm::vec3(VoxelChunk::CHUNK_SIZE, 0, 0)), (int)RIGHT);

		//back and front
		render_list[i]->AssignNeighbor(FindChunk(position + glm::vec3(0, 0, -VoxelChunk::CHUNK_SIZE)), (int)BACK);
		render_list[i]->AssignNeighbor(FindChunk(position + glm::vec3(0, 0, VoxelChunk::CHUNK_SIZE)), (int)FRONT);

		//XZ diagonal
		render_list[i]->AssignNeighbor(FindChunk(position + glm::vec3(VoxelChunk::CHUNK_SIZE, 0, VoxelChunk::CHUNK_SIZE)), (int)FRONT_RIGHT);
	}
}

VoxelOctree * VoxelOctree::FindChunkNode(glm::vec3 pos)
{
	//check if position is within world bounds
	if (pos.x < 0 || pos.y < 0 || pos.z < 0 ||
		pos.x >= voxelManager->GetWorldSize() ||
		pos.y >= voxelManager->GetWorldSize() ||
		pos.z >= voxelManager->GetWorldSize())
	{
		return NULL;
	}

	//iteratively find chunk that contains start position
	VoxelOctree *node = voxelManager->getRootNode();

	//traverse tree, find chunk
	bool found = false;

	while (!found)
	{
		int index = 0;

		//check which child the next node is in 
		int child_size = node->m_size >> 1;

		//octal search
		int x = pos.x >= node->m_min.x && pos.x < node->m_min.x + child_size ? 0 : 1;
		int y = pos.y >= node->m_min.y && pos.y < node->m_min.y + child_size ? 0 : 1;
		int z = pos.z >= node->m_min.z && pos.z < node->m_min.z + child_size ? 0 : 1;

		//calculate index 
		index = 4 * x + 2 * y + z;

		node = node->m_children[index];

		if (!node || ~node->m_flag & OCTREE_ACTIVE || ~node->m_flag & OCTREE_INUSE)
			return NULL;

		if (node->m_size == VoxelChunk::CHUNK_SIZE)
			found = true;
		else if (node->m_size < VoxelChunk::CHUNK_SIZE)
			return NULL;
	}

	return node;
}

VoxelChunk * VoxelOctree::FindChunk(glm::vec3 pos)
{
	VoxelOctree * node = FindChunkNode(pos);

	if (node)
	{
		return node->m_chunk;
	}

	return NULL;
}

VoxelOctree * VoxelOctree::FindLeafNode(glm::vec3 pos)
{
	//check if position is within world bounds
	if (pos.x < 0 || pos.y < 0 || pos.z < 0 ||
		pos.x >= voxelManager->GetWorldSize() ||
		pos.y >= voxelManager->GetWorldSize() ||
		pos.z >= voxelManager->GetWorldSize())
	{
		return NULL;
	}

	//iteratively find chunk that contains start position
	VoxelOctree *node = this;

	//traverse tree, find chunk
	bool found = false;

	while (!found)
	{
		int index = 0;

		//check which child the next node is in 
		int child_size = node->m_size >> 1;

		//octal search
		int x = pos.x >= node->m_min.x && pos.x < node->m_min.x + child_size ? 0 : 1;
		int y = pos.y >= node->m_min.y && pos.y < node->m_min.y + child_size ? 0 : 1;
		int z = pos.z >= node->m_min.z && pos.z < node->m_min.z + child_size ? 0 : 1;

		//calculate index 
		index = 4 * x + 2 * y + z;

		node = node->m_children[index];

		if (!node || ~node->m_flag & OCTREE_ACTIVE || ~node->m_flag & OCTREE_INUSE)
			return NULL;

		if (node->m_type  == Node_Leaf)
			found = true;
	}

	return node;
}

VoxelOctree * VoxelOctree::FindNode(glm::vec3 pos, int size)
{
	//check if position is within world bounds
	if (pos.x < 0 || pos.y < 0 || pos.z < 0 ||
		pos.x >= voxelManager->GetWorldSize() ||
		pos.y >= voxelManager->GetWorldSize() ||
		pos.z >= voxelManager->GetWorldSize())
	{
		return NULL;
	}

	//iteratively find chunk that contains start position
	VoxelOctree *node = this;

	//traverse tree, find chunk
	bool found = false;

	while (!found)
	{
		int index = 0;

		//check which child the next node is in 
		int child_size = node->m_size >> 1;

		//octal search
		int x = pos.x >= node->m_min.x && pos.x < node->m_min.x + child_size ? 0 : 1;
		int y = pos.y >= node->m_min.y && pos.y < node->m_min.y + child_size ? 0 : 1;
		int z = pos.z >= node->m_min.z && pos.z < node->m_min.z + child_size ? 0 : 1;

		//calculate index 
		index = 4 * x + 2 * y + z;

		node = node->m_children[index];

		if (!node || ~node->m_flag & OCTREE_ACTIVE || ~node->m_flag & OCTREE_INUSE)
			return NULL;

		if (node->m_size == size)
			found = true;
	}

	return node;
}

//TODO move drawing functions out of Octree
void VoxelOctree::DrawGrass()
{
	glBindVertexArray(g_grassVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g_terrainGrassPos.size());
	glBindVertexArray(0);
}

void VoxelOctree::Draw()
{
	for (int i = 0; i < render_list.size(); i++)
	{
		if(render_list[i] && render_list[i]->m_flag & CHUNK_FLAG_INUSE)
		{ 
			render_list[i]->Render();
		}		
	}
}

void VoxelOctree::Draw(GLint shader)
{

	for (int i = 0; i < render_list.size(); i++)
	{
		if (render_list[i] && render_list[i]->m_flag & CHUNK_FLAG_INUSE)
		{
			GLint location = glGetUniformLocation(shader, "chunkID");
			glUniform1i(glGetUniformLocation(shader, "chunkID"), (GLint) i);
			render_list[i]->Render();
		}
	}
}


/******************************************dual contouring ***********************************************/
glm::vec3 VoxelOctree::ApproximateZeroCrossingPosition(const glm::vec3& p0, const glm::vec3& p1)
{
	// approximate the zero crossing by finding the min value along the edge
	float minValue = 100000.f;
	float t = 0.f;
	float currentT = 0.f;
	const int steps = 12;
	const float increment = 1.f / (float)steps;
	while (currentT <= 1.f)
	{
		const glm::vec3 p = p0 + ((p1 - p0) * currentT);
		const float density = glm::abs(Density_Func(p));
		if (density < minValue)
		{
			minValue = density;
			t = currentT;
		}

		currentT += increment;
	}

	return p0 + ((p1 - p0) * t);
}

glm::vec3 VoxelOctree::ApproximateZeroCrossingPosition(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3 &csgOperationPos)
{
	// approximate the zero crossing by finding the min value along the edge
	float minValue = 100000.f;
	float t = 0.f;
	float currentT = 0.f;
	const int steps = 8;
	const float increment = 1.f / (float)steps;
	while (currentT <= 1.f)
	{
		const glm::vec3 p = p0 + ((p1 - p0) * currentT);
		const float density = glm::abs(Density_Func(p, csgOperationPos));
		if (density < minValue)
		{
			minValue = density;
			t = currentT;
		}

		currentT += increment;
	}

	return p0 + ((p1 - p0) * t);
}

glm::vec3 VoxelOctree::CalculateSurfaceNormal(const glm::vec3 &pos)
{
	const float H = 0.05f;

	if (pos.y <= 1.0f)
	{
		return glm::vec3(0.0f, 1.0f, 0.0f);
	}

	//finite difference method to get partial derivatives

	const float dx = Density_Func(pos + glm::vec3(H, 0.f, 0.f)) - Density_Func(pos - glm::vec3(H, 0.f, 0.f));
	const float dy = Density_Func(pos + glm::vec3(0.f, H, 0.f)) - Density_Func(pos - glm::vec3(0.f, H, 0.f));
	const float dz = Density_Func(pos + glm::vec3(0.f, 0.f, H)) - Density_Func(pos - glm::vec3(0.f, 0.f, H));

	return glm::normalize(glm::vec3(dx, dy, dz));
}

glm::vec3 VoxelOctree::CalculateSurfaceNormal(const glm::vec3 &pos, const std::vector<glm::vec3> &csgOperationPos)
{
	const float H = 0.05f;

	if (pos.y <= 1.0f)
	{
		return glm::vec3(0.0f, 1.0f, 0.0f);
	}

	//finite difference method to get partial derivatives

	const float dx = Density_Func(pos + glm::vec3(H, 0.f, 0.f), csgOperationPos) - Density_Func(pos - glm::vec3(H, 0.f, 0.f), csgOperationPos);
	const float dy = Density_Func(pos + glm::vec3(0.f, H, 0.f), csgOperationPos) - Density_Func(pos - glm::vec3(0.f, H, 0.f), csgOperationPos);
	const float dz = Density_Func(pos + glm::vec3(0.f, 0.f, H), csgOperationPos) - Density_Func(pos - glm::vec3(0.f, 0.f, H), csgOperationPos);

	return glm::normalize(glm::vec3(dx, dy, dz));
}
glm::vec3 VoxelOctree::CalculateSurfaceNormal(const glm::vec3 &pos, const glm::vec3 &csgOperationPos)
{
	const float H = 0.05f;

	if (pos.y <= 1.0f)
	{
		return glm::vec3(0.0f, 1.0f, 0.0f);
	}

	//finite difference method to get partial derivatives

	const float dx = Density_Func(pos + glm::vec3(H, 0.f, 0.f), csgOperationPos) - Density_Func(pos - glm::vec3(H, 0.f, 0.f), csgOperationPos);
	const float dy = Density_Func(pos + glm::vec3(0.f, H, 0.f), csgOperationPos) - Density_Func(pos - glm::vec3(0.f, H, 0.f), csgOperationPos);
	const float dz = Density_Func(pos + glm::vec3(0.f, 0.f, H), csgOperationPos) - Density_Func(pos - glm::vec3(0.f, 0.f, H), csgOperationPos);

	return glm::normalize(glm::vec3(dx, dy, dz));
}


void VoxelOctree::GenerateMeshFromOctree()
{
	for (int i = 0; i < render_list.size(); i++)
	{
		render_list[i]->m_node->GenerateVertexIndices(render_list[i]->m_vertices);
		render_list[i]->m_node->ContourCellProc(render_list[i]->m_tri_indices);
	}
}

void VoxelOctree::GenerateSeams()
{
	for (int i = 0; i < render_list.size(); i++)
	{
		render_list[i]->GenerateSeam();
	}

}

void VoxelOctree::GenerateVertexIndices(std::vector<VoxelVertex> &voxelVerts)
{
	if (!this)
	{
		return;
	}

	if (~m_flag & OCTREE_ACTIVE)
		return;

	if (m_type != Node_Leaf)
	{
		for (int i = 0; i < 8; i++)
		{
			if (m_childMask & 1 << i)
			{
				m_children[i]->GenerateVertexIndices(voxelVerts);
			}
		}
	}

	if (m_type != Node_Internal)
	{
		if (!m_drawInfo)
		{
			printf("Error! Could not add vertex!\n");
			exit(EXIT_FAILURE);
		}

		m_drawInfo->index = voxelVerts.size();
		voxelVerts.push_back(VoxelVertex(m_drawInfo->position, m_drawInfo->averageNormal, m_drawInfo->type));
	}

}


void VoxelOctree::ContourProcessEdge(std::vector<GLuint> &m_tri_indices, VoxelOctree* node[4], int dir)
{
	int minSize = 1000000;		// arbitrary big number
	int minIndex = 0;
	int indices[4] = { -1, -1, -1, -1 };
	bool flip = false;
	bool signChange[4] = { false, false, false, false };

	for (int i = 0; i < 4; i++)
	{
		const int edge = processEdgeMask[dir][i];
		const int c1 = edgevmap[edge][0];
		const int c2 = edgevmap[edge][1];

		const int m1 = (node[i]->m_drawInfo->corners >> c1) & 1;
		const int m2 = (node[i]->m_drawInfo->corners >> c2) & 1;

		if (node[i]->m_size < minSize)
		{
			minSize = node[i]->m_size;
			minIndex = i;
			flip = m1 != MATERIAL_AIR;
		}


		indices[i] = node[i]->m_drawInfo->index;

		signChange[i] =
			(m1 == MATERIAL_AIR && m2 != MATERIAL_AIR) ||
			(m1 != MATERIAL_AIR && m2 == MATERIAL_AIR);

		if (node[i]->m_drawInfo->averageNormal.y > .95f && node[i]->m_drawInfo->type == DIRT)
		{
			g_terrainGrassPos.push_back(node[i]->m_drawInfo->position + glm::vec3((rand() % 10) / 10.0f, 0, (rand() % 10) / 10.0f));
		}
	}

	if (signChange[minIndex])
	{
		if (!flip)
		{
			m_tri_indices.push_back(indices[0]);
			m_tri_indices.push_back(indices[1]);
			m_tri_indices.push_back(indices[3]);

			m_tri_indices.push_back(indices[0]);
			m_tri_indices.push_back(indices[3]);
			m_tri_indices.push_back(indices[2]);
		}
		else
		{
			m_tri_indices.push_back(indices[0]);
			m_tri_indices.push_back(indices[3]);
			m_tri_indices.push_back(indices[1]);

			m_tri_indices.push_back(indices[0]);
			m_tri_indices.push_back(indices[2]);
			m_tri_indices.push_back(indices[3]);
		}
	}
}


void VoxelOctree::ContourEdgeProc(std::vector<GLuint> &m_tri_indices, VoxelOctree* node[4], int dir)
{
	if (!node[0] || !node[1] || !node[2] || !node[3])
	{
		return;
	}
	if ((~node[0]->m_flag & OCTREE_ACTIVE) || (~node[1]->m_flag & OCTREE_ACTIVE)
		|| (~node[2]->m_flag & OCTREE_ACTIVE) || (~node[3]->m_flag & OCTREE_ACTIVE))
	{
		return;
	}


	if (node[0]->m_type != Node_Internal &&
		node[1]->m_type != Node_Internal &&
		node[2]->m_type != Node_Internal &&
		node[3]->m_type != Node_Internal)
	{
		ContourProcessEdge(m_tri_indices, node, dir);
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			VoxelOctree* edgeNodes[4];
			const int c[4] =
			{
				edgeProcEdgeMask[dir][i][0],
				edgeProcEdgeMask[dir][i][1],
				edgeProcEdgeMask[dir][i][2],
				edgeProcEdgeMask[dir][i][3],
			};

			for (int j = 0; j < 4; j++)
			{
				if (node[j]->m_type == Node_Leaf || node[j]->m_type == Node_Psuedo)
				{
					edgeNodes[j] = node[j];
				}
				else
				{
					edgeNodes[j] = node[j]->m_children[c[j]];
				}
			}

			ContourEdgeProc(m_tri_indices, edgeNodes, edgeProcEdgeMask[dir][i][4]);
		}
	}
}


void VoxelOctree::ContourFaceProc(std::vector<GLuint> &m_tri_indices, VoxelOctree* node[2], int dir)
{
	if (!node[0] || !node[1])
	{
		return;
	}
	if ((~node[0]->m_flag & OCTREE_ACTIVE) || (~node[1]->m_flag & OCTREE_ACTIVE))
	{
		return;
	}

	if (node[0]->m_type == Node_Internal ||
		node[1]->m_type == Node_Internal)
	{
		for (int i = 0; i < 4; i++)
		{
			VoxelOctree* faceNodes[2];
			const int c[2] =
			{
				faceProcFaceMask[dir][i][0],
				faceProcFaceMask[dir][i][1],
			};

			for (int j = 0; j < 2; j++)
			{
				if (node[j]->m_type != Node_Internal)
				{
					faceNodes[j] = node[j];
				}
				else
				{
					faceNodes[j] = node[j]->m_children[c[j]];
				}
			}

			ContourFaceProc(m_tri_indices, faceNodes, faceProcFaceMask[dir][i][2]);
		}

		const int orders[2][4] =
		{
			{ 0, 0, 1, 1 },
			{ 0, 1, 0, 1 },
		};
		for (int i = 0; i < 4; i++)
		{
			VoxelOctree* edgeNodes[4];
			const int c[4] =
			{
				faceProcEdgeMask[dir][i][1],
				faceProcEdgeMask[dir][i][2],
				faceProcEdgeMask[dir][i][3],
				faceProcEdgeMask[dir][i][4],
			};

			const int* order = orders[faceProcEdgeMask[dir][i][0]];
			for (int j = 0; j < 4; j++)
			{
				if (node[order[j]]->m_type == Node_Leaf ||
					node[order[j]]->m_type == Node_Psuedo)
				{
					edgeNodes[j] = node[order[j]];
				}
				else
				{
					edgeNodes[j] = node[order[j]]->m_children[c[j]];
				}
			}

			ContourEdgeProc(m_tri_indices, edgeNodes, faceProcEdgeMask[dir][i][5]);
		}
	}
}


void VoxelOctree::ContourCellProc(std::vector<GLuint> &m_tri_indices)
{
	if (!this || ~m_flag & OCTREE_ACTIVE)
	{
		return;
	}
	if (m_type == Node_Internal)
	{
		for (int i = 0; i < 8; i++)
		{
			if (m_children[i] && m_children[i]->m_flag & OCTREE_ACTIVE)
			{
				m_children[i]->ContourCellProc(m_tri_indices);
			}
		}

		for (int i = 0; i < 12; i++)
		{
			VoxelOctree* faceNodes[2];
			const int c[2] = { cellProcFaceMask[i][0], cellProcFaceMask[i][1] };


			faceNodes[0] = m_children[c[0]];
			faceNodes[1] = m_children[c[1]];

			ContourFaceProc(m_tri_indices, faceNodes, cellProcFaceMask[i][2]);
		}

		for (int i = 0; i < 6; i++)
		{
			VoxelOctree* edgeNodes[4];
			const int c[4] =
			{
				cellProcEdgeMask[i][0],
				cellProcEdgeMask[i][1],
				cellProcEdgeMask[i][2],
				cellProcEdgeMask[i][3],
			};

			for (int j = 0; j < 4; j++)
			{
				edgeNodes[j] = m_children[c[j]];
			}

			ContourEdgeProc(m_tri_indices, edgeNodes, cellProcEdgeMask[i][4]);
		}
	}
}

VoxelOctree * VoxelOctree::SimplifyOctree(float threshold)
{
	if (!this)
	{
		return NULL;
	}

	if (m_type != Node_Internal)
	{
		// can't simplify!
		return this;
	}

	svd::QefSolver qef;
	int signs[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	int midsign = -1;
	int edgeCount = 0;
	bool isCollapsible = true;

	for (int i = 0; i < 8; i++)
	{
		m_children[i] = m_children[i]->SimplifyOctree(threshold);

		if (m_children[i])
		{
			VoxelOctree* child = m_children[i];

			if (child->m_type == Node_Internal)
			{
				isCollapsible = false;
			}
			else
			{
				qef.add(child->m_drawInfo->qef);

				midsign = (child->m_drawInfo->corners >> (7 - i)) & 1;
				signs[i] = (child->m_drawInfo->corners >> i) & 1;

				edgeCount++;
			}
		}
	}

	if (!isCollapsible)
	{
		// at least one child is an internal node, can't collapse
		return this;
	}

	svd::Vec3 qefPosition;
	qef.solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);
	float error = qef.getError();

	// convert to glm vec3 for ease of use
	glm::vec3 position(qefPosition.x, qefPosition.y, qefPosition.z);

	// at this point the masspoint will actually be a sum, so divide to make it the average
	if (error > threshold)
	{
		// this collapse breaches the threshold
		return this;
	}

	if (position.x < m_min.x || position.x >(m_min.x + m_size) ||
		position.y < m_min.y || position.y >(m_min.y + m_size) ||
		position.z < m_min.z || position.z >(m_min.z + m_size))
	{
		const auto& mp = qef.getMassPoint();
		position = glm::vec3(mp.x, mp.y, mp.z);
	}

	// change the node from an internal node to a 'psuedo leaf' node
	OctreeDrawInfo* drawInfo = new OctreeDrawInfo;

	for (int i = 0; i < 8; i++)
	{
		if (signs[i] == -1)
		{
			// Undetermined, use centre sign instead
			drawInfo->corners |= (midsign << i);
		}
		else
		{
			drawInfo->corners |= (signs[i] << i);
		}
	}

	drawInfo->averageNormal = glm::vec3(0.f);

	for (int i = 0; i < 8; i++)
	{
		if (m_children[i])
		{

			if (m_children[i]->m_type == Node_Psuedo ||
				m_children[i]->m_type == Node_Leaf)
			{
				drawInfo->averageNormal += m_children[i]->m_drawInfo->averageNormal;
			}

		}
	}

	drawInfo->averageNormal = glm::normalize(drawInfo->averageNormal);
	drawInfo->position = position;
	drawInfo->qef = qef.getData();

	for (int i = 0; i < 8; i++)
	{
		m_children[i]->DestroyNode();
		m_children[i] = nullptr;
	}

	m_type = Node_Psuedo;
	m_drawInfo = drawInfo;
}


void VoxelOctree::UploadMesh()
{
	GLuint vertices = 0;
	GLuint indices = 0;
	for (int i = 0; i < render_list.size(); i++)
	{
		vertices += render_list[i]->m_vertices.size();
		indices += render_list[i]->m_tri_indices.size();

		render_list[i]->BindMesh();
	}

	slog("Vertices: %u, Triangles %u", vertices, indices / 3);
}

void VoxelOctree::UploadGrass()
{

	const GLfloat g_grassVerts[] = {
		// Positions				// uv
		-0.50f,  0.50f,  0.0f,	1.0f, 0.0f,
		0.50f, -0.50f,  0.0f,	0.0f, 1.0f,
		-0.50f, -0.50f,  0.0f,	1.0f, 1.0f,

		0.50f, -0.50f,  0.0f,	0.0f, 1.0f,
		-0.50f,  0.50f, 0.0f,	1.0f, 0.0f,
		0.50f,  0.50f, 0.0f,	0.0f, 0.0f
	};

	if (g_terrainGrassPos.empty())
	{
		return;
	}

	glGenBuffers(1, &g_grassPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_grassPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * g_terrainGrassPos.size(), &g_terrainGrassPos[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &g_grassVAO);
	glBindVertexArray(g_grassVAO);

	glGenBuffers(1, &g_grassQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_grassQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_grassVerts), &g_grassVerts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(3 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, g_grassPosVBO);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glVertexAttribDivisor(2, 1);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
