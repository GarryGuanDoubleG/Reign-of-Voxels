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

	void setPosition(Vec3 pos);
	void draw();
private:
	Model *m_model;

	Vec3 m_pos;
	Vec3 m_color;

	GLuint m_shader;
	GLuint vao, vbo;
};