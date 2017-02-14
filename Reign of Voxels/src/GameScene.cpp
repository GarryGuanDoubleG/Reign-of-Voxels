#include "GameScene.h"


GameScene::GameScene()
{
	m_camera = new Camera();	
	//m_model = new Model("Resources\\models\\nanosuit\\nanosuit.obj");
	m_model = new Model("Resources\\models\\cube.obj");

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
	model_mat4 = glm::scale(model_mat4, glm::vec3(0.05f, 0.05f, 0.05f));	// It's a bit too big for our scene, so scale it down

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glUseProgram(g_shader_prog);

	model_loc = glGetUniformLocation(g_shader_prog, "model");
	view_loc = glGetUniformLocation(g_shader_prog, "view");
	proj_loc = glGetUniformLocation(g_shader_prog, "projection");

	/*glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_mat4));
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMat()));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetProj()));
*/
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (m_model)
	{
		for (float x = 0; x < 4; x++)
		{
			for (float y = 0; y < 4; y++)
			{
				for (float z = 0; z < 4; z++)
				{
					Mat4 model_mat4 = Mat4(1.0f);
					model_mat4 = glm::scale(model_mat4, glm::vec3(0.05f, 0.05f, 0.05f));	// It's a bit too big for our scene, so scale it down
					model_mat4 = glm::translate(model_mat4, glm::vec3(x, y, z)); // Translate it down a bit so it's at the center of the scene

					glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_mat4));
					glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMat()));
					glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetProj()));

					m_model->Draw(g_shader_prog);
				}
			}
		}

	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Game::instance().getWindow()->display();
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