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
		CubeRegion region = { glm::vec3(0.0), m_worldSize };

		m_octreeRoot->InitNode(region);//root octree
		m_octreeRoot->InitializeOctree(heightmap, m_worldSize, this);
	}

	delete heightmap;
}


VoxelChunk * VoxelManager::CreateChunk(glm::vec3 worldPosition)
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
	GLuint model_loc;

	glUseProgram(voxel_shader);

	glm::vec3 light_pos = glm::vec3(256, 512, 256);
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 view = player_cam->GetViewMat();
	glm::mat4 proj = player_cam->GetProj();

	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);

	glm::vec3 voxel_color(0.6f, 1.0f, 0.3f);

	//lighting
	glUniform3fv(glGetUniformLocation(voxel_shader, "viewPos"), 1, &player_cam->GetPosition()[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightPos"), 1, &light_pos[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightColor"), 1, &light_color[0]);

	//voxels
	glUniform3fv(glGetUniformLocation(voxel_shader, "voxelColor"), 1, &voxel_color[0]);

	model_loc = glGetUniformLocation(voxel_shader, "model");

	for (int i = 0; i < VoxelOctree::render_list.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);		
		model = glm::translate(model, VoxelOctree::render_list[i]->getPosition());
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

		glUniform1i(glGetUniformLocation(voxel_shader, "chunkID"), i);

		VoxelOctree::render_list[i]->Render();
	}
	
}

void VoxelManager::RenderMinimap(GLuint shader, glm::vec2 &scale, glm::vec2 &position)
{
	GLuint model_loc = glGetUniformLocation(shader, "model");

	for (int i = 0; i < VoxelOctree::render_list.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::scale(model, glm::vec3(scale.x, 1.0f, scale.y));
		model = glm::translate(model, VoxelOctree::render_list[i]->getPosition());
		model = glm::translate(model, glm::vec3(position.x, 0.0f, position.y));		

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));		

		VoxelOctree::render_list[i]->RenderMinimap();
	}
}
