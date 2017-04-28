#include <fstream>
#include <map>

#include "game.hpp"
#include "model.hpp"
#include "GameScene.hpp"
#include "PhysicsUtil.hpp"
#include "simple_logger.h"

#define MODEL_PATH "Resources/models/models.json"

#define RAY_CAST_MODE 1
#define AABB_MODE 2
#define RENDER_GROUND_MODE 4
#define RENDER_SKYBOX_MODE 8
/**
* constructor
* Subscribes to event system and sets up a camera and loads models
*/
GameScene::GameScene()
{
	m_flags |= RENDER_GROUND_MODE;

	//hud handler
	m_hud = new HUD();

	//main player camera
	m_camera = new Camera(glm::vec3(256, 64, 256), glm::vec3(256, 0, 255));
	m_camera->SetToPersp();

	//Generate vertice and types for voxels
	m_voxelManager = new VoxelManager(GetWorldResolution());
	m_voxelManager->GenerateVoxels();

	InitMinimap();
	InitRayVertex();
	InitSkybox();

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

void GameScene::InitRayVertex()
{
	float verts[] = {
		1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		.0000001f,.0000001f,.0000001f
	};

	glGenVertexArrays(1, &m_raycast_vao);
	glBindVertexArray(m_raycast_vao);

	glGenBuffers(1, &m_raycast_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_raycast_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GameScene::InitMinimap()
{
	GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
								 // Positions   // TexCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	m_minimapCam = new Camera(glm::vec3(128, 256, 128), glm::vec3(127.5, 0, 127.5));
	m_minimapCam->SetToOrtho(glm::ortho(128.0f, -128.0f, 128.0f, -128.0f, 0.1f, 1000.0f));

	m_minimapScale = glm::vec2(256.0f / (float) Game::screen_width, 256.0f / (float) Game::screen_height);
	//bind quad vertices
	glGenVertexArrays(1, &m_minimapVAO);
	glBindVertexArray(m_minimapVAO);

	glGenBuffers(1, &m_minimapVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_minimapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//Frame Buffer
	glGenFramebuffers(1, &m_minimapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_minimapFBO);

	glGenTextures(1, &m_minimapTexture);
	glBindTexture(GL_TEXTURE_2D, m_minimapTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Game::screen_width, Game::screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_minimapTexture, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameScene::InitSkybox()
{
	GLfloat cubeVertices[] = {
		// Positions          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &m_cubemapVAO);
	glBindVertexArray(m_cubemapVAO);

	glGenBuffers(1, &m_cubemapVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubemapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_skyboxVAO);
	glBindVertexArray(m_skyboxVAO);

	glGenBuffers(1, &m_skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	m_camera->Update();
}


/**
*@brief Handles drawing the game scene
*/
void GameScene::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat bg_color[] = { 0.3f, 0.3f, 0.3f, 0.3f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);

	sf::Clock timer;

	RenderWorld();
	RenderEntities();

	if(m_flags & RAY_CAST_MODE)
		RenderRayCast();

	if (m_flags & RENDER_SKYBOX_MODE)
	{
		RenderSkybox();
	}

	RenderMinimap();

	m_hud->Render();

	Game::instance().getWindow()->display();

	//std::cout << "FPS " << timer.getElapsedTime().asSeconds() << std::endl;
}

void GameScene::RenderAABB(Entity *entity, GLuint shader)
{
	shader = GetShader("object");
	glUseProgram(shader);

	glBindVertexArray(m_cubemapVAO);

	GLuint modelID = GetModelID("cube");

	glm::mat4 model;
	model = glm::translate(glm::mat4(1.0f), entity->GetPosition() + entity->GetAABB().min);
	model = glm::scale(model, entity->GetAABB().max - entity->GetAABB().min);

	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);

	glm::vec3 light_pos(256, 512.0f, 256);
	glm::vec3 light_color(1.0f, 1.0f, 1.0f);

	//mvp matrices
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &m_camera->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &m_camera->GetProj()[0][0]);

	//lighting
	//glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, &m_camera->GetPosition()[0]);
	//glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, &light_pos[0]);
	//glUniform3fv(glGetUniformLocation(shader, "lightColor"), 1, &light_color[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GetTextureID("skybox1"));
	glUniform1i(glGetUniformLocation(shader, "texture_diffuse0"), 0);

	//Game::instance().m_resourceManager->GetModel(modelID)->Draw(shader);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void GameScene::RenderModel(Entity *entity)
{
	Model *ent_model = GetModel(entity->GetModelID());
	
	GLuint shader;

	if (ent_model->IsRigged())
		shader = GetShader("model");
	else
		shader = GetShader("object");

	glUseProgram(shader);

	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
	model = glm::translate(glm::mat4(1.0f), entity->GetPosition());
	//model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));

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
		ent_color = glm::vec3(1.0f, .5f, .5f);
	else
		ent_color = glm::vec3(1.0f);

	int location = glGetUniformLocation(shader, "colorMod");

	glUniform3fv(glGetUniformLocation(shader, "colorMod"), 1, &ent_color[0]);

	if (ent_model->IsRigged())
		ent_model->Draw(shader, Game::instance().clock.getElapsedTime().asSeconds());
	else
		ent_model->Draw(shader);

	if(m_flags & AABB_MODE)
		RenderAABB(entity, shader);
}
void GameScene::RenderSkybox()
{	
	GLuint shader = GetShader("skybox");

	glDepthFunc(GL_LEQUAL);
	glUseProgram(shader);

	// ... Set view and projection matrix
	glm::mat4 model = glm::translate(glm::mat4(1.0f), m_camera->GetPosition());
	glm::mat4 view = glm::mat4(glm::mat3(m_camera->GetViewMat()));

	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &m_camera->GetProj()[0][0]);

	glBindVertexArray(m_skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, GetTextureID("skybox"));
	glUniform1i(glGetUniformLocation(shader, "skybox"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}
void GameScene::RenderWorld()
{
	if (wire_frame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_flags & RENDER_GROUND_MODE)
	{
		m_voxelManager->RenderWorld(draw_textured, m_camera);
	}
	else
	{
		m_voxelManager->RenderGrass(m_camera);
	}
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
	glBindFramebuffer(GL_FRAMEBUFFER, m_minimapFBO);
	GLfloat bg_color[] = { 0.3f, 0.3f, 0.3f, 0.3f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);


	m_voxelManager->RenderWorld(draw_textured, m_minimapCam);


	//rebind original
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint shader = GetShader("quad"); // draw quad
	glUseProgram(shader);

	glBindVertexArray(m_minimapVAO);

	glUniform2fv(glGetUniformLocation(shader, "scale"), 1, &m_minimapScale[0]);

	;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_minimapTexture);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}

void GameScene::RenderRayCast()
{
	GLuint shader = GetShader("ray");
	glUseProgram(shader);

	glBindVertexArray(m_raycast_vao);

	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &m_camera->GetViewMat()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &m_camera->GetProj()[0][0]);

	for (int i = 0; i < m_rays.size(); i++)
	{
		glm::mat4 model(1.0f);	
		model = glm::translate(model, m_rays[i].start);
		model = glm::scale(model, 1000.0f * (m_rays[i].dir));

		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);


		glDrawArrays(GL_LINES, 0, 2);

		CheckGLError();
	}
	glBindVertexArray(0);
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
	//m_minimapCam->HandleInput(event);

	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Insert:
			wire_frame = !wire_frame;
			break;
		case sf::Keyboard::R:
			CreateEntity();
			break;
		case sf::Keyboard::T:
			m_rays.clear();
			break;
		case sf::Keyboard::Y:
			m_flags ^= RAY_CAST_MODE;
			break;
		case sf::Keyboard::U:
			m_flags ^= AABB_MODE;
			break;
		case sf::Keyboard::H:
			m_flags ^= RENDER_GROUND_MODE;
			break;
		case sf::Keyboard::G:
			m_flags ^= RENDER_SKYBOX_MODE;
			break;
		case sf::Keyboard::Num1:
			draw_textured = false;
			break;
		case sf::Keyboard::Num2:
			draw_textured = true;
			break;
		default:
			break;
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		Ray ray;
		sf::Vector2i mouse_pos(event.mouseButton.x, event.mouseButton.y);

		//TODO thread ray intersection
		 PhysicsUtil::ScreenPosToWorldRay(m_camera->GetPosition(), mouse_pos,
								glm::vec2((float)Game::screen_width, (float)Game::screen_height), m_camera->GetViewMat(),
								m_camera->GetProj(), ray);

		 m_rays.push_back(ray);

		if (event.mouseButton.button == sf::Mouse::Left)
		{

			for (int i = 0; i < MAX_ENTITES; i++)
			{
				if (!m_entity_list[i].IsActive())
					continue;

				glm::vec3 intersection;
				float t;

				bool hit = AABBRayIntersection(m_entity_list[i].GetPosition(), m_entity_list[i].GetAABB(), ray);

				m_entity_list[i].SetSelected(hit);	
			}
		}
		else // right click
		{
			glm::vec3 hit;

			PhysicsUtil::WorldRayCast(m_voxelManager, ray, 1000.0f, hit);
			
			if (hit.y < 0)
			{
				slog("No hit");
				return; //no hit if underneath world
			}
			else
			{
				std::cout << "Hit.x " << hit.x << std::endl;
				std::cout << "Hit.y " << hit.y << std::endl;
				std::cout << "Hit.z " << hit.z << std::endl;

			}

			for (int i = 0; i < MAX_ENTITES; i++)
			{
				if (!m_entity_list[i].IsActive() || 
					!m_entity_list[i].IsSelected())
					continue;

				m_entity_list[i].MoveTo(hit);				
			}
		}
	}
}

void GameScene::CreateEntity()
{
	Entity *entity = m_next_free_entity;

	m_next_free_entity = entity->m_nextFree;

	int id = entity - m_entity_list;

	int size = 1;
	//TODO move this to a json file
	AABB bounds = { glm::vec3(-4.0f, 0.0f, -4.0f), glm::vec3(3.0f, 12.5f, 3.0f) };

	entity->Init(GetModelID("worker"), glm::vec3(size*id, 64, size*id), bounds);

	entity->m_nextFree = NULL;
}