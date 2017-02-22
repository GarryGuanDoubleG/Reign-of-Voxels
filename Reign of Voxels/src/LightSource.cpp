#include "LightSource.hpp"
#include "shader.hpp"
#include "simple_logger.h"

LightSource::LightSource()
{
	m_model = new Model("Resources\\models\\cube.obj");
	m_pos = Vec3(10.0f, 2.0f, 5.0f);
	m_color = Vec3(0.33f, 0.42f, 0.18f);

	if (!m_model)
	{
		slog("Failed to load light source model");
	}
}

void LightSource::draw()
{
	glUseProgram(g_shader_prog);
	m_model->Draw(m_shader);
}

void LightSource::setPosition(Vec3 position)
{
	m_pos = position;
}