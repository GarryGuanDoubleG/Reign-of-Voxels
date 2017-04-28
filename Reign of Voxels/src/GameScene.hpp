#pragma once
#include "SFML\Graphics.hpp"

#include "scene.hpp"
#include "LightSource.hpp"

#include "ResourceManager.hpp"
#include "entity.hpp"
#include "camera.hpp"
#include "HUD.hpp"
#include "VoxelManager.hpp"

#define MAX_ENTITES 400

/**
* GameScene class that manages rendering the voxel world. Inherits from abstract class Scene
*/
class GameScene : public Scene
{
private:
	sf::Uint16 m_flags;

	//entites
	Entity *m_entity_list;/**<object pool of entities. */
	Entity *m_next_free_entity; /**<head of free list for entites*/
	GLuint m_entity_count;

	std::vector<Ray> m_rays;
	GLuint m_raycast_vao,
		m_raycast_vbo;

	GLuint m_cubemapVAO, m_cubemapVBO;
	GLuint m_skyboxVAO, m_skyboxVBO;

	HUD * m_hud;/**<HUD handler class that managers hud inputs and rendering*/

	VoxelManager * m_voxelManager; /**<Handles Voxel Generation, rendering, and interaction*/

								   //main player camera
	Camera *m_camera; /**<player camera*/

	
					  //minimap
	Camera *m_minimapCam; /**<camera used to display minimap*/
	glm::vec2 m_minimapScale;
	
	//minimap objects
	GLuint m_minimapFBO;
	GLuint m_minimapRBO;
	GLuint m_minimapTexture;
	GLuint m_minimapVAO;//quad
	GLuint m_minimapVBO;//quad



	Model * m_model; // test model, get rid of it once entities work

	bool wire_frame = false;
	bool draw_textured = false;
	bool m_worldChanged;

public:
	/**
	* constructor
	* Subscribes to event system and sets up a camera and loads models
	*/
	GameScene();
	/**
	* destructor
	* unSubscribes from event system
	*/
	~GameScene();

	/**
	*@brief code to run every frame of game loop
	*/
	virtual void SceneFrame();

	/**
	*@brief Listens to user input events and handles it
	*@param event type of user input event
	*@param input the input from client or server
	*/
	virtual void onNotify(Event event, sf::Event &input);
private:

	void InitRayVertex();
	void InitMinimap();
	void InitSkybox();

	/**
	*@brief Updates game world 
	*/
	void Update();

	/**
	*@brief Handles user input
	*@param event the user input
	*/
	virtual void HandleInput(sf::Event event);

	/**
	*@brief Handles drawing the game scene
	*/
	virtual void Render();
	/**
	* @brief Renders the voxel terrain
	*/
	void RenderWorld();

	void RenderEntities();
	void RenderModel(Entity *entity);
	void RenderSkybox();
	void RenderAABB(Entity *entity, GLuint shader);
	void RenderRayCast();

	/**
	* @brief Renders a scaled down version of world
	*/
	void RenderMinimap();

	/**
	* @brief Loads model into global array
	*/
	void CreateEntity();

};