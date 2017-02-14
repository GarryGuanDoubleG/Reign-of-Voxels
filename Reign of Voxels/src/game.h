#pragma once
#ifndef _GAME_H_
#define _GAME_H_
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "EventSystem.h"
#include "glm.h"
#include "shader.h"
#include "model.h"
#include "sceneManager.h"
#include "Gui\Menu.h"
#include "client.h"
/**
* Singleton Game class
* Handles game loops and stores all managers & systems
*/
class Game
{
public:
	static sf::Clock g_delta_clock; /**<timer that tracks time since last iteration of game loop*/  
	static sf::Clock g_clock;/**<tracks total time since ininitation of gamescene*/  

	/**
	* Constructor
	* initializes event system
	*/
	Game();
	/**
	* Destrutor
	* cleans up member classes
	*/
	~Game();
	/*
	* @brief initializes the game from graphics to loading scene and managing client connection
	*/
	void					Initialize();
	/*
	* @brief Game loop that handles what happens every frame
	*/
	void					GameLoop();
	/*
	* @brief returns a reference to the global game class
	* @return reference to game class
	*/
	static Game&			instance();
	/*
	* @brief returns a reference to the global event system
	* @return reference to the global event system
	*/
	EventSystem&			getEventSystem();
	/*
	* @brief returns a pointer to the game window
	* @return pointer to RenderWindow
	*/
	sf::RenderWindow*		getWindow();
private:
	/*
	* @brief Initializes sfml window and glew
	*/
	void					GraphicsInit();
	/*
	* @brief throws a close game event to start clean up process 
	*/
	void					GameClose();

	static Game				m_instance; /**<singleton instance of game */  
	sf::RenderWindow		*m_window; /**<the window to render to*/  
	
	//managers
	SceneManager			*m_sceneManager; /**< manages which scene runs in the foreground*/  
	EventSystem				*m_eventSystem; /**<Throws events from anywhere in code*/  
	Client					*m_client; /**<manages connection to server*/  

	//flags
	bool					m_in_game; /**<flag if game already initiatd */  
	bool					m_running; /**<flag for whether or not to run gameloop*/  
	bool					m_initialized; /**< flag for whether initialized or not*/
};

#endif