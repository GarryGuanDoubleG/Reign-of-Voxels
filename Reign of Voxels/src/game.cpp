#include "game.h"
#include "glm.h"
#include "graphics.h"
#include "Gui\Menu.h"
#include "client.h"
#include "shader.h"
#include "model.h"
#include "camera.h"


//doing this just to test menu out. should refactor soon
//sf::Font main_font; 

//load fonts.
//should probably make a font utility class
//and load everything from start up using cjson for file locations

sf::Clock g_delta_clock;
sf::Clock g_clock;

void Initialize()
{
	//initialize funcitons
	GraphicsInit();
	compile_shaders(); 
}

void RenderScene(Model *model, Mat4 &mvp)
{
	GLfloat bg_color[] = { 0.0f, 0.0f, 0.5f, 0.0f };
	GLuint mvp_location;

	mvp_location = glGetUniformLocation(g_shader_prog, "mvp");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glUseProgram(g_shader_prog);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

	model->Draw(g_shader_prog);

	g_window->display();
}

void GameLoop()
{
	Model *model = new Model("Resources\\models\\nanosuit\\nanosuit.obj");
	Camera *camera = new Camera();
	Mat4 model_mat4 = Mat4(1.0f);

	model_mat4 = glm::translate(model_mat4, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	model_mat4 = glm::scale(model_mat4, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down

	while (g_window->isOpen())
	{
		Mat4 mvp = camera->GetProj() * camera->GetCamView() * model_mat4;
		RenderScene(model, mvp);
		
		sf::Event event;
		while (g_window->pollEvent(event))
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				//GraphicsClose();
				break;
			}
			camera->HandleInput(event);
		}
		g_delta_clock.restart();
	}
}