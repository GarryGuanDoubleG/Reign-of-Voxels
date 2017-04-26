#include <mutex>

#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

std::mutex g_free_chunk_guard;

VoxelManager::VoxelManager()
{
	//TODO load voxel world from json
	m_worldSize = 512;
	
	int worldSizeXZ = m_worldSize * m_worldSize;

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

	//TODO reduce nodes so those above max height aren't allocated
	int maxOctree =  (int)log2(m_worldSize);
		maxOctree = (1 << (3 * maxOctree) ) ; //2^3 times for 8 child nodes

	m_maxOctNodes = maxOctree;
	sizeof(VoxelOctree);
	sizeof(OctreeDrawInfo*);
	m_octreePool = new VoxelOctree[maxOctree];
	m_octreeRoot = &m_octreePool[0];
}

VoxelManager::~VoxelManager()
{

}

int VoxelManager::GetWorldSize()
{
	return m_worldSize;
}

void VoxelManager::GenerateVoxels()
{
	sf::Image *heightmap = new sf::Image();

	if (!heightmap->loadFromFile(GenerateTerrainMap(m_worldSize)))
		slog("Failed to Load or Generate HeightMap");
	else
	{
		glm::ivec3 rootMinPos(0.0f);

		m_octreeRoot->InitNode(rootMinPos, m_worldSize);//activate root octree node
		
		//start building tree and voxel data
		m_octreeRoot->InitOctree(m_worldSize, this);

		//start generating vertices
		m_octreeRoot->GenerateWorldMesh();
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

VoxelOctree *VoxelManager::getRootNode()
{
	return m_octreeRoot;
}

VoxelOctree *VoxelManager::getOctreeChild(VoxelOctree * currentNode, int child_index)
{
	//get index of current node
	int curr = currentNode - m_octreePool;
	int child = 8 * curr + child_index + 1; //child: 8i + (1 to 8)

	if (child >= m_maxOctNodes)
	{
		std::cout << "Too many octree nodes" << std::endl;
		return NULL;
	}

	//in-array octree, return child at given index
	return &m_octreePool[child];
}

VoxelOctree *VoxelManager::createOctreeChild(VoxelOctree *currentNode, int child_index, glm::ivec3 minPos, int size)
{
	VoxelOctree *child = getOctreeChild(currentNode, child_index);

	child->InitNode(minPos, size);

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
	glm::mat4 model(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "model"), 1, GL_FALSE, &model[0][0]);

	glm::vec3 voxel_color(0.6f, 1.0f, 0.3f);

	//lighting
	glUniform3fv(glGetUniformLocation(voxel_shader, "viewPos"), 1, &player_cam->GetPosition()[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightPos"), 1, &light_pos[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightColor"), 1, &light_color[0]);

	//voxels
	glUniform3fv(glGetUniformLocation(voxel_shader, "voxelColor"), 1, &voxel_color[0]);


	m_octreeRoot->Draw();

	//VoxelOctree::SortRenderList(player_cam->GetPosition());

	//for (int i = 0; i < VoxelOctree::render_list.size(); i++)
	//{
	//	glm::vec3 position = VoxelOctree::render_list[i]->getPosition();

	///*	if (!player_cam->AABBInCamera(chunkRegion))
	//		continue;
	//	else
	//		++render_count;*/

	//	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
	//	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

	//	VoxelOctree::render_list[i]->Render();
	//}

}

void VoxelManager::RenderMinimap(GLuint shader, glm::vec2 &scale, glm::vec2 &position)
{
	GLuint model_loc = glGetUniformLocation(shader, "model");

	for (int i = 0; i < VoxelOctree::render_list.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		
		//scale down to minimap size
		model = glm::scale(model, glm::vec3(scale.x, 1.0f, scale.y));

		//translate voxels to world position
		model = glm::translate(model, VoxelOctree::render_list[i]->getPosition());

		//translate again to minimap position
		model = glm::translate(model, glm::vec3(position.x, 0.0f, position.y));		

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));		

		VoxelOctree::render_list[i]->RenderMinimap();
	}
}

bool VoxelManager::BlockWorldPosActive(glm::vec3 world_pos)
{
	VoxelChunk * chunk;

	chunk = m_octreeRoot->FindLeafChunk(world_pos);

	if (!chunk)
		return false;

	//get voxel in chunk object space
	glm::ivec3 local_pos = world_pos - chunk->getPosition();

	if (chunk->GetVoxel(local_pos) & VOXEL_TYPE_AIR)
		return false;
	
	return true;
}