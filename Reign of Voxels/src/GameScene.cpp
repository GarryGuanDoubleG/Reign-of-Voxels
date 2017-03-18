#include <fstream>
#include <map>

#include "GameScene.hpp"
#include "simple_logger.h"

std::map<std::string, GLint> g_model_keys;
static std::vector<Model *> g_models;

#define MODEL_PATH "Resources/models/models.json"

/**
* constructor
* Subscribes to event system and sets up a camera and loads models
*/
GameScene::GameScene()
{
	m_hud = new HUD();

	//main player camera
	m_camera = new Camera(glm::vec3(256, 256, 256), glm::vec3(256, 0, 255), glm::vec3(0, -1.0f, 0));
	
	//minimap cam
	m_minimap_cam = new Camera(glm::vec3(256, 256, 512), glm::vec3(255, 0, 255), glm::vec3(0, -1.0f, 0));
	m_minimap_cam->SetToOrtho(glm::ortho(-256.0f, 256.0f, -256.0f, 256.0f, 0.1f, 1000.0f));

	//minimap resizing
	glm::vec2 minimap_size = glm::vec2(512.0f, 512.0f);

	//put minimap on the bottom left
	m_minimap_pos = glm::vec2(0.0f, SCREEN_HEIGHT - (minimap_size.y / 2.0f));
	m_minimap_scale = glm::vec2(minimap_size.x / (float)SCREEN_WIDTH, minimap_size.y / (float)SCREEN_HEIGHT);
	
	//Generate vertice and types for voxels
	m_voxelManager = new VoxelManager();
	m_voxelManager->GenerateVoxels();

	LoadModels();

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

	m_hud->Render();

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

	m_voxelManager->RenderMinimap(shader, m_minimap_scale, m_minimap_pos);
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

void GameScene::LoadModels()
{
	Json data;

	std::ifstream in(MODEL_PATH);
	in >> data;

	int i = 0;

	//goes thr ough every object in model.json and loads into globals
	for (Json::iterator it = data.begin(); it != data.end(); ++it)
	{
		Json model = *it;

		std::string path = model["path"];		

		g_models.push_back(new Model(path));
		g_model_keys.insert(std::pair<std::string, int>(it.key(), i));

		++i;
	}
}

GLint GameScene::GetModelID(std::string name)
{
	return g_model_keys[name];
}
