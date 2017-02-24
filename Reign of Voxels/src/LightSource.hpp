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
	void setColor(Vec3 color);
	Vec3 getPosition();
	Vec3 getColor();

	void Draw(Mat4 view, Mat4 proj);

	Model *m_model;
private:

	Vec3 m_pos;
	Vec3 m_color;
};