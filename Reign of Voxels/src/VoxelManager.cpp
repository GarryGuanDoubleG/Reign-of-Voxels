#include "VoxelManager.hpp"
#include "VoxelNoise.hpp"
#include "texture.hpp"
#include "game.hpp"
#include "simple_logger.h"

VoxelManager::VoxelManager()
{
	m_voxelModel = new Model("Resources\\models\\cube.obj");
}
VoxelManager::~VoxelManager()
{

}

void VoxelManager::GenerateVoxelChunks(sf::Image *heightmap)
{
	int chunk_size = 16;
	int scale = 10;
	int max_height = 512;
	int world_size = heightmap->getSize().x;
	int chunk_count = 0;

}
void VoxelManager::GenerateVoxels()
{
	m_worldSize = 64;

	sf::Image *heightmap = new sf::Image();
	if (!heightmap->loadFromFile(GenerateTerrainMap(m_worldSize)))
		slog("Failed to Load or Generate HeightMap");
	else
	{
		//GenerateVoxelChunks(heightmap);
		m_octree = new VoxelOctree(NULL);
		m_octree->InitializeOctree(heightmap, m_worldSize);
	}
	delete heightmap;
}

void VoxelManager::RenderVoxels(Camera * player_cam)
{
	GLuint voxel_shader = GetShader("model");
	GLuint model_loc, view_loc, proj_loc, 
		light_loc, obj_loc, light_pos_loc, view_pos_loc;

	model_loc = glGetUniformLocation(voxel_shader, "model");
	view_loc = glGetUniformLocation(voxel_shader, "view");
	proj_loc = glGetUniformLocation(voxel_shader, "projection");
	light_loc = glGetUniformLocation(voxel_shader, "lightColor");
	obj_loc = glGetUniformLocation(voxel_shader, "objectColor");
	light_pos_loc = glGetUniformLocation(voxel_shader, "lightPos");
	view_pos_loc = glGetUniformLocation(voxel_shader, "viewPos");
	//
	Vec3 light_pos = Vec3(64, 64, 64);
	Vec3 light_color = Vec3(1.0f, 1.0f, 1.0f);
	Vec3 voxel_color = Vec3(.2f, .6f, .2f);
	Mat4 view = player_cam->GetViewMat();
	Mat4 proj = player_cam->GetProj();
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

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