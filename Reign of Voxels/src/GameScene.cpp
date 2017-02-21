#include "GameScene.hpp"
/**
* constructor
* Subscribes to event system and sets up a camera and loads models
*/
GameScene::GameScene()
{
	m_camera = new Camera();	


	//subscribe to global events
	m_size = 512;
	int voxel_amount = m_size * m_size * m_size / 16 / 16 / 16;
	m_model = new Model("Resources\\models\\cube.obj");

	if (m_model)
	{
		float chunk_size = 16;
		m_modelMatrices = new Mat4[voxel_amount];
		unsigned int count = 0;
		for (GLfloat x = 0; x < m_size ; x+=chunk_size)
		{
			for (GLfloat y = 0; y < m_size; y+=chunk_size)
			{
				for (GLfloat z = 0; z < m_size; z+=chunk_size)
				{
					Mat4 model;
					model = glm::scale(model, Vec3(chunk_size));
					model = glm::translate(model, Vec3(x/ chunk_size, y/ chunk_size, z/ chunk_size));

					m_modelMatrices[count++] = model;
				}
			}
		}
	}
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, voxel_amount * sizeof(glm::mat4), &m_modelMatrices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_model->SetInstanceRendering(buffer, voxel_amount);
	Game::instance().getEventSystem().addObserver(this);
}
/**
* destructor
* unSubscribes from event system
*/
GameScene::~GameScene()
{
	Game::instance().getEventSystem().removeObserver(this);
}
/**
*@brief code to run every frame of game loop
*/
void GameScene::SceneFrame()
{
	Render();
}
/**
*@brief Handles drawing the game scene
*/
void GameScene::Render()
{
	GLuint view_loc, proj_loc;
	GLfloat bg_color[] = { 0.1f, 0.1f, 0.1f, 0.2f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glUseProgram(g_shader_prog);

	view_loc = glGetUniformLocation(g_shader_prog, "view");
	proj_loc = glGetUniformLocation(g_shader_prog, "projection");

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMat()));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetProj()));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_model->Draw(g_shader_prog);

	Game::instance().getWindow()->display();
}
/**
*@brief Listens to user input events and handles it
*@param event type of user input event
*@param input the input from client or server
*/
void GameScene::onNotify(Event event, sf::Event &input)
{
	/*if (event == ServerInput)*/
		HandleInput(input); 
}
/**
*@brief Handles user input
*@param event the user input
*/
void GameScene::HandleInput(sf::Event event)
{
	m_camera->HandleInput(event);
}