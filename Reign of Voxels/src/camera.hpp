#pragma once
#include "game.hpp"
#include "3dmath.hpp"
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
	Camera(glm::vec3 position, glm::vec3 target);
	/**
	* Empty Destructor
	*/
	~Camera();

	void Update();
	void UpdateFrustum();
	/**
	* @brief returns the view matrix
	* @return the view matrix
	*/
	glm::mat4 GetViewMat();
	/**
	* @brief returns the projection matrix
	* @return the projection matrix
	*/
	glm::mat4 GetProj();
	glm::vec3 GetRight();
	glm::vec3 GetUp();
	glm::vec3 GetForward();
	glm::vec3 GetRotation();
	glm::vec3 GetPosition();


	/**
	* @brief adjusts camera on user input
	* @param event the user input
	*/
	void HandleInput(sf::Event event);

	void SwitchProjection();

	void SetToOrtho(glm::mat4 ortho);
	void SetToPersp();
	void SetPosition(glm::vec3 position);
	void InvertPitch();

	//checks if object is within frustum
	bool PointInCamera(glm::vec3 &point);
	bool SphereInCamera(glm::vec3 &point, float radius);
	bool AABBInCamera(AABB &aabb);
	bool AABBInCamera(glm::ivec3 minPos, int size);

private:
	void HandleKeyboard();

	bool m_keyW;
	bool m_keyA;
	bool m_keyS;
	bool m_keyD;

	glm::vec3 m_pos; /**< position of camera */  
	glm::vec3 m_target; /**<the position the camera looks at */  
	glm::vec3 m_forward; /**< the direction the camera loks at */  
	glm::vec3 m_right; /**< the direction right of the camera */  
	glm::vec3 m_up; /**<the direction above the camera  */  

	GLfloat m_yaw; /**< x rotation of camera */  
	GLfloat m_pitch; /**< y rotation of camera  */  
	GLfloat m_roll; /**<  z rotation of camera*/  
	
	/**frustum culling**/
	GLfloat m_fov;
	GLfloat m_aspect_ratio;
	GLfloat m_nearD;
	GLfloat m_farD;

	GLfloat m_nearH;
	GLfloat m_nearW;
	GLfloat m_farH;
	GLfloat m_farW;	

	Plane m_planes[6];
	/**end of frustum culling **/

	//projection and view matrices 			
	glm::mat4 m_view_mat;
	glm::mat4 m_perspect_proj; /**< projection matrix*/
	glm::mat4 m_ortho_proj;

	GLfloat m_zoom;

	glm::vec3 m_ray;

	bool m_perspective; /**< flag to use projection or ortho view */
};