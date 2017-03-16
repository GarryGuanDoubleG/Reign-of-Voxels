#include <fstream>
#include "game.hpp"
#include "HUD.hpp"
#include "shader.hpp"

#include "Gui/layout.hpp"
#include "simple_logger.h"

#define HUD_PATH "Resources/gui/HUD.json"

HUD::HUD()
{
	Init();
}

void LoadHudWidget(Json &data, HUDWidget &widget)
{
	sf::Texture *texture = new sf::Texture();
	texture->loadFromFile(data["texture"]);

	widget.rect.size.x = data["width"];
	widget.rect.size.y = data["height"];

	//window space coordinates
	if (data["position"] == "bottom_right")
	{
		widget.rect.position.x = SCREEN_WIDTH - widget.rect.size.x;
		widget.rect.position.y = SCREEN_HEIGHT - widget.rect.size.y;
	}
	else if (data["position"] == "bottom_center")
	{
		widget.rect.position.x = SCREEN_WIDTH * .5f - widget.rect.size.x *.5f;
		widget.rect.position.y = SCREEN_HEIGHT - widget.rect.size.y;
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
	BindMesh();
}

void HUD::GenerateMesh()
{
	for (int i = 0; i < m_widgets.size(); i++)
	{
		Vertex vertex;
		vertex.normal = Vec3(1.0f);		

		//convert to normalized coordinates

		//top left vertice
		vertex.position.x = m_widgets[i].rect.position.x / (float)SCREEN_WIDTH;
		vertex.position.y = m_widgets[i].rect.position.y / (float)SCREEN_HEIGHT;
		vertex.position.z = 1.0f;
		vertex.uv = Vec2(0.0f);

		m_vertices.push_back(vertex);

		//top right
		vertex.position.x = m_widgets[i].rect.position.x + m_widgets[i].rect.size.x / (float)SCREEN_WIDTH;
		vertex.position.y = m_widgets[i].rect.position.y - (.5f * SCREEN_HEIGHT) / (float)SCREEN_HEIGHT;
		vertex.uv = Vec2(1.0f, 0.0f);
		m_vertices.push_back(vertex);

		//bottom left vertice
		vertex.position.x = m_widgets[i].rect.position.x / (float)SCREEN_WIDTH;
		vertex.position.y = m_widgets[i].rect.position.y + m_widgets[i].rect.size.y / (float)SCREEN_HEIGHT;
		vertex.uv = Vec2(0.0f, 1.0f);

		m_vertices.push_back(vertex);
		
		//bottom right
		vertex.position.x = m_widgets[i].rect.position.x + m_widgets[i].rect.size.x / (float)SCREEN_WIDTH;
		vertex.position.y = m_widgets[i].rect.position.y + m_widgets[i].rect.size.y / (float)SCREEN_HEIGHT;
		vertex.uv = Vec2(1.0f, 1.0f);

		m_vertices.push_back(vertex);

		m_indices.push_back(m_vertices.size() - 4);
		m_indices.push_back(m_vertices.size() - 3);
		m_indices.push_back(m_vertices.size() - 2);

		m_indices.push_back(m_vertices.size() - 4);
		m_indices.push_back(m_vertices.size() - 2);
		m_indices.push_back(m_vertices.size() - 1);
	}
}

void HUD::BindMesh()
{
	glGenBuffers(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
	
	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_vertices[0], GL_STATIC_DRAW);

	//location 0 should be verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	//now textures
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_indices_count = m_indices.size();
}

void HUD::Render()
{
	GLuint shader = GetShader("HUD");
	glUseProgram(shader);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void HUD::HandleInput(sf::Event event)
{

}