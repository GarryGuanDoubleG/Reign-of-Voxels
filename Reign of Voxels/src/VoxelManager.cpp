#include <mutex>

#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

std::mutex g_free_chunk_guard;

VoxelManager::VoxelManager()
{
	m_worldSize = 512;
	
	int worldSizeXZ = m_worldSize * m_worldSize;

	m_cube = new Model("Resources\\models\\cube.obj");
	//intialize chunkpool
	int maxChunks = worldSizeXZ * (VoxelOctree::maxHeight * 2) / VoxelChunk::CHUNK_SIZE_CUBED;
	//int maxChunks = worldSizeXZ * m_worldSize / VoxelChunk::CHUNK_SIZE_CUBED;

	m_maxChunks = maxChunks;
	
	m_chunkPool = new VoxelChunk[maxChunks];
	m_freeChunkHead = &m_chunkPool[0];
	
	sizeof(VoxelChunk);

	//set next available nodes
	for (int i = 0; i < maxChunks - 1; i++)
		m_chunkPool[i].m_next = &m_chunkPool[i + 1];

	//Allocate space for Octree. 2^(3h + 1) - 1 nodes
	int maxOctree =  (int)log2(m_worldSize) - (int)log2(VoxelChunk::CHUNK_SIZE);
		maxOctree = (2 << (3 * maxOctree)) - 1; //2^3 times for 8 child nodes
	m_maxOctNodes = maxOctree;

	m_octreePool = new VoxelOctree[maxOctree];
	m_octreeRoot = &m_octreePool[0];

	for (int i = 0; i < maxOctree; i++)
		m_octreePool[i].index = i;
}

VoxelManager::~VoxelManager()
{

}

void VoxelManager::GenerateVoxels()
{
	sf::Image *heightmap = new sf::Image();

	if (!heightmap->loadFromFile(GenerateTerrainMap(m_worldSize)))
		slog("Failed to Load or Generate HeightMap");
	else
	{
		CubeRegion region = { Vec3(0.0), m_worldSize };

		m_octreeRoot->InitNode(region);//root octree
		m_octreeRoot->InitializeOctree(heightmap, m_worldSize, this);
	}

	delete heightmap;
}


VoxelChunk * VoxelManager::createChunk(Vec3 worldPosition)
{
	if (m_freeChunkHead == m_freeChunkHead->m_next)
		std::cout << "free list is empty" << std::endl;

	VoxelChunk * chunk = m_freeChunkHead;
	m_freeChunkHead = chunk->m_next;

	chunk->Init(worldPosition);
	chunk->m_next = NULL;

	return chunk;
}


void VoxelManager::destroyChunk(VoxelChunk * chunk)
{
	chunk->Destroy();


	chunk->m_next = m_freeChunkHead;
	m_freeChunkHead = chunk; //prepend chunk to head of free list
}


VoxelOctree *VoxelManager::getOctreeChild(VoxelOctree * currentNode, int child_index)
{
	//root node has children at index 1-8
	if (currentNode == m_octreeRoot)
		return &m_octreePool[child_index + 1];

	//bound child_index
	if (child_index > 7 || child_index < 0)
		slog("Error child index is from 0-7");

	//get index of current node
	int index = currentNode - m_octreePool;

	if (8 * index + child_index + 1 >= m_maxOctNodes)
		std::cout << "Too many octree nodes" << std::endl;

	//in-array octree, return child at given index
	return &m_octreePool[8 * index + child_index + 1];
}

VoxelOctree *VoxelManager::createOctreeChild(VoxelOctree *currentNode, int child_index, CubeRegion &region)
{
	VoxelOctree *child = getOctreeChild(currentNode, child_index);

	child->InitNode(region);

	return child;
}

void VoxelManager::destroyOctreeNode(VoxelOctree * node)
{
	node->DestroyNode(); //clears minimal data for reuse
}

void VoxelManager::RenderVoxels(Camera * player_cam)
{
	GLuint voxel_shader = GetShader("voxel");
	GLuint model_loc, view_loc, proj_loc, 
		light_loc, obj_loc, light_pos_loc, view_pos_loc;

	glUseProgram(voxel_shader);

	model_loc = glGetUniformLocation(voxel_shader, "model");
	view_loc = glGetUniformLocation(voxel_shader, "view");
	proj_loc = glGetUniformLocation(voxel_shader, "projection");
	light_loc = glGetUniformLocation(voxel_shader, "lightColor");
	obj_loc = glGetUniformLocation(voxel_shader, "objectColor");
	light_pos_loc = glGetUniformLocation(voxel_shader, "lightPos");
	view_pos_loc = glGetUniformLocation(voxel_shader, "viewPos");
	//
	Vec3 light_pos = Vec3(-32, 512, -32);
	Vec3 light_color = Vec3(1.0f, 1.0f, 1.0f);
	Vec3 voxel_color = Vec3(.2f, .6f, .2f);
	Mat4 view = player_cam->GetViewMat();
	Mat4 proj = player_cam->GetProj();
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &player_cam->GetProj()[0][0]);

	Vec3 obj_color(0.6f, 1.0f, 0.3f);
	glUniform3fv(light_pos_loc, 1, &light_pos[0]);
	glUniform3fv(light_loc, 1, &light_color[0]);
	glUniform3fv(obj_loc, 1, &obj_color[0]);
	glUniform3fv(view_pos_loc, 1, &player_cam->getPosition()[0]);


	for (int i = 0; i < VoxelOctree::render_list.size(); i++)
	{
		Mat4 model = Mat4(1.0f);		
		model = glm::translate(model, VoxelOctree::render_list[i]->getPosition());
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

		glUniform1i(glGetUniformLocation(voxel_shader, "chunkID"), i);


		VoxelOctree::render_list[i]->Render();
	}
	
}
