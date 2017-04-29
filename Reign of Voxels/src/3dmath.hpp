
#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/postprocess.h>//assimp to glm conversion

typedef struct
{
	glm::vec3 start;
	glm::vec3 dir;
}Ray;

typedef struct Vertex_S
{
	Vertex_S() {};
	Vertex_S(glm::vec3 pos, glm::vec3 norm, glm::vec2 texel)
		: position(pos), normal(norm), uv(texel) {};
	glm::vec3 position; /**< vertex postion in ndc */
	glm::vec3 normal; /**< normal value of vertex */
	glm::vec2 uv; /**< texture coordinates */
}Vertex;

typedef struct
{
	glm::vec3 position; /**< vertex postion in ndc */
	glm::vec3 normal; /**< normal value of vertex */
	glm::vec2 uv; /**< texture coordinates */
	glm::ivec4 boneIds;
	glm::vec4 weights;
}BoneVertex;


typedef struct
{
	glm::vec2 position;
	glm::vec2 size;
}Rectf;

//axis aligned bounding box
typedef struct
{
	glm::vec3 min;
	glm::vec3 max;
}AABB;

typedef struct
{
	glm::vec3 normal;
	glm::vec3 point;
	float D; //equation of plane: Ax +By + Cz + D = 0
}Plane;

enum Side
{
	TOP = 0,
	BOTTOM,
	LEFT,
	RIGHT,
	BACK,
	FRONT,
	FRONT_RIGHT
};

/**
* A macro used to detect 2D collision. Useful to see if mouse click is inside a widget's bounds
*/
#define BOUNDCONTAINS2D(x,y, bx,by,bw,bh) ((x >= bx && y >= by) && (x <= bx + bw && y <= by + bh))
#define BOUNDCONTAINS3D(x,y,z, bx,by,bz,  bw,bh,bl) ((x >= bx && y >= by && z >= bz) \
					   && (x <= bx + bw && y <= by + bh && z <= bz + bl))
#define AABBCONTAINS(p1, aabb) ((p1.x >= aabb.min.x) && (p1.y >= aabb.min.y) && (p1.z >= aabb.min.z) \
								&& (p1.x <= aabb.max.x) && (p1.y <= aabb.max.y) && (p1.z <= aabb.max.z))

//sets 3 points of a plane
#define SET_PLANE(pl, a, b, c) ((pl.p1 = a, pl.p2 = b, pl.p3 = c))

#define MAX(a, b) (a >= b ? a : b)
#define MIN(a, b) (a <= b ? a : b)

void SetPlane(Plane &plane, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
float DistanceToPlane(Plane plane, glm::vec3 point);

//bool ClipLine(const glm::vec3 &objPos, AABB aabb, const glm::vec3 &v0, const glm::vec3 &v1, float& fmin, float& fmax);
//bool ClipLine(int d, const glm::vec3 &objPos, AABB aabb, const glm::vec3 &v0, const glm::vec3 &v1, float& f_low, float& f_high);
bool LineAABBIntersection(const glm::vec3 &objPos, const AABB &aabb, const Ray &ray,
							glm::vec3 &outIntersect, float &out_t);

bool AABBRayIntersection(const glm::vec3 &objPos, const AABB &aabb, const Ray &r);

void assimpToGLMMat4(const aiMatrix4x4 *from, glm::mat4 &to);

float SignedDistFunc(const glm::vec3 &from, const glm::vec3 &to);