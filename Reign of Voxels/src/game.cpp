#include "game.h"
#include "simple_logger.h"

sf::Clock g_delta_clock;
sf::Clock g_clock;

Game Game::m_instance;

Game::Game()
{
	//true after Initialize()
	m_initialized = false;
	//initialilze members
	slog("Game init");
}

void Game::Initialize()
{
	if (!m_initialized)
	{
		GraphicsInit();
		compile_shaders();
		m_initialized = true;
	}
	if (m_initialized)
	{
		m_instance.m_sceneManager = new SceneManager(new Menu(LoginMenu)); // first scene will be menu
		m_instance.m_eventSystem = new Subject();
		m_instance.m_client = new Client();
		m_eventSystem->addObserver(m_sceneManager);
		m_eventSystem->addObserver(m_client);
	}
}

void Game::GameLoop()
{
	while (g_window->isOpen())
	{
		g_delta_clock.restart();
		m_sceneManager->SceneFrame();
	}
}