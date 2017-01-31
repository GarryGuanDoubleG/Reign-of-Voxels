#pragma once

#include "graphics.h"
#include "glm.h"

class Camera
{
public:

	Camera();
	~Camera();

	Mat4 GetCamView();
	Mat4 GetProj();
	void HandleInput(sf::Event event);
	void Rotate();

private:
	Vec3 m_pos;
	Vec4 m_vel;
	Vec3 m_target;
	Vec3 m_forward;
	Vec3 m_right;
	Vec3 m_up;

	GLfloat yaw;
	GLfloat pitch;
	GLfloat roll;

	Mat4 m_view_mat;
	Mat4 m_proj_mat;
	//update the view matrix with new camera values
	void UpdateView();
};