#include <thread>
#include <mutex>
#include <math.h>

#include "VoxelOctree.hpp"
#include "VoxelManager.hpp"
#include "3dmath.hpp"
#include "simple_logger.h"

static sf::Image *g_heightMap;
static const sf::Uint8 *g_heightarray;//array of pixel values

static unsigned int vox_count = 0;
int leaf_count = 0;

unsigned int VoxelOctree::m_chunkCount = 0;

std::vector<VoxelChunk *> VoxelOctree::render_list; //list of leaf nodes
static std::mutex g_render_list_mutex;

VoxelManager * VoxelOctree::voxelManager;


VoxelOctree::VoxelOctree()
	: m_flag(0),
	m_childMask(0)
{
}


void VoxelOctree::InitNode(CubeRegion region)
{
	if (m_flag & OCTREE_INUSE)
		std::cout << "Already using node ?" << std::endl;
	m_flag |= OCTREE_INUSE;

	m_childMask = 0;
	m_region = region;

	m_chunk = NULL;
}

void VoxelOctree::DestroyNode()
{
	m_flag = 0;
	if(m_chunk)
		m_chunk->Destroy();

}


void VoxelOctree::SortRenderList(glm::vec3 camera_pos)
{
	//update chunk's dist to cam
	int threads = 8;

	for (int i = 0; i < render_list.size(); i++)
		render_list[i]->distToCam = glm::distance(camera_pos, render_list[i]->getPosition());

	//sort by distance
	std::sort(render_list.begin(), render_list.end());
}

void VoxelOctree::InitializeOctree(sf::Image *heightmap, int worldSize, VoxelManager *manager)
{
	voxelManager = manager;

	g_heightMap = heightmap;
	g_heightarray = g_heightMap->getPixelsPtr();

	sf::Clock build_time; //profiling


	this->BuildTree();
	this->AssignNeighbors();

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Childnode calls is " << vox_count << std::endl;
	std::cout << "leaf nodes " << leaf_count << std::endl;

	build_time.restart();
}

void VoxelOctree::InitializeChildren()
{
	int i = 0;

	int childSize = m_region.size / 2;

	for (float x = 0; x < 2; x++)
	{
		for (float y = 0; y < 2; y++)
		{
			for (float z = 0; z < 2; z++)
			{
				CubeRegion childRegion = { m_region.position + glm::vec3(x * childSize, y * childSize, z * childSize),
					childSize };

				voxelManager->createOctreeChild(this, i, childRegion);
				i++;
			}
		}
	}
}

bool VoxelOctree::BuildTree()
{
	int size = m_region.size;
	bool active = false;

	int i = 0;
	
	//base case
	//region matches chunk size
	if (m_region.size <= VoxelChunk::CHUNK_SIZE)
	{
		//initialize chunk
		if (!m_chunk)
			m_chunk = voxelManager->CreateChunk(m_region.position);

		//apply scale factor for max height
		float scale = (float)VoxelOctree::maxHeight / 256.0f;

		//go through respective pixels on heightmap and set it as height
		//activate voxels at or below height
		for (int x = m_region.position.x; x < m_region.position.x + size; x++)
		{
			for (int z = m_region.position.z; z < m_region.position.z + size; z++)
			{
				float height = g_heightMap->getPixel(x, z).r;
				
				height *= scale;

				if (height >= m_region.position.y)
					active = true;

				m_chunk->InsertVoxelAtPos(x, height, z);
			}
		}

		if (active)
		{
			//lock global
			g_render_list_mutex.lock();

			m_chunk->SetActive(active);
			
			m_flag |= OCTREE_ACTIVE;
			m_flag |= OCTREE_LEAF;

			++leaf_count;
			//push chunk
			render_list.push_back(m_chunk);

			//unlock global
			g_render_list_mutex.unlock();
		}
		else
			voxelManager->destroyChunk(m_chunk);

		return active;
	}

	//activate 8 children
	InitializeChildren();

	//recursively descend tree to find the active nodes
	for (int i = 0; i < 8; i++)
	{
		VoxelOctree *child = voxelManager->getOctreeChild(this, i);
		
		//skip nodes above the max world height
		if (child->m_region.position.y <= VoxelOctree::maxHeight 
			 && child->BuildTree())
		{
			m_childMask |= 1 << i;
			m_flag |= OCTREE_ACTIVE;//if at least one child is active, then active
		}
		else
		{
			m_childMask &= ~(1 << i);
			child->m_flag &= ~(OCTREE_ACTIVE);
		}
	}

	return m_childMask;
}

void VoxelOctree::AssignNeighbors()
{
	for (int i = 0; i < render_list.size(); i++)
	{
		glm::vec3 position = render_list[i]->getPosition();
		
		//find chunk above and below
		render_list[i]->AssignNeighbor(FindLeafChunk(position + glm::vec3(0, VoxelChunk::CHUNK_SIZE, 0)), (int)TOP);
		render_list[i]->AssignNeighbor(FindLeafChunk(position + glm::vec3(0, -VoxelChunk::CHUNK_SIZE, 0)), (int)BOTTOM);	

		//assign left and right chunks
		render_list[i]->AssignNeighbor(FindLeafChunk(position + glm::vec3(-VoxelChunk::CHUNK_SIZE, 0, 0)), (int)LEFT);	
		render_list[i]->AssignNeighbor(FindLeafChunk(position + glm::vec3(VoxelChunk::CHUNK_SIZE, 0, 0)), (int)RIGHT);

		//back and front
		render_list[i]->AssignNeighbor(FindLeafChunk(position + glm::vec3(0, 0, -VoxelChunk::CHUNK_SIZE)), (int)BACK);
		render_list[i]->AssignNeighbor(FindLeafChunk(position + glm::vec3(0, 0, VoxelChunk::CHUNK_SIZE)), (int)FRONT);
	}
}

VoxelChunk * VoxelOctree::FindLeafChunk(glm::vec3 pos)
{
	VoxelOctree * node = FindLeafNode(pos);
	return node ? node->m_chunk : NULL;
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
	VoxelOctree *node = voxelManager->getRootNode();

	//traverse tree, find chunk
	bool found = false;

	while (!found)
	{
		int index = 0;

		//check which child the next node is in 
		float child_size = node->m_region.size * .5f;

		//octal search
		int x = pos.x >= node->m_region.position.x && pos.x < node->m_region.position.x + child_size ? 0 : 1;
		int y = pos.y >= node->m_region.position.y && pos.y < node->m_region.position.y + child_size ? 0 : 1;
		int z = pos.z >= node->m_region.position.z && pos.z < node->m_region.position.z + child_size ? 0 : 1;

		//calculate index 
		index = 4 * x + 2 * y + z;

		node = voxelManager->getOctreeChild(node, index);

		if (!node || ~node->m_flag & OCTREE_ACTIVE)
			return NULL;

		if (node->m_flag & OCTREE_LEAF)
			found = true;
	}

	return node;
}


void VoxelOctree::CreatePlayerStart(glm::vec3 pos)
{
	VoxelOctree *node = FindLeafNode(pos);
	
	node->m_chunk->ClearPlayerStart(pos.y);

	//find node above it and deactvate;
	node = FindLeafNode(glm::vec3(pos.x, pos.y + (float)VoxelChunk::CHUNK_SIZE, pos.z));
	
	//center
	glm::vec3 origin = node->m_region.position + glm::vec3(m_region.size / 2.0f);

	//smooth terrain in each direction

	//left
	VoxelOctree *neighbor;
	neighbor = FindLeafNode(glm::vec3(pos.x - VoxelChunk::CHUNK_SIZE, pos.y, pos.z));
	neighbor->SmoothTerrain(4.0f, origin);

	//right
	neighbor = FindLeafNode(glm::vec3(pos.x + VoxelChunk::CHUNK_SIZE, pos.y, pos.z));
	neighbor->SmoothTerrain(4.0f, origin);

	//back
	neighbor = FindLeafNode(glm::vec3(pos.x, pos.y, pos.z - VoxelChunk::CHUNK_SIZE));
	neighbor->SmoothTerrain(4.0f, origin);

	//front
	neighbor = FindLeafNode(glm::vec3(pos.x, pos.y, pos.z + VoxelChunk::CHUNK_SIZE));
	neighbor->SmoothTerrain(4.0f, origin);

	if (node)
		node->DestroyNode();
}

void VoxelOctree::SmoothTerrain(float scale, glm::vec3 origin)
{
	if (m_flag & OCTREE_LEAF)
	{
		m_chunk->SmoothTerrain(scale, origin);
	}
}

void VoxelOctree::GenerateMesh(int portion, int portion_len)
{
	sf::Clock timer;

	for (int j = portion * portion_len; j < portion * portion_len + portion_len; j++)
	{
		if (j < render_list.size())
			render_list[j]->GenerateMesh();
	}
}

void VoxelOctree::GenerateWorldMesh()
{
	int thread_count = 8;
	std::vector<std::thread> threads;

	int j = 0;
	int length = render_list.size() / thread_count;

	sf::Clock build_time;

	//start threads to generate vertices
	while (j * length < render_list.size())
	{
		threads.push_back(std::thread(&VoxelOctree::GenerateMesh, this, j, length));
		++j;
	}

	//sync mesh gen threads
	for (auto& t : threads)
		t.join();

	std::cout << "Generate Mesh time is " << build_time.getElapsedTime().asSeconds() << std::endl;

	//send vertices to gpu
	unsigned int vertices = 0;

	for (int i = 0; i < render_list.size(); i++)
	{
		vertices += render_list[i]->m_vertices.size();
		render_list[i]->BindMesh();
	}

	for (int i = 0; i < render_list.size(); i++)
	{
		render_list[i]->ClearMeshData();
	}

	//get binding time
	std::cout << "Bind Mesh time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Vertices: " << vertices << std::endl;
}