#pragma once
#include "SFML\Graphics.hpp"
#include "scene.h"
#include "model.h"
#include "camera.h"
#include "game.h"
#include "Voxel.h"
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
	/**
	*@brief code to run every frame of game loop
	*/
	virtual void SceneFrame();
	/**
	*@brief Handles drawing the game scene 
	*/
	virtual void Render();
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
	static Voxel *m_Voxels[32][32][32]; /**<3d container of voxels for rendering */  

	Model *m_model; /**<pointer to model loaded */  
	Camera *m_camera; /**<player camera*/  
};