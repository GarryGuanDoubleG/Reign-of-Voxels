#include <fstream>
#include <map>

#include "game.hpp"
#include "model.hpp"
#include "GameScene.hpp"
#include "simple_logger.h"


#define MODEL_PATH "Resources/models/models.json"


/**
* constructor
* Subscribes to event system and sets up a camera and loads models
*/
GameScene::GameScene()
{
	//load models, textures, fonts, etc.
	m_resrcMang = new ResourceManager();
	m_resrcMang->LoadResources();

	//hud handler
	m_hud = new HUD();

	//main player camera
	m_camera = new Camera(glm::vec3(256, 128, 256), glm::vec3(256, 0, 255));
	m_camera->SetToPersp();

	//Generate vertice and types for voxels
	m_voxelManager = new VoxelManager();
	m_voxelManager->GenerateVoxels();

	InitMinimap();

	//allocate memeory for all entities
	m_entity_list = new Entity[MAX_ENTITES];

	//set free list
	for (int i = 0; i < MAX_ENTITES - 1; i++)
		m_entity_list[i].m_nextFree = &m_entity_list[i + 1];

	//head of free list
	m_next_free_entity = m_entity_list;

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

void GameScene::InitMinimap()
{
	//minimap cam
	m_minimap_cam = new Camera(glm::vec3(256, 256, 512), glm::vec3(255, 0, 255));
	m_minimap_cam->SetToOrtho(glm::ortho(-256.0f, 256.0f, -256.0f, 256.0f, 0.1f, 1000.0f));

	//minimap resizing^
	glm::vec2 minimap_size = glm::vec2(512.0f, 512.0f);

	//put minimap on the bottom left
	//m_minimap_pos = glm::vec2(0.0f, SCREEN_HEIGHT - (minimap_size.y / 2.0f));
	m_minimap_pos = glm::vec2(-5.0f, -256.0f);
	m_minimap_scale = glm::vec2(minimap_size.x / (float)SCREEN_WIDTH, minimap_size.y / (float)SCREEN_HEIGHT);
}

/**
*@brief code to run every frame of game loop
*/
void GameScene::SceneFrame()
{
	Update();
	Render();
}

void GameScene::Update()
{
	for (int i = 0; i < MAX_ENTITES; i++)
	{
		if (m_entity_list[i].IsActive())
			m_entity_list[i].Update();
	}
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
	RenderEntities();
	m_hud->Render();


	Game::instance().getWindow()->display();	
}

void GameScene::RenderModel(Entity *entity)
{
	GLuint shader = GetShader("model");

	glUseProgram(shader);

	glm::mat4 model;
	model = glm::translate(glm::mat4(1.0f), entity->GetPosition());
	model = glm::scale(model, glm::vec3(64.0f, 64.0f, 64.0f));

	glm::vec3 light_pos(256, 512.0f, 256);
	glm::vec3 light_color(1.0f, 1.0f, 1.0f);

	//mvp matrices
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &m_camera->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &m_camera->GetProj()[0][0]);

	//lighting
	glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, &m_camera->GetPosition()[0]);
	glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, &light_pos[0]);
	glUniform3fv(glGetUniformLocation(shader, "lightColor"), 1, &light_color[0]);

	//color of entity

	glm::vec3 ent_color;
	if (entity->IsSelected())
		ent_color = glm::vec3(1.0f, .2f, .2f);
	else
		ent_color = glm::vec3(.5f, 1.0f, .5f);

	glUniform3fv(glGetUniformLocation(shader, "model_color"), 1, &ent_color[0]);

	m_resrcMang->GetModel(entity->GetModelID())->Draw(shader);	 
}

void GameScene::RenderWorld()
{
	if (wire_frame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_voxelManager->RenderVoxels(m_camera);
}

void GameScene::RenderEntities()
{
	for (int i = 0; i < MAX_ENTITES; i++)
	{
		if (m_entity_list[i].IsActive())
			RenderModel(&m_entity_list[i]);
	}
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
		case sf::Keyboard::R:
			CreateEntity();
		default:
			break;
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(*Game::instance().getWindow());
		glm::vec3 world_pos = m_camera->MouseToWorldSpace(mouse_pos);
		
		std::cout << "world pos x " << world_pos.x << std::endl;
		std::cout << "world pos y " << world_pos.y << std::endl;
		std::cout << "world pos z " << world_pos.z << std::endl;
	}
	m_camera->HandleInput(event);
}


void GameScene::CreateEntity()
{
	Entity *entity = m_next_free_entity;

	m_next_free_entity = entity->m_nextFree;

	int id = entity - m_entity_list;

	//TODO move this to a json file
	BBox bounds = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(64,64,64) };

	entity->Init(m_resrcMang->GetModelID("worker"), glm::vec3(64 * id,64, 64 * id), bounds);
	entity->m_nextFree = NULL;
}
