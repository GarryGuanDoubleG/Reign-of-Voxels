#include "camera.hpp"
#include "game.hpp"
#include "simple_logger.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 forward)
{
	m_perspective = true;

	m_pos = position;
	m_target = target;
	m_forward = forward;//get forward direction
	m_up = glm::vec3(0.0f, 1.0f, 0.0f); //set default up vector to positive y
	m_right = glm::normalize(glm::cross(m_forward, m_up));

	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
	
	glm::vec2 screen_dimensions = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);

	//default perspective mat
	m_view_mat = glm::lookAt(m_pos, m_target, m_up);//set view matrix
	m_perspect_proj = glm::perspective(glm::radians(45.0f), screen_dimensions.x / screen_dimensions.y, 0.1f, 1000.0f);
	
	m_last_mouse_pos = sf::Mouse::getPosition(*Game::instance().getWindow());
}

Camera::~Camera()
{

}

void Camera::SwitchProjection()
{
	m_perspective = !m_perspective;
}

void Camera::SetToOrtho()
{
	m_perspective = false;
}

void Camera::SetToOrtho(glm::mat4 ortho)
{
	m_ortho_proj = ortho;
	m_perspective = false;
}

void Camera::SetToPersp()
{
	m_perspective = true;
}

glm::mat4 Camera::GetProj()
{
	return m_perspective ? m_perspect_proj : m_ortho_proj;
}

glm::mat4 Camera::GetViewMat()
{
	return m_view_mat;
}

glm::vec3 Camera::GetPosition()
{
	return m_pos;
}

void Camera::SetPosition(glm::vec3 position)
{
	m_pos = position;
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
		case sf::Keyboard::PageUp:
			SetToPersp();
			break;
		case sf::Keyboard::PageDown:
			SetToOrtho();
			break;
		default:
			break;
		}
	}
	else if (event.type == sf::Event::MouseWheelMoved)
	{
		m_zoom += event.mouseWheel.delta *time.asSeconds();

		m_ortho_proj = glm::ortho(-256.0f, 256.0f, -256.0f, 256.0f, 0.1f, 1000.0f);
		m_ortho_proj = glm::scale(m_ortho_proj, glm::vec3(m_zoom));
	}
	else if (event.type == sf::Event::MouseMoved)
	{
		GLfloat sensitivity = 0.05f; // mouse sensitivity
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(*Game::instance().getWindow());
		sf::Vector2i center(SCREEN_WIDTH / 2, SCREEN_HEIGHT /2);
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

		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

		m_forward = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_forward, m_up));
	}

	m_view_mat = glm::lookAt(m_pos, m_pos + m_forward, m_up);
}