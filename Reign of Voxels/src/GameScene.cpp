#include "GameScene.hpp"
#include "simple_logger.h"
/**
* constructor
* Subscribes to event system and sets up a camera and loads models
*/#
GameScene::GameScene()
{
	std::cout << "Init Game \n";

	m_camera = new Camera();	
	
	//m_model = new Model("Resources\\models\\nanosuit\\nanosuit.obj");
	m_model = new Model("Resources\\models\\cube.obj");
	m_light = new LightSource(); 
	m_light->m_model = m_model;// use the same model for the lighitng for now
	/*m_voxelManager = new VoxelManager();
	m_voxelManager->GenerateVoxels();*/
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
	Mat4 model(1.0f);
	GLuint model_shader, light_shader;
	GLuint model_loc, view_loc, proj_loc, light_loc, obj_loc, light_pos_loc, view_pos_loc;
	GLfloat bg_color[] = { 0.2f, 0.25f, 0.2f, 0.3f };

 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glClearBufferfv(GL_COLOR, 0, bg_color);
	model_shader = GetShader("model");
	glUseProgram(model_shader);

	model_loc = glGetUniformLocation(model_shader, "model");
	view_loc = glGetUniformLocation(model_shader, "view");
	proj_loc = glGetUniformLocation(model_shader, "projection");
	light_loc = glGetUniformLocation(model_shader, "lightColor");
	obj_loc = glGetUniformLocation(model_shader, "objectColor");
	light_pos_loc = glGetUniformLocation(model_shader, "lightPos");
	view_pos_loc = glGetUniformLocation(model_shader, "viewPos");

	model = glm::translate(model, Vec3(0.0f, 0.0f, -2.0f));
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMat()));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetProj()));

	Vec3 obj_color(0.6f, 1.0f, 0.3f);
	glUniform3fv(light_pos_loc, 1, &m_light->getPosition()[0]);
	glUniform3fv(light_loc, 1, &m_light->getColor()[0]);
	glUniform3fv(obj_loc, 1, &obj_color[0]);
	glUniform3fv(view_pos_loc, 1, &m_camera->getPosition()[0]);

	m_model->Draw(model_shader);
	m_light->Draw(m_camera->GetViewMat(), m_camera->GetProj());
	//slog("hi3");
	////if(wire_frame)
	////	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	////else
	////	glPolygonMode(GL_FRONT, GL_FILL);
	////m_voxelManager->RenderVoxels(shader);
	//
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
	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Insert:
			wire_frame = !wire_frame;
			break;
		default:
			break;
		}
	}
	m_camera->HandleInput(event);
}