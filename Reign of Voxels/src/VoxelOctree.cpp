#include <thread>
#include <mutex>
#include <math.h>

#include "VoxelOctree.hpp"
#include "glm.hpp"
#include "simple_logger.h"

static sf::Image *g_heightMap;
static const sf::Uint8 *g_heightarray;//array of pixel values

static unsigned int vox_count = 0;
int leaf_count = 0;

unsigned int VoxelOctree::m_chunkCount = 0;

std::vector<VoxelChunk *> VoxelOctree::render_list; //list of leaf nodes
std::mutex g_render_list_mutex;

static float g_max_voxel_height = 16;

VoxelOctree::VoxelOctree(VoxelOctree * parent)
{
	m_parent = parent;
	m_childMask = 0;
	m_chunkCount = 0;
}

VoxelOctree::~VoxelOctree()
{
	if (m_chunk)
		delete m_chunk;
}

void VoxelOctree::GenerateMesh(Model *cube, int portion, int portion_len)
{
	sf::Clock timer;

	for (int j = portion * portion_len; j < portion * portion_len + portion_len; j++)
	{
		if(j < render_list.size())
			render_list[j]->GenerateMesh(cube);
	}
	std::cout << "Time for i: " << portion << " is " << timer.getElapsedTime().asSeconds() << std::endl;
}

void VoxelOctree::InitializeOctree(sf::Image *heightmap, int worldSize)
{
	int thread_count = 32;
	std::vector<std::thread> threads;

	g_heightMap = heightmap;
	g_heightarray = g_heightMap->getPixelsPtr();

	m_region.position = Vec3(0, 0, 0);
	m_region.size = worldSize;
		
	sf::Clock build_time;
	int i = 0;
	for (float x = 0; x < 2; x++)
	{
		for (float y = 0; y < 2; y++)
		{
			for (float z = 0; z < 2; z++)
			{
				int childSize = m_region.size / 2;

				m_childNodes[i] = new VoxelOctree(this);
				m_childNodes[i]->m_region.position = m_region.position + Vec3(x * childSize, y * childSize, z * childSize);
				m_childNodes[i]->m_region.size = childSize;
				threads.push_back(std::thread(&VoxelOctree::BuildNode, this->m_childNodes[i]));
				i++;
			}
		}
	}


	for (auto& t : threads)
	{
		t.join();
	}

	threads.clear();

	std::cout << "Build time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Childnode calls is " << vox_count << std::endl;
	std::cout << "leaf nodes " << leaf_count << std::endl;

	build_time.restart();

	Model * cube = new Model("Resources\\models\\cube.obj");
	//thread_count = log2(render_list.size());

	int j = 0;
	int length = render_list.size() / thread_count;
	while(j * length < render_list.size())
	{
		threads.push_back(std::thread(&VoxelOctree::GenerateMesh, this, cube, j, length));
		++j;
	}

	std::cout << "waiting on threads" << std::endl;
	for (auto& t : threads)
	{
		t.join();
	}
	std::cout << "Generate Mesh time is " << build_time.getElapsedTime().asSeconds() << std::endl;

	build_time.restart();
	unsigned int vertices = 0;
	
	for (int i = 0; i < render_list.size(); i++)
	{
		render_list[i]->BindMesh();
		vertices += render_list[i]->m_vertices.size();
	}

	std::cout << "Bind Mesh time is " << build_time.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Vertices: " << vertices << std::endl;
}

bool VoxelOctree::BuildNode()
{
	int size = m_region.size;
	Vec3 pos = m_region.position;
	bool active = false;

	int i = 0;
	vox_count++;

	if (size <= VoxelChunk::CHUNK_SIZE)
	{
		if (!m_chunk)
			m_chunk = new VoxelChunk(m_region.position);

		for (int x = m_region.position.x; x < m_region.position.x + size; x++)
		{
			for (int z = m_region.position.z; z < m_region.position.z + size; z++)
			{
				float height = g_heightMap->getPixel(x, z).r;
				float scale = g_max_voxel_height / 256;

				height *= scale;

				if (height >= pos.y)
				{
					active = true;
					m_leaf = true;
				}
				m_chunk->InsertVoxelAtPos(x, height, z);
			}
		}


		if (active)
		{
			m_chunk->SetActive(active);
			leaf_count++;
			g_render_list_mutex.lock();
			render_list.push_back(m_chunk);
			g_render_list_mutex.unlock();
		}


		return active;
	}

	for (float x = 0; x < 2; x++)
	{
		for (float y = 0; y < 2; y++)
		{
			for (float z = 0; z < 2; z++)
			{
				int childSize = m_region.size / 2;
				Vec3 child_position = m_region.position + Vec3(x * childSize, y * childSize, z * childSize);

				if (child_position.y <= g_max_voxel_height)
				{
					m_childNodes[i] = new VoxelOctree(this);
					m_childNodes[i]->m_region.position = child_position;
					m_childNodes[i]->m_region.size = childSize;

					if (m_childNodes[i]->BuildNode())
					{
						m_childMask |= 1 << i;
						m_active = true;
					}
					else
					{
						m_childMask &= ~(1 << i);
						delete m_childNodes[i];
					}
				}
				i++;
			}
		}
	}
	return m_childMask;
}

bool inline RegionContains(Vec3 region_pos, int region_size, Vec3 object_pos)
{
	if ((object_pos.x >= region_pos.x && object_pos.x < region_pos.x + region_size)
		&& (object_pos.y >= region_pos.y && object_pos.y < region_pos.y + region_size)
		&& (object_pos.z >= region_pos.z && object_pos.z < region_pos.z + region_size))
	{
		return true;
	}

	return false;
}

void VoxelOctree::UpdateTree()
{

}

void VoxelOctree::Render()
{
	m_chunk->Render();
}