#include "GameScene.hpp"
#include "simple_logger.h"
/**
* constructor
* Subscribes to event system and sets up a camera and loads models
*/#
GameScene::GameScene()
{
	std::cout << "Init Game \n";

	m_camera = new Camera(Vec3(256, 256, 256), Vec3(256, 0, 255), Vec3(0, -1.0f, 0));

	m_minimap_cam = new Camera(Vec3(256, 256, 256), Vec3(256,0,255), Vec3(0, -1.0f, 0));
	m_minimap_cam->SetToOrtho();


	Vec2 minimap_size = Vec2(512.0f, 512.0f);
	std::cout << "minimap size " << minimap_size.x << " \t\t\t\t" << minimap_size. y<< std::endl;
	//m_minimap_pos = Vec2((float)SCREEN_WIDTH - minimap_size.x, (float)SCREEN_HEIGHT - minimap_size.y);
	m_minimap_pos = Vec2(1408, 0);
	m_minimap_scale = Vec2(minimap_size.x / (float)SCREEN_WIDTH, minimap_size.y / (float)SCREEN_HEIGHT);
	
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
	
	RenderWorld();
	RenderMinimap();

	Game::instance().getWindow()->display();	
}

void GameScene::RenderWorld()
{
	if (wire_frame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_voxelManager->RenderVoxels(m_camera);
}

void GameScene::RenderMinimap()
{
	GLuint shader = GetShader("minimap");

	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &m_minimap_cam->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &m_minimap_cam->GetProj()[0][0]);
	
	glUniform2fv(glGetUniformLocation(shader, "position"), 1, &m_minimap_pos[0]);
	glUniform2fv(glGetUniformLocation(shader, "scale"), 1, &m_minimap_scale[0]);

	//glEnable(GL_CULL_FACE);

	m_voxelManager->RenderMinimap(shader, m_minimap_scale, m_minimap_pos);

	//glDisable(GL_CULL_FACE);
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