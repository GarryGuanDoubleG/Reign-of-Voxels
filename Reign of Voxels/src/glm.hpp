#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>

#include "SFML\Graphics.hpp"

#define Vec4 glm::vec4/**<replace glm::vec4 with Vec4 type  */
#define Vec3 glm::vec3/**<replace glm::vec4 with Vec3 type  */
#define Vec2 glm::vec2 /**<replace glm::vec4 with Vec2 type  */

#define Mat4 glm::mat4/**<replace glm::mat4 with Mat4 type  */
#define Mat3 glm::mat3/**<replace glm::mat3 with Mat3 type  */
#define Mat2 glm::mat2/**<replace glm::mat2 with Mat2 type  */

#define glmNormalize glm::normalize/**<define glm::normalize to glmNormalize for convenience */
#define glmCross glm::cross /**<define glm::cross to glmCross for convenience */

#define glmLookAt glm::lookAt /**<define glm::lookat to glmLookAt for convenience  */  
#define glmPerspective glm::perspective /**< define glm::perspective to glmPerspective for convenience */  

typedef struct
{
	Vec3 position; /**< vertex postion in ndc */
	Vec3 normal; /**< normal value of vertex */
	Vec2 uv; /**< texture coordinates */
}Vertex;

typedef struct
{
	GLbyte x;
	GLbyte y;
	GLbyte z;
}Vec3byte;

typedef struct
{
	GLbyte x;
	GLbyte y;
}Vec2byte;