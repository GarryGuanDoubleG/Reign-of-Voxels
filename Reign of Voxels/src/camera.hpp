#pragma once
#include "game.hpp"
#include "glm.hpp"
/**
* Manages frustum by managing projection and view matrice. Sets up what the player can see.
* Currently creates a perspective projection but will eventually change to orthographic projection
*/
class Camera
{
public:
	/**
	* A constructor
	* Sets up projection and view matrices. Starts at position 0,0,3 and looks towards origin
	*/
	Camera(Vec3 position, Vec3 target, Vec3 forward);
	/**
	* Empty Destructor
	*/
	~Camera();
	/**
	* @brief returns the view matrix
	* @return the view matrix
	*/
	Mat4 GetViewMat();
	/**
	* @brief returns the projection matrix
	* @return the projection matrix
	*/
	Mat4 GetProj();

	Mat4 GetPerspectProj();

	Mat4 GetOrthoProj();
	/**
	* @brief adjusts camera on user input
	* @param event the user input
	*/
	void HandleInput(sf::Event event);

	Vec3 GetPosition();
	void SetPosition(Vec3 position);

	void SwitchProjection();
	void SetToOrtho();
	void SetToPersp();

private:
	sf::Vector2i m_last_mouse_pos; /**<The last position mouse was tracked. Used to compare to new mouse pos  */
	Vec3 m_pos; /**< position of camera */  
	Vec3 m_target; /**<the position the camera looks at */  
	Vec3 m_forward; /**< the direction the camera loks at */  
	Vec3 m_right; /**< the direction right of the camera */  
	Vec3 m_up; /**<the direction above the camera  */  

	GLfloat m_yaw; /**< x rotation of camera */  
	GLfloat m_pitch; /**< y rotation of camera  */  
	GLfloat m_roll; /**<  z rotation of camera*/  

	//projection and view matrices 			
	Mat4 m_view_mat;
	Mat4 m_perspect_proj; /**< projection matrix*/
	Mat4 m_ortho_proj;

	float m_zoom;

	bool m_perspective; /**< flag to use projection or ortho view */

};