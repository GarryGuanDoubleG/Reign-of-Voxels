#include "camera.hpp"
#include "simple_logger.h"

Camera::Camera()
{
	m_pos = Vec3(0.0, 0.0f, 200.0f);
	m_target = Vec3(0.0f, 0.0f, 0.0f);
	m_forward = Vec3(0.0f, 0.0f, -1.0f);//get forward direction
	m_up = Vec3(0.0f, 1.0f, 0.0f); //set default up vector to positive y
	m_right = glmNormalize(glmCross(m_forward, m_up));

	m_view_mat = glmLookAt(m_pos, m_target, m_up);//set view matrix

	sf::Vector2u win_size = Game::instance().getWindow()->getSize();
	m_proj_mat = glmPerspective(glm::radians(45.0f), (float)win_size.x / win_size.y, 0.1f, 1000.0f);

	m_pitch = 0.0f;
	m_yaw = -90.0f;
	m_roll = 0.0f;

	m_last_mouse_pos = sf::Mouse::getPosition(*Game::instance().getWindow());
}

Camera::~Camera()
{

}
Mat4 Camera::GetProj()
{
	return m_proj_mat;
}

Mat4 Camera::GetViewMat()
{
	return m_view_mat;
}

Vec3 Camera::getPosition()
{
	return m_pos;
}

void Camera::HandleInput(sf::Event event)
{
	sf::Time time = Game::g_delta_clock.getElapsedTime();
	GLfloat cam_speed = 200.0f * time.asSeconds();

	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::W:
			m_pos += cam_speed * m_forward;
			break;
		case sf::Keyboard::S:
			m_pos -= cam_speed * m_forward;
			break;
		case sf::Keyboard::A:
			m_pos -= m_right * cam_speed;
			break;
		case sf::Keyboard::D:
			m_pos += m_right * cam_speed;
			break;
		default:
			break;
		}
	}

	else if (event.type == sf::Event::MouseMoved)
	{
		GLfloat sensitivity = 0.05f; // mouse sensitivity
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(*Game::instance().getWindow());
		sf::Vector2i center(640, 360);
		if (mouse_pos == center)
			return;
		sf::Vector2i offset = mouse_pos - center;
		//sf::Vector2i offset = mouse_pos - m_last_mouse_pos;
		m_last_mouse_pos = mouse_pos;

		float xoffset = (float)offset.x * sensitivity;
		float yoffset = (float)offset.y * sensitivity;

		m_yaw += xoffset;
		m_pitch -= yoffset;

		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		else if (m_pitch < -89.0f)
			m_pitch = -89.0f;

		Vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

		m_forward = glmNormalize(front);
		m_right = glmNormalize(glmCross(m_forward, m_up));
	}

	m_view_mat = glmLookAt(m_pos, m_pos + m_forward, m_up);
}