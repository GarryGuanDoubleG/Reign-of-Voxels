#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "camera.hpp"
#include "glm.hpp"

typedef struct HUDWidget
{
	Rectf rect;

	GLuint textureID;
	GLuint vao,
		vbo,
		ebo;

	//float *vertices;
	glm::vec3 vertices[4];
	glm::vec2 uv[4];

	GLuint indices[6];
};

class HUD
{
public:
	HUD();
	~HUD();
	void Init();
	
	void GenerateMesh();
	void BindWidgets();

	void Render();

	void HandleInput(sf::Event event);
private:
	std::vector<HUDWidget> m_widgets;
	
	Camera *m_camera;
};