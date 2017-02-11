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

//function meant for testing model loading code
void RenderScene(Model *model, Mat4 &mvp)
{
	GLfloat bg_color[] = { 0.0f, 0.0f, 0.5f, 0.0f };
	GLuint mvp_location;

	mvp_location = glGetUniformLocation(g_shader_prog, "mvp");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glUseProgram(g_shader_prog);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

	model->Draw(g_shader_prog);

	g_window->display();
}

void Game::GameLoop()
{
	while (g_window->isOpen())
	{
		m_sceneManager->SceneFrame();
	}
}