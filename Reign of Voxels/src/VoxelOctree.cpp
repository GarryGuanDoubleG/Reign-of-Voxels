#include <thread>
#include <mutex>
#include <math.h>

#include "VoxelOctree.hpp"
#include "VoxelManager.hpp"
#include "glm.hpp"
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
}


void VoxelOctree::GenerateMesh(Model *cube, int portion, int portion_len)
{
	sf::Clock timer;

	for (int j = portion * portion_len; j < portion * portion_len + portion_len; j++)
	{
		if(j < render_list.size())
			render_list[j]->GenerateMesh(cube);
	}
	//std::cout << "Time for i: " << portion << " is " << timer.getElapsedTime().asSeconds() << std::endl;
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

void VoxelOctree::InitializeOctree(sf::Image *heightmap, int worldSize, VoxelManager *manager)
{
	voxelManager = manager;

	int thread_count = 32;
	std::vector<std::thread> threads;

	g_heightMap = heightmap;
	g_heightarray = g_heightMap->getPixelsPtr();

	sf::Clock build_time; //profiling


	//initialize 8 children children
	//InitializeChildren();

	////start a thread for each child to build its respective region
	//for (int i = 0; i < 8; i++)
	//{
	//	threads.push_back(std::thread(&VoxelOctree::BuildTree, voxelManager->getOctreeChild(this, i)));
	//}

	this->BuildTree();
	//sync threads
	//for (auto& t : threads)
	//{
	//	t.join();
	//}

	//threads.clear();

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Childnode calls is " << vox_count << std::endl;
	std::cout << "leaf nodes " << leaf_count << std::endl;

	build_time.restart();

	//load cube model
	Model * cube = new Model("Resources\\models\\cube.obj");

	int j = 0;
	int length = render_list.size() / thread_count;

	//start threads to generate vertices
	while(j * length < render_list.size())
	{
		threads.push_back(std::thread(&VoxelOctree::GenerateMesh, this, cube, j, length));
		++j;
	}

	//sync mesh gen threads
	for (auto& t : threads)
		t.join();
	
	std::cout << "Generate Mesh time is " << build_time.getElapsedTime().asSeconds() << std::endl;	
	build_time.restart();
	
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

bool VoxelOctree::BuildTree()
{
	int size = m_region.size;
	bool active = false;

	int i = 0;
	vox_count++;
	
	if (m_region.size <= VoxelChunk::CHUNK_SIZE)
	{
		if (!m_chunk)
		{
			m_chunk = voxelManager->CreateChunk(m_region.position);
		}

		float scale = (float)VoxelOctree::maxHeight / (float)256;

		for (int x = m_region.position.x; x < m_region.position.x + size; x++)
		{
			for (int z = m_region.position.z; z < m_region.position.z + size; z++)
			{
				float height = g_heightMap->getPixel(x, z).r;
				
				height *= scale;

				if (height >= m_region.position.y)
				{
					active = true;
				}
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
		{
			voxelManager->destroyChunk(m_chunk);
		}

		return active;
	}

	//activate 8 children
	InitializeChildren();

	for (int i = 0; i < 8; i++)
	{
		VoxelOctree *child = voxelManager->getOctreeChild(this, i);
		
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

//void VoxelOctree::AssignNeighbors()
//{
//
//}