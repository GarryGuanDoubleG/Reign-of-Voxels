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
	m_model = new Model("Resources\\models\\sphere.obj");
	
	
	m_light = new LightSource(); 
	m_light->m_model = m_model;// use the same model for the lighitng for now


	m_voxelManager = new VoxelManager();
	m_voxelManager->GenerateVoxels();


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
	GLfloat bg_color[] = { 0.3f, 0.3f, 0.3f, 0.3f };

 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glClearBufferfv(GL_COLOR, 0, bg_color);

	m_light->Draw(m_camera->GetViewMat(), m_camera->GetProj());

	if(wire_frame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_voxelManager->RenderVoxels(m_camera);
	
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