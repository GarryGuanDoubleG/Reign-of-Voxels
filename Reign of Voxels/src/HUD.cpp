#include <fstream>
#include "game.hpp"
#include "HUD.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include "Gui/layout.hpp"
#include "simple_logger.h"

#define HUD_PATH "Resources/gui/HUD.json"

HUD::HUD()
{
	
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f), 
						  glm::vec3(0.0f, 0.0f, 0.0f), 
						  glm::vec3(0.0f, 1.0f, 0.0));
	
	m_camera->SetToOrtho(glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f));

	Init();
}

HUD::~HUD()
{
	for (int i = 0; i < m_widgets.size(); i++)
	{
		glDeleteVertexArrays(1, &m_widgets[i].vao);
		glDeleteBuffers(1, &m_widgets[i].vbo);
		glDeleteBuffers(1, &m_widgets[i].ebo);
	}
}

void LoadHudWidget(Json &data, HUDWidget &widget)
{
	std::string texture_path = data["texture"];
	widget.textureID = LoadTexture(texture_path.c_str());

	widget.rect.size.x = (float)data["width"];
	widget.rect.size.y = (float)data["height"];

	if (data["position"] == "bottom_right")
	{
		widget.rect.position.x = (float)SCREEN_WIDTH - widget.rect.size.x;
		widget.rect.position.y = (float)SCREEN_HEIGHT  - widget.rect.size.y;
	}
	else if (data["position"] == "bottom_center")
	{
		widget.rect.position.x = (float)SCREEN_WIDTH * .5f -  widget.rect.size.x *.5f;
		widget.rect.position.y = (float)SCREEN_HEIGHT - widget.rect.size.y;
	}

}

//load textures and fonts
void HUD::Init()
{
	Json layout;
	std::ifstream in(HUD_PATH);

	if(in.is_open())
		in >> layout;
	else 
		slog("Failed to open HUD %s", HUD_PATH);

	//3 by 3 grid with options from selected unit
	if (layout.find("action_grid") != layout.end())
	{
		Json data = layout["action_grid"];
		HUDWidget widget;

		LoadHudWidget(data, widget);
		m_widgets.push_back(widget);
	}

	if (layout.find("unit_display") != layout.end())
	{
		Json data = layout["unit_display"];
		HUDWidget widget;

		LoadHudWidget(data, widget);
		m_widgets.push_back(widget);
	}

	GenerateMesh();
	BindWidgets();
}

void HUD::GenerateMesh()
{
	//vertice positions for hud quad
	for (int i = 0; i < m_widgets.size(); i++)
	{
		//top left vertice
		m_widgets[i].vertices[0] = glm::vec3(m_widgets[i].rect.position.x, 
											 m_widgets[i].rect.position.y, 0.5f);
		m_widgets[i].uv[0] = glm::vec2(0.0f, 0.0f);

		//top right
		m_widgets[i].vertices[1] = glm::vec3(m_widgets[i].rect.position.x + m_widgets[i].rect.size.x,
											 m_widgets[i].rect.position.y, 0.5f);
		m_widgets[i].uv[1] = glm::vec2(1.0f, 0.0f);

		//bottom left
		m_widgets[i].vertices[2] = glm::vec3(m_widgets[i].rect.position.x, 
											 m_widgets[i].rect.position.y + m_widgets[i].rect.size.y, 0.5f);
		m_widgets[i].uv[2] = glm::vec2(0.0f, 1.0f);

		//bottom right
		m_widgets[i].vertices[3] = glm::vec3(m_widgets[i].rect.position.x + m_widgets[i].rect.size.x,
											 m_widgets[i].rect.position.y + m_widgets[i].rect.size.y, 0.5f);
		m_widgets[i].uv[3] = glm::vec2(1.0f, 1.0f);

		//top left triangle
		m_widgets[i].indices[0] = 0;
		m_widgets[i].indices[1] = 2;
		m_widgets[i].indices[2] = 1;

		//bottom right triangle
		m_widgets[i].indices[3] = 1;
		m_widgets[i].indices[4] = 2;
		m_widgets[i].indices[5] = 3;
	}
}

void HUD::BindWidgets()
{
	for (int i = 0; i < m_widgets.size(); i++)
	{
		//gen and bind vao
		glGenVertexArrays(1, &m_widgets[i].vao);
		glBindVertexArray(m_widgets[i].vao);

		//gen and bind vertex buffer
		glGenBuffers(1, &m_widgets[i].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_widgets[i].vbo);
		//4 vertex and uv positions
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3) + 4 * sizeof(glm::vec2), &m_widgets[i].vertices[0], GL_STATIC_DRAW);

		//gen and bind indices
		glGenBuffers(1, &m_widgets[i].ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_widgets[i].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_widgets[i].indices), &m_widgets[i].indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);		
	}
}

void HUD::Render()
{
	GLuint shader = GetShader("hud");
	GLuint model_location = glGetUniformLocation(shader, "model");
	
	glUseProgram(shader);

	glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &m_camera->GetViewMat()[0][0]);


	for (int i = 0; i < m_widgets.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(m_widgets[i].rect.position, 0.0f));

		glBindTexture(GL_TEXTURE_2D, m_widgets[i].textureID);

		glBindVertexArray(m_widgets[i].vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	
}

void HUD::HandleInput(sf::Event event)
{

}