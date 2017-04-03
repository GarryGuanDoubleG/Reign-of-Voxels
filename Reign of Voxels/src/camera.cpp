#include "camera.hpp"
#include "game.hpp"
#include "simple_logger.h"

Camera::Camera(glm::vec3 position, glm::vec3 target)
{
	m_perspective = true;

	m_pos = position;

	//camera orientation vecs
	m_up = glm::vec3(0.0f, 1.0f, 0.0f); //set default up vector to positive y
	m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	m_forward = glm::normalize(target - position);

	//default perspective mat
	m_view_mat = glm::lookAt(m_pos, target, m_up);

	//set up perspective mat4
	m_fov = glm::radians(45.0f);
	m_aspect_ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	m_nearD = 1.0f;
	m_farD = 1000.0f;

	m_perspect_proj = glm::perspective(m_fov, m_aspect_ratio, m_nearD, m_farD);

	//compute near far plane dimensions for frustum culling
	GLfloat tan_angle = tan(glm::radians(m_fov * .5f));
	
	m_nearH = m_nearD * tan_angle;
	m_nearW = m_nearH * m_aspect_ratio;

	m_farH = m_farD * tan_angle;
	m_farW = m_farH * m_aspect_ratio;

	Update();
}

Camera::~Camera()
{

}

void Camera::SwitchProjection()
{
	m_perspective = !m_perspective;
}

void Camera::SetToOrtho(glm::mat4 ortho)
{
	m_ortho_proj = ortho;
	m_perspective = false;
}

void Camera::SetToPersp()
{
	m_pitch = -60.0f;
	m_yaw = -89.0f;
	m_roll = 0.0f;

	m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_forward.y = sin(glm::radians(m_pitch));
	m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	m_forward = glm::normalize(m_forward);

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

void Camera::Update()
{
	//based on lighthouse3d tutorials
	glm::vec3 dir, nearCenter, farCenter,
			X_axis, Y_axis, Z_axis;
	
	//4 corners of near plane
	glm::vec3 nearTL, nearTR, nearBL, nearBR;
	
	//4 corners of far plane
	glm::vec3 farTL, farTR, farBL, farBR;

	//opposite direction of looking axis
	Z_axis = -m_forward;
	X_axis = m_right;
	Y_axis = m_up;

	//compute center of near and far planes
	nearCenter = m_pos - Z_axis * m_nearD;
	farCenter = m_pos - Z_axis * m_farD;

	//compute 4 corners of near plane
	nearTL = nearCenter + Y_axis * m_nearH - X_axis * m_nearW;
	nearTR = nearCenter + Y_axis * m_nearH + X_axis * m_nearW;
	nearBL = nearCenter - Y_axis * m_nearH - X_axis * m_nearW;
	nearBR = nearCenter - Y_axis * m_nearH + X_axis * m_nearW;

	//corners of far plane
	farTL = farCenter + Y_axis * m_farH - X_axis * m_farW;
	farTR = farCenter + Y_axis * m_farH + X_axis * m_farW;
	farBL = farCenter - Y_axis * m_farH - X_axis * m_farW;
	farBR = farCenter - Y_axis * m_farH + X_axis * m_farW;

	SetPlane(m_planes[TOP], nearTR, nearTL, farTL);
	SetPlane(m_planes[BOTTOM], nearBL, nearBR, farBR);
	SetPlane(m_planes[LEFT], nearTL, nearBL, farBL);
	SetPlane(m_planes[RIGHT], nearBR, nearTR, farBR);
	SetPlane(m_planes[NEARP], nearTL, nearTR, nearBR);
	SetPlane(m_planes[FARP], farTR, farTL, farBL);
}

//checks if object is within frustum
bool Camera::PointInCamera(glm::vec3 &point)
{
	for (int i = 0; i < 6; i++)
	{
		if (DistanceToPlane(m_planes[i], point) < 0)
			return false;
	}

	return true;
}
bool Camera::SphereInCamera(glm::vec3 &point, float radius)
{
	for (int i = 0; i < 6; i++)
	{		
		if (DistanceToPlane(m_planes[i], point) < -radius)
			return false;
	}

	return true;
}

bool Camera::AABBInCamera(BBox &aabb)
{
	for (int i = 0; i < 6; i++)
	{
		bool inside = false,
			outside = false;

		for (int x = 0; x <= 1; x++)
		{
			for (int y = 0; y <= 1; y++)
			{
				for (int z = 0; z <= 1; z++)
				{
					glm::vec3 point = aabb.position +
						glm::vec3(aabb.size.x * x, aabb.size.y * y, aabb.size.z * z);
					if (DistanceToPlane(m_planes[i], point) > 0)
						inside = true;
				}
			}
		}

		if (!inside)
			return false;
	}

	return true;
}

bool Camera::AABBInCamera(CubeRegion &aabb)
{
	for (int i = 0; i < 6; i++)
	{
		bool inside = false,
			 outside = false;

		for (int x = 0; x <= 1; x++)
		{
			for (int y = 0; y <= 1; y++)
			{
				for (int z = 0; z <= 1; z++)	
				{
					glm::vec3 point = aabb.position + glm::vec3(aabb.size * x, aabb.size * y, aabb.size * z);
					if (DistanceToPlane(m_planes[i], point) > -128)
						inside = true;
				}
			}
		}

		if (!inside)
			return false;
	}

	return true;
}

void Camera::HandleInput(sf::Event event)
{
	sf::Time time = Game::g_delta_clock.getElapsedTime();
	GLfloat cam_speed = 3.0f;

	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::W:
			m_pos.z -= cam_speed;
			break;
		case sf::Keyboard::S:
			m_pos.z += cam_speed;
			break;
		case sf::Keyboard::A:
			m_pos.x -= cam_speed;
			break;
		case sf::Keyboard::D:
			m_pos.x += cam_speed;
			break;
		default:
			break;
		}

		Update();
	}
	else if (event.type == sf::Event::MouseWheelMoved)
	{
		m_pos += (float)event.mouseWheel.delta * m_forward * cam_speed;
		Update();
	}

	m_view_mat = glm::lookAt(m_pos, m_pos + m_forward, m_up);
}