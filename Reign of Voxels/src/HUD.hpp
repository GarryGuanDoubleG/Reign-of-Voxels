#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "camera.hpp"
#include "glm.hpp"

typedef struct HUDWidget
{
	sf::Texture* texture;
	Rectf rect;
};

class HUD
{
public:
	HUD();

	void Init();

	void DrawMiniMap();
	
	void GenerateMesh();
	void BindMesh();

	void Render();

	void HandleInput(sf::Event event);
private:
	std::vector<HUDWidget> m_widgets;

	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;

	GLuint m_vao,
		m_vbo,
		m_ebo;

	GLint m_indices_count;
};