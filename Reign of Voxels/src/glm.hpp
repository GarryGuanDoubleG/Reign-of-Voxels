#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>

typedef struct
{
	glm::vec3 position; /**< vertex postion in ndc */
	glm::vec3 normal; /**< normal value of vertex */
	glm::vec2 uv; /**< texture coordinates */
}Vertex;

typedef struct
{
	glm::vec2 position;
	glm::vec2 size;
}Rectf;

/**
* A macro used to detect 2D collision. Useful to see if mouse click is inside a widget's bounds
*/
#define BOUNDCONTAINS2D(x,y, bx,by,bw,bh) ((x >= bx && y >= by) && (x <= bx + bw && y <= by + bh))
#define BOUNDCONTAINS3D(x,y,z, bx,by,bz,  bw,bh,bl) ((x >= bx && y >= by && z >= bz) \
					   && (x <= bx + bw && y <= by + bh && z <= bz + bl))
