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
	GraphicsClose();
}

void Game::GameLoop()
{
	m_running = true;
	g_clock.restart();

	while (m_running)
	{
		g_delta_clock.restart();
		m_sceneManager->SceneFrame();
	}
}