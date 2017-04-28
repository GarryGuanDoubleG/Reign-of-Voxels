#include "game.hpp"
#include "GameScene.hpp"
#include "simple_logger.h"

float Game::delta_time;
sf::Clock Game::clock; /**<tracks total time since ininitation of gamescene*/  

int Game::screen_width;
int Game::screen_height;

Game Game::m_instance; /**<singleton instance of game */ 


#define FPS_TIME 0.01666666666f /**<total time per frame in seconds. 60 dps */ 

/**
* Constructor
* initializes event system
*/
Game::Game()
	: m_eventSystem(new EventSystem()),
	  m_initialized(false)
{	
}
/**
* Destrutor
* cleans up member classes
*/
Game::~Game()
{
	delete m_sceneManager;
	delete m_client;
	delete m_eventSystem;
}
/*
* @brief Initializes sfml window and glew
*/
void Game::GraphicsInit()
{
	int err;
	sf::ContextSettings settings;
	GLuint vert_buff, uv_buff;

	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = .0;

	m_resourceManager = new ResourceManager();
	m_resourceManager->LoadConfig();

	screen_width = m_resourceManager->GetConfigSetting("screen_width");
	screen_height = m_resourceManager->GetConfigSetting("screen_height");

	//Create context
	m_window = new sf::RenderWindow(sf::VideoMode(Game::screen_width, Game::screen_height), "Reign of Voxels", sf::Style::Default, settings);
	m_window->setMouseCursorGrabbed(false);
	m_window->setMouseCursorVisible(true);

	m_window->setVerticalSyncEnabled(true);

	if ((err = glewInit()) != GLEW_OK)
	{
		printf("Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}
/*
* @brief initializes the game from graphics to loading scene and managing client connection
*/
void Game::Initialize()
{
	if (m_initialized) return;

	GraphicsInit();
	CompileAllShaders();

	m_instance.m_sceneManager = new SceneManager(new GameScene()); // first scene will be menu
	m_instance.m_client = new Client();//managers communication with the server

	m_initialized = true;
	m_lock_mouse = false;

	m_resourceManager->LoadResources();
}
/*
* @brief throws a close game event to start clean up process
*/
void Game::GameClose()
{
	Json event;
	event["event"] = Event::Close;

	m_running = false;
	m_instance.getEventSystem().Notify(Event::Close, event);

	delete m_resourceManager;

	exit(0);
}
/*
* @brief returns a reference to the global game class
* @return reference to game class
*/
Game& Game::instance()
{ 
	return m_instance; 
}
/*
* @brief returns a reference to the global event system
* @return reference to the global event system
*/
EventSystem& Game::getEventSystem() 
{ 
	return *m_eventSystem; 
}
/*
* @brief returns a pointer to the game window
* @return pointer to RenderWindow
*/
sf::RenderWindow*	Game::getWindow() 
{ 
	return m_window; 
}

void Game::HandleInput()
{
	sf::Event event;

	//get user inputs
	while (m_window->pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Q:
			{
				m_lock_mouse = !m_lock_mouse;

				if (m_lock_mouse)
				{
					Game::instance().getWindow()->setMouseCursorGrabbed(true);
					Game::instance().getWindow()->setMouseCursorVisible(false);
				}
				else
				{
					Game::instance().getWindow()->setMouseCursorGrabbed(false);
					Game::instance().getWindow()->setMouseCursorVisible(true);
				}
				break;
			}
			case sf::Keyboard::Escape:
				GameClose();
				break;
			default:
				break;
			}

		}
		m_eventSystem->Notify(ClientInput, event);
	}
}

/*
* @brief Game loop that handles what happens every frame
*/
void Game::GameLoop()
{
	m_running = true;
	clock.restart();
	
	//center mouse in window
	sf::Mouse::setPosition(sf::Vector2i(Game::screen_width / 2, Game::screen_height / 2), *m_window);
	
	float last_time = 0;

	while (m_running)
	{

		HandleInput();

		if (m_lock_mouse)
		{
			sf::Mouse::setPosition(sf::Vector2i(Game::screen_width / 2, Game::screen_height / 2), *m_window);
		}

		m_sceneManager->SceneFrame();

		delta_time = clock.getElapsedTime().asSeconds() - last_time;
		last_time = clock.getElapsedTime().asSeconds();

		//std::cout << "Framerate: " << 1.0f / delta_time << std::endl;
	}
	m_window->close();
}