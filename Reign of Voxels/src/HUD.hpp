#pragma once
#include <vector>
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "camera.hpp"
#include "3dmath.hpp"

typedef struct Vertex2D
{
	glm::vec3 position;
	glm::vec2 texCoord;
};

typedef struct HUDWidget
{
	std::string name;

	Rectf rect;

	GLuint textureID;
	GLuint vao,vbo, ebo;

	Vertex2D vertices[4];
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