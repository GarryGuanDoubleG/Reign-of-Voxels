#pragma once
#ifndef _GAME_H_
#define _GAME_H_
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "EventSystem.hpp"
#include "shader.hpp"
#include "sceneManager.hpp"
#include "Gui\Menu.hpp"
#include "client.hpp"
#include "ResourceManager.hpp"

//#define Game::screen_width 1280 /**<width of the window */ 
//#define Game::screen_height 720 /**<height of the window */ 

/**
* Singleton Game class
* Handles game loops and stores all managers & systems
*/
class Game
{
public:
	bool				m_lock_mouse;
	ResourceManager			*m_resourceManager;

	static float delta_time;
	static sf::Clock clock;/**<tracks total time since ininitation of gamescene*/  
	static int Game::screen_width;
	static int Game::screen_height;

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

	void HandleInput();

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