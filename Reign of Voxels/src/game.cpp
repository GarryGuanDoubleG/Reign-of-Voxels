#include "game.h"
#include "simple_logger.h"

sf::Clock g_delta_clock;
sf::Clock g_clock;

Game Game::m_instance;

//event system needs to be initialized first
Game::Game()
	: m_eventSystem(new EventSystem()),
	  m_initialized(false)
{	
}

Game::~Game()
{
	delete m_sceneManager;
	delete m_client;
	delete m_eventSystem;

	
}

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
	//Create context
	m_window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Reign of Voxels", sf::Style::Default, settings);
	/*Game::instance().getWindow()->setMouseCursorGrabbed(false);
	Game::instance().getWindow()->setMouseCursorVisible(false);*/
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
}

void Game::Initialize()
{
	if (m_initialized) return;

	GraphicsInit();
	compile_shaders();

	m_instance.m_sceneManager = new SceneManager(new Menu(LoginMenu)); // first scene will be menu
	m_instance.m_client = new Client();//managers communication with the server

	m_initialized = true;
}

void Game::Close()
{
	Json event;
	event["event"] = Event::Close;

	m_running = false;
	m_instance.getEventSystem().Notify(Event::Close, event);
}

Game& Game::instance()
{ 
	return m_instance; 
}

EventSystem& Game::getEventSystem() 
{ 
	return *m_eventSystem; 
}
sf::RenderWindow*	Game::getWindow() 
{ 
	return m_window; 
}


void Game::GameLoop()
{
	m_running = true;
	g_clock.restart();
	
	while (m_running)
	{
		sf::Event event;
		g_delta_clock.restart();

		m_window->clear(sf::Color::Black);
		
		while (m_window->pollEvent(event))
		{
			m_eventSystem->Notify(ClientInput, event);
		}

		m_sceneManager->SceneFrame();
		m_window->display();
	}
}