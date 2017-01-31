#pragma once

#include "graphics.h"
#include "glm.h"


extern sf::Clock g_delta_clock;
class Camera
{
public:

	Camera();
	~Camera();

	Mat4 GetCamView();
	Mat4 GetProj();
	void HandleInput(sf::Event event);
	void Rotate();
	void Update();

private:
	Vec3 m_pos;
	Vec4 m_vel;
	Vec3 m_target;
	Vec3 m_forward;
	Vec3 m_right;
	Vec3 m_up;

	GLfloat m_yaw;
	GLfloat m_pitch;
	GLfloat m_roll;

	Mat4 m_view_mat;
	Mat4 m_proj_mat;
	//update the view matrix with new camera values
	void UpdateView();
	void MouseInput();
};