#pragma once
#include "SFML\Graphics.hpp"
#include "scene.hpp"
#include "model.hpp"
#include "LightSource.hpp"
#include "camera.hpp"
#include "HUD.hpp"
#include "game.hpp"
#include "VoxelManager.hpp"
/**
* GameScene class that manages rendering the voxel world. Inherits from abstract class Scene
*/
class GameScene : public Scene
{
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


	void InitHUD();

	/**
	*@brief code to run every frame of game loop
	*/
	virtual void SceneFrame();
	/**
	*@brief Handles drawing the game scene 
	*/
	virtual void Render();

	void RenderWorld();
	void RenderMinimap();
	void RenderHUD();
	/**
	*@brief Listens to user input events and handles it
	*@param event type of user input event
	*@param input the input from client or server
	*/
	virtual void onNotify(Event event, sf::Event &input);
	/**
	*@brief Handles user input
	*@param event the user input
	*/
	virtual void HandleInput(sf::Event event) ;
private:
	HUD * m_hud;

	int m_size; /**<size of the voxel world. This is to test instance rendering */
	glm::mat4 *m_modelMatrices;/**<array of matrix model positions for instance rendering */

	Model *m_model; /**<pointer to model loaded */
	LightSource *m_light;

	VoxelManager * m_voxelManager;

	Camera *m_camera; /**<player camera*/  
	Camera *m_minimap_cam; /**<camera used to display minimap*/

	glm::vec2 m_minimap_size;//size to display minimap
	glm::vec2 m_minimap_scale;
	glm::vec2 m_minimap_pos;

	bool wire_frame = false;
};