#include "GameScene.hpp"
/**
* constructor
* Subscribes to event system and sets up a camera and loads models
*/
GameScene::GameScene()
{
	m_camera = new Camera();	

	m_voxelManager = new VoxelManager();
	//subscribe to global events
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
	GLuint model_loc, view_loc, proj_loc;
	GLfloat bg_color[] = { 0.1f, 0.1f, 0.1f, 0.2f };

	Mat4 model_mat4 = Mat4(1.0f);
	model_mat4 = glm::scale(model_mat4, glm::vec3(0.05f, 0.05f, 0.05f));	// It's a bit too big for our scene, so scale it down

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glUseProgram(g_shader_prog);

	view_loc = glGetUniformLocation(g_shader_prog, "view");
	proj_loc = glGetUniformLocation(g_shader_prog, "projection");

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMat()));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(m_camera->GetProj()));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_voxelManager->RenderVoxels();
	//PolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Game::instance().getWindow()->display();
}
/**
*@brief Listens to user input events and handles it
*@param event type of user input event
*@param input the input from client or server
*/
void GameScene::onNotify(Event event, sf::Event &input)
{
	if (event == ServerInput)
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