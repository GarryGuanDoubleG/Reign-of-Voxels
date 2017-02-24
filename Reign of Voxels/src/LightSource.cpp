#include "LightSource.hpp"
#include "shader.hpp"
#include "simple_logger.h"

LightSource::LightSource()
{
	m_model = new Model("Resources\\models\\sphere.obj");
	m_pos = Vec3(0.0f, 0.0f, 0.0f);
	m_color = Vec3(1.0f, 1.0f, 1.0f);

	if (!m_model)
	{
		slog("Failed to load light source model");
	}
}

void LightSource::Draw(Mat4 view, Mat4 proj)
{
	Mat4 model(1.0f);
	GLuint model_loc, view_loc, proj_loc;
	GLuint shader = GetShader("lighting");
	
	glUseProgram(shader);

	model_loc = glGetUniformLocation(shader, "model");
	view_loc = glGetUniformLocation(shader, "view");
	proj_loc = glGetUniformLocation(shader, "projection");

	model = glm::translate(model, m_pos);
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

	m_model->Draw(shader);
}

void LightSource::setPosition(Vec3 position)
{
	m_pos = position;
}

void LightSource::setColor(Vec3 color)
{
	m_color = color;
}
Vec3 LightSource::getPosition()
{
	return m_pos;
}
Vec3 LightSource::getColor()
{
	return m_color;
}