#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>

#define Vec4 glm::vec4
#define Vec3 glm::vec3
#define Vec2 glm::vec2

#define Mat4 glm::mat4
#define Mat3 glm::mat3
#define Mat2 glm::mat2

#define glmNormalize glm::normalize
#define glmCross glm::cross

#define glmLookAt glm::lookAt
#define glmPerspective glm::perspective