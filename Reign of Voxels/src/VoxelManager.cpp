#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

VoxelManager::VoxelManager()
{
	m_worldSize = 512;
	
	int worldSizeXZ = m_worldSize * m_worldSize;

	//intialize chunkpool
	int maxChunks = worldSizeXZ * (VoxelOctree::maxHeight * 2) / VoxelChunk::CHUNK_SIZE_CUBED;

	m_chunkPool = new VoxelChunk[maxChunks];
	
	//set next available nodes
	for (int i = 0; i < maxChunks - 1; i++)
		m_chunkPool[i].m_next = &m_chunkPool[i + 1];


	//intialize octree
	int maxOctree =  (int)log2(m_worldSize) - (int)log2(VoxelChunk::CHUNK_SIZE);
		maxOctree = 1 << (3 * maxOctree); //2^3 times for 8 child nodes

	m_octreePool = new VoxelOctree[maxOctree];
	sizeof(VoxelOctree);
	m_freeOctreeHead = m_octreePool;

	//set next available nodes
	for (int i = 0; i < maxOctree - 1; i++)
		m_octreePool[i].m_nextFree = &m_octreePool[i + 1];
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
		m_octreeRoot = createOctreeNode(NULL);//root octree
		m_octreeRoot->InitializeOctree(heightmap, m_worldSize, this);
	}

	delete heightmap;
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
	Vec3 light_pos = Vec3(64, 256, 64);
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
		//model = glm::scale(model, Vec3(10.0f, 10.0f, 10.0f));
		model = glm::translate(model, VoxelOctree::render_list[i]->getPosition());
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

		VoxelOctree::render_list[i]->Render();
	}
	//glPolygonMode(GL_FRONT, GL_FILL);
}

VoxelChunk * VoxelManager::createChunk(Vec3 worldPosition)
{
	VoxelChunk * chunk = m_freeChunkHead;
	chunk->m_flag |= CHUNK_FLAG_INUSE;

	m_freeChunkHead = m_freeChunkHead->m_next;
	return chunk;
}

void VoxelManager::destroyChunk(VoxelChunk * chunk)
{
	chunk->m_flag = 0; //clear in use flag
	chunk->m_next = m_freeChunkHead;

	memset(chunk->m_voxels, VOXEL_TYPE_AIR, sizeof(chunk->m_voxels));

	m_freeChunkHead = chunk; //prepend chunk to head of free list
}

VoxelOctree *VoxelManager::createOctreeNode(VoxelOctree *parent)
{
	VoxelOctree *node = m_freeOctreeHead;

	m_freeOctreeHead = m_freeOctreeHead->m_nextFree;
	
	node->InitNode(parent);

	return node;
}

void VoxelManager::destroyOctreeNode(VoxelOctree * node)
{
	node->m_nextFree = m_freeOctreeHead;

	node->DestroyNode(); //clears minimal data for reuse

	m_freeOctreeHead = node;//prepend node to free list
}