#pragma once
#include <SFML\Graphics.hpp>
#include <GL/glew.h>
#include <model.hpp>
#include "glm.hpp"

class LightSource
{
public:
	LightSource();
	~LightSource();

	void setPosition(glm::vec3 pos);
	void setColor(glm::vec3 color);
	glm::vec3 getPosition();
	glm::vec3 getColor();

	void Draw(glm::mat4 view, glm::mat4 proj);

	Model *m_model;
private:

	glm::vec3 m_pos;
	glm::vec3 m_color;
};