#include <mutex>
#include "ResourceManager.hpp"
#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

std::mutex g_free_chunk_guard;

VoxelManager::VoxelManager(int worldSize)
{
	//TODO load voxel world from json
	m_worldSize = worldSize;
	
	int worldSizeXZ = m_worldSize * m_worldSize;

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
	int maxOctree = (int)log2(m_worldSize) - 1;
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
	return m_worldSize;
}


void VoxelManager::GenerateVoxels()
{
	glm::ivec3 rootMinPos(0.0f);

	GenerateTerrainMap(m_worldSize);
	m_octreeRoot->InitNode(rootMinPos, m_worldSize);//activate root octree node
		
	//start building tree and voxel data
	m_octreeRoot->InitOctree(m_worldSize, this);

}

void VoxelManager::GenerateChunks()
{
	//TODO generate octree bottom up then assign to chunks
	//chunks are SVO with cubic bound box

	for (int x = 0; x < m_worldSize; x += VoxelChunk::CHUNK_SIZE)
	{
		for (int y = 0; y <= VoxelOctree::maxHeight; y += VoxelChunk::CHUNK_SIZE)
		{
			for (int z = 0; z < m_worldSize; z += VoxelChunk::CHUNK_SIZE)
			{
				glm::vec3 chunkPos(x, y, z);
				CreateChunk(chunkPos, m_octreeRoot->FindLeafNode(chunkPos));
			}
		}
	}

}

VoxelChunk * VoxelManager::CreateChunk(glm::vec3 worldPosition, VoxelOctree *node)
{
	if (!node)
	{
		return NULL;
	}
	if (m_freeChunkHead == m_freeChunkHead->m_next)
	{
		std::cout << "free list is empty" << std::endl;
	}

	VoxelChunk * chunk = m_freeChunkHead;
	m_freeChunkHead = chunk->m_next;

	chunk->Init(worldPosition, node);

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

void VoxelManager::RenderMinimap(Camera * player_cam)
{
	GLuint voxel_shader = GetShader("minimap");

	glUseProgram(voxel_shader);

	glm::mat4 model(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(voxel_shader, "model"), 1, GL_FALSE, &model[0][0]);

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

	GLint samplers[4] = { 0, 1, 2, 3 };
	glUniform1iv(glGetUniformLocation(voxel_shader, "voxelTexture"), 4, &samplers[0]);

	m_octreeRoot->Draw();
}

void VoxelManager::RenderGrass(Camera * player_cam)
{
	GLuint shader = GetShader("grass");
	glUseProgram(shader);
	
	glDepthFunc(GL_ALWAYS);
	/*glDisable(GL_DEPTH_FUNC);
	glDisable(GL_DEPTH_TEST);*/

	glm::vec3 light_pos = glm::vec3(256, 512, 256);
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

	
	glm::mat4 model(1.0f);

	glm::vec2 billboardSize(1.0f, 2.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &player_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &player_cam->GetProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);

	glUniform3fv(glGetUniformLocation(shader, "cameraRight"), 1, &player_cam->GetRight()[0]);
	glUniform3fv(glGetUniformLocation(shader, "cameraUp"), 1, &player_cam->GetUp()[0]);
	glUniform2fv(glGetUniformLocation(shader, "billboardSize"), 1, &billboardSize[0]);
	glUniform1f(glGetUniformLocation(shader, "time"), Game::clock.getElapsedTime().asSeconds());

	GLuint grass = GetTextureID("billboard_grass");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass);

	glUniform1i(glGetUniformLocation(shader, "grassTexture"), 0);

	m_octreeRoot->DrawGrass();
	
	glActiveTexture(GL_TEXTURE0);

	glDepthFunc(GL_LESS);
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

	VoxelOctree::Draw(voxel_shader);
	//m_octreeRoot->Draw();
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

bool VoxelManager::BlockWorldPosActive(glm::vec3 world_pos)
{
	VoxelOctree * node = m_octreeRoot->FindLeafNode(world_pos);

	if (!node || ~node->m_flag & OCTREE_ACTIVE)
		return false;
	
	/*VoxelChunk * chunk = m_octreeRoot->FindChunk(world_pos);
	chunk->m_render_mode = !chunk->m_render_mode;*/

	return true;
}

void VoxelManager::DestroyVoxel(glm::ivec3 world_pos, glm::ivec3 face)
{
	if (world_pos.y <= 1)
	{
		return;
	}

	VoxelChunk * chunk = m_octreeRoot->FindChunk(world_pos);

	//DestroyVoxel(world_pos, m_node);
	glm::ivec3 min = chunk->m_node->m_min;
	glm::ivec3 max = min + VoxelChunk::CHUNK_SIZE - 1;

	chunk->m_csgOpPos.push_back(world_pos);

	if (world_pos.x == min.x || world_pos.y == min.y || world_pos.z == min.z ||
		world_pos.x == max.x || world_pos.y == max.y || world_pos.z == max.z)
	{
		glm::ivec3 neighborPos = world_pos - face;
		VoxelChunk *neighbor = chunk->m_node->FindChunk(neighborPos);

		VoxelOctree *leaf_node = NULL;
		if (!neighbor || ~neighbor->m_node->m_flag & OCTREE_ACTIVE)
		{
			leaf_node = InitNode(neighborPos, 1);
			m_octreeRoot->AssignLeafNode(leaf_node);

			neighbor = m_octreeRoot->FindChunk(neighborPos);
			neighbor->m_node->BuildTree(neighborPos);

			neighbor->m_csgOpPos.push_back(neighborPos);
		}
		else
		{
			neighbor->m_csgOpPos.push_back(neighborPos);
			neighbor->m_node->BuildTree(neighbor->m_csgOpPos);
		}

		neighbor->m_vertices.clear();
		neighbor->m_tri_indices.clear();

		neighbor->m_node->GenerateVertexIndices(neighbor->m_vertices);
		neighbor->m_node->ContourCellProc(neighbor->m_tri_indices);

		neighbor->GenerateSeam();
		neighbor->BindMesh();
	}

	chunk->m_csgOpPos.push_back(world_pos);
	chunk->m_node->BuildTree(chunk->m_csgOpPos);

	chunk->m_vertices.clear();
	chunk->m_tri_indices.clear();

	chunk->m_node->GenerateVertexIndices(chunk->m_vertices);
	chunk->m_node->ContourCellProc(chunk->m_tri_indices);

	chunk->GenerateSeam();
	chunk->BindMesh();
}


//void VoxelManager::GenerateWater()
//{
//
//}