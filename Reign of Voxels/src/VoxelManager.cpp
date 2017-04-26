#include <mutex>
#include "ResourceManager.hpp"
#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

std::mutex g_free_chunk_guard;

VoxelManager::VoxelManager()
{
	//TODO load voxel world from json
	m_resolution = 256;
	
	int worldSizeXZ = m_resolution * m_resolution;

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
	int maxOctree =  (int)log2(m_resolution);
		maxOctree = (1 << (3 * maxOctree) ) ; //2^3 times for 8 child nodes

	m_maxOctNodes = maxOctree;
	sizeof(VoxelOctree);

	m_octreePool = new VoxelOctree[maxOctree];
	m_octreeRoot = &m_octreePool[0];
}

VoxelManager::~VoxelManager()
{

}

int VoxelManager::GetWorldSize()
{
	return m_resolution;
}

void VoxelManager::GenerateVoxels()
{
	sf::Image *heightmap = new sf::Image();

	if (!heightmap->loadFromFile(GenerateTerrainMap(m_resolution)))
		slog("Failed to Load or Generate HeightMap");
	else
	{
		glm::ivec3 rootMinPos(0.0f);

		m_octreeRoot->InitNode(rootMinPos, m_resolution);//activate root octree node
		
		//start building tree and voxel data
		m_octreeRoot->InitOctree(m_resolution, this);

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

	glUseProgram(voxel_shader);

	glm::vec3 light_pos = glm::vec3(256, 512, 256);
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 view = player_cam->GetViewMat();
	glm::mat4 proj = player_cam->GetProj();
	glm::mat4 model(1.0f);
	//glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, 4.0f));

	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "model"), 1, GL_FALSE, &model[0][0]);

	glm::vec3 voxel_color(0.6f, 1.0f, 0.3f);

	//lighting
	glUniform3fv(glGetUniformLocation(voxel_shader, "viewPos"), 1, &player_cam->GetPosition()[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightPos"), 1, &light_pos[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightColor"), 1, &light_color[0]);


	// Now set the sampler to the correct texture unit
	glUniform1i(glGetUniformLocation(voxel_shader, "voxel_texture"), 0);
	glUniform1i(glGetUniformLocation(voxel_shader, "normalMap"), 0);
	// And finally bind the texture
	//textures
	GLuint texture = GetTextureID("grass");
	GLuint normalMap = GetNormalMapID("grass");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);

	m_octreeRoot->Draw();

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
	VoxelOctree * node = m_octreeRoot->FindLeafNode(world_pos);

	if (!node)
		return false;
	
	return true;
}