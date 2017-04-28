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
	m_resolution = 512;
	
	int worldSizeXZ = m_resolution * m_resolution;

	//intialize chunkpool
	int maxChunks = worldSizeXZ * (VoxelOctree::maxHeight * 2) / VoxelChunk::CHUNK_SIZE_CUBED;

	m_maxChunks = maxChunks;
	
	m_chunkPool = new VoxelChunk[maxChunks];
	m_freeChunkHead = &m_chunkPool[0];
	
	sizeof(VoxelChunk);

	//set next available nodes
	for (int i = 0; i < maxChunks - 1; i++)
	{
		m_chunkPool[i].m_next = &m_chunkPool[i + 1];
	}

	//Allocate space for Octree. 2^(3h + 1) - 1 nodes
	int maxOctree = (int)log2(m_resolution) - 2;
	maxOctree = (1 << (3 * maxOctree)); //2^3 times for 8 child nodes

	m_maxOctNodes = maxOctree;

	m_octreeRoot = new VoxelOctree[maxOctree];
	memset(m_octreeRoot, 0, sizeof(VoxelOctree) * maxOctree);

	for (int i = 0; i < maxOctree - 1; i++)
	{
		m_octreeRoot[i].next_free = &m_octreeRoot[i + 1];
	}

	m_nextFreeNode = m_octreeRoot->next_free;
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
	glm::ivec3 rootMinPos(0.0f);

	GenerateTerrainMap(m_resolution);
	m_octreeRoot->InitNode(rootMinPos, m_resolution);//activate root octree node
		
	//start building tree and voxel data
	m_octreeRoot->InitOctree(m_resolution, this);

	//start generating vertices&
	m_octreeRoot->GenerateWorldMesh();
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
	if (chunk->m_flag != 0)
	{
		chunk->Destroy();
	}


	chunk->m_next = m_freeChunkHead;
	m_freeChunkHead = chunk; //prepend chunk to head of free list
}

VoxelOctree *VoxelManager::getRootNode()
{
	return m_octreeRoot;
}


VoxelOctree *VoxelManager::InitNode(glm::ivec3 minPos, int size)
{
	if (!m_nextFreeNode)
	{
		slog("Out of nodes");
	}

	VoxelOctree * node = m_nextFreeNode;	
	m_nextFreeNode = m_nextFreeNode->next_free;

	node->next_free = NULL;
	node->InitNode(minPos, size);
	return node;
}


void VoxelManager::destroyOctreeNode(VoxelOctree * node)
{
	//prepend free node
	node->next_free = m_nextFreeNode;
	m_nextFreeNode = node;
}

void VoxelManager::RenderWorld(bool draw_textured, Camera * player_cam)
{
	RenderVoxels(draw_textured, player_cam);
	RenderGrass(player_cam);
}

void VoxelManager::RenderGrass(Camera * player_cam)
{
	GLuint shader = GetShader("grass");
	glUseProgram(shader);

	glm::vec3 light_pos = glm::vec3(256, 512, 256);
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

	//glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	glm::mat4 model(1.0f);

	glm::vec2 billboardSize(1.0f, 2.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);

	glUniform3fv(glGetUniformLocation(shader, "cameraRight"), 1, &player_cam->GetRight()[0]);
	glUniform3fv(glGetUniformLocation(shader, "cameraUp"), 1, &player_cam->GetUp()[0]);
	glUniform2fv(glGetUniformLocation(shader, "billboardSize"), 1, &billboardSize[0]);

	GLuint grass = GetTextureID("billboard_grass");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass);

	glUniform1i(glGetUniformLocation(shader, "grassTexture"), 0);

	m_octreeRoot->DrawGrass();
	
	glActiveTexture(GL_TEXTURE0);
}

void VoxelManager::RenderVoxels(bool draw_textured, Camera * player_cam)
{
	if (draw_textured)
	{
		RenderVoxelTextured(player_cam);
		return;
	}

	GLuint voxel_shader = GetShader("voxel");

	glUseProgram(voxel_shader);

	glm::vec3 light_pos = glm::vec3(256, 512, 256);
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 model(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "model"), 1, GL_FALSE, &model[0][0]);

	glm::vec3 voxel_color(0.8f, 0.0f, 0.3f);

	//lighting
	glUniform3fv(glGetUniformLocation(voxel_shader, "viewPos"), 1, &player_cam->GetPosition()[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightPos"), 1, &light_pos[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightColor"), 1, &light_color[0]);

	//voxels
	glUniform3fv(glGetUniformLocation(voxel_shader, "voxelColor"), 1, &voxel_color[0]);


	m_octreeRoot->Draw();
}

void VoxelManager::RenderVoxelTextured(Camera *player_cam)
{
	GLuint voxel_shader = GetShader("voxelTex");

	glUseProgram(voxel_shader);

	glm::vec3 light_pos = glm::vec3(256, 512, 256);
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 model(1.0f);
	//glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, 4.0f));

	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "model"), 1, GL_FALSE, &model[0][0]);

	//lighting
	glUniform3fv(glGetUniformLocation(voxel_shader, "viewPos"), 1, &player_cam->GetPosition()[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightPos"), 1, &light_pos[0]);
	glUniform3fv(glGetUniformLocation(voxel_shader, "lightColor"), 1, &light_color[0]);


	// Now set the sampler to the correct texture unit
	
	// And finally bind the texture
	//textures
	GLuint grass = GetTextureID("grass");
	GLuint water = GetTextureID("water");
	GLuint snow = GetTextureID("snow");
	GLuint dirt = GetTextureID("dirt");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, water);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, snow);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, dirt);

	glActiveTexture(GL_TEXTURE0);

	GLint samplers[4] = { 0, 1, 2, 3};
	glUniform1iv(glGetUniformLocation(voxel_shader, "voxelTexture"), 4, &samplers[0]);

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

	if (!node || ~node->m_flag & OCTREE_ACTIVE)
		return false;
	
	return true;
}