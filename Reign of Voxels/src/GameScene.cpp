#include "GameScene.h"


GameScene::GameScene()
{
	m_camera = new Camera();	
	//m_model = new Model("Resources\\models\\nanosuit\\nanosuit.obj");



	//subscribe to global events
	Game::instance().getEventSystem().addObserver(this);
}

GameScene::~GameScene()
{
	Game::instance().getEventSystem().removeObserver(this);
}

void GameScene::SceneFrame()
{
	Render();
}

void GameScene::Render()
{
	GLuint model_loc, view_loc, proj_loc;
	GLfloat bg_color[] = { 0.1f, 0.1f, 0.1f, 0.2f };

	Mat4 model_mat4 = Mat4(1.0f);
	model_mat4 = glm::translate(model_mat4, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	model_mat4 = glm::scale(model_mat4, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glUseProgram(g_shader_prog);

	model_loc = glGetUniformLocation(g_shader_prog, "model");
	view_loc = glGetUniformLocation(g_shader_prog, "view");
	proj_loc = glGetUniformLocation(g_shader_prog, "projection");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_mat4));
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMat()));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetProj()));

	if(m_model)
		m_model->Draw(g_shader_prog);
	
}

void GameScene::onNotify(Event event, sf::Event &input)
{
	if(event == ServerInput)
		HandleInput(input);
}

void GameScene::HandleInput(sf::Event event)
{
	m_camera->HandleInput(event);
}