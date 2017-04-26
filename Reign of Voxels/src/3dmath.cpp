#include <utility>
#include <algorithm>
#include <iostream>

#include "3dmath.hpp"



void SetPlane(Plane &plane, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 aux1, aux2;

	aux1 = p1 - p2;
	aux2 = p3 - p2;

	plane.normal = glm::normalize(glm::cross(aux2, aux1));
	plane.point = p2;

	plane.D = -(glm::dot(plane.normal, plane.point));
}

float DistanceToPlane(Plane plane, glm::vec3 point)
{
	//calculate D of plane Ax + By + Cz + D = 0
	//D = -n dot point
	return glm::dot(plane.normal, point) + plane.D; // signed distance from plane to point
}

void GetAABBPlane(Plane &out, const AABB &aabb, Side side, float D)
{
	out.D = D;

	switch (side)
	{
	case TOP:
		out.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		break;
	case BOTTOM:
		out.normal = glm::vec3(0.0f, -1.0f, 0.0f);
		break;
	case LEFT:
		out.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
		break;
	case RIGHT:
		out.normal = glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	case BACK:
		out.normal = glm::vec3(0.0f, 0.0f, -1.0f);
		break;
	case FRONT:
		out.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	}
}

bool RayInPlane(const Ray &ray, const Plane &plane, glm::vec3 &intersect)
{
	float denom, t;

	denom = glm::dot(ray.dir, plane.normal);
	
	if (denom == 0) return 0;

	t = -((glm::dot(ray.start, plane.normal) + plane.D) / denom);

	intersect = ray.start + ray.dir * t;

	if ((t > 0) && (t <= 1000))
		return true;

	return false;
}

bool LineAABBIntersection(const glm::vec3 &objPos, const AABB &aabb, const Ray &ray,
							glm::vec3 &outIntersect, float &out_t)
{
	Plane planes[3]; //check 3 visible planes

	AABB worldAABB = aabb;//aabb in world space
	worldAABB.min += objPos;
	worldAABB.max += objPos;

	float distance = glm::distance(ray.start, objPos);
	std::cout << "Distance " << distance << std::endl;

	if (objPos.x <= ray.start.x)
		GetAABBPlane(planes[0], aabb, RIGHT, worldAABB.max.x);
	else
		GetAABBPlane(planes[0], aabb, LEFT, worldAABB.min.x);

	if (objPos.y <= ray.start.y)
		GetAABBPlane(planes[1], aabb, TOP, worldAABB.max.y);
	else
		GetAABBPlane(planes[1], aabb, BOTTOM, worldAABB.min.y);

	if (objPos.z <= ray.start.z)
		GetAABBPlane(planes[2], aabb, FRONT, worldAABB.max.z);
	else
		GetAABBPlane(planes[2], aabb, BACK, worldAABB.min.z);

	//check for ray intersections with 3 visible faces
	for (int i = 0; i < 3; i++)
	{
		glm::vec3 intersect(0.0f);
		
		if (!RayInPlane(ray, planes[i], intersect))
			continue;

		if (AABBCONTAINS(intersect, worldAABB))
		{
			outIntersect = intersect;
			return true;
		}
		//TODO check if bounds of the rectangle of plane
	}

	out_t = -1;
	outIntersect = glm::vec3(-1.0f);
	return false;
}

bool AABBRayIntersection(const glm::vec3 &objPos, const AABB &aabb, const Ray &r)
{
 	AABB worldAABB = aabb;
	worldAABB.min += objPos;
	worldAABB.max += objPos;

	float tmin = (worldAABB.min.x - r.start.x) / r.dir.x;
	float tmax = (worldAABB.max.x - r.start.x) / r.dir.x;

	if (tmin > tmax) 
		std::swap(tmin, tmax);

	float tymin = (worldAABB.min.y - r.start.y) / r.dir.y;
	float tymax = (worldAABB.max.y - r.start.y) / r.dir.y;

	if (tymin > tymax) 
		std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (worldAABB.min.z - r.start.z) / r.dir.z;
	float tzmax = (worldAABB.max.z - r.start.z) / r.dir.z;

	if (tzmin > tzmax) 
		std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

void assimpToGLMMat4(const aiMatrix4x4 *from, glm::mat4 &to)
{
	to[0][0] = from->a1; to[1][0] = from->a2;
	to[2][0] = from->a3; to[3][0] = from->a4;
	to[0][1] = from->b1; to[1][1] = from->b2;
	to[2][1] = from->b3; to[3][1] = from->b4;
	to[0][2] = from->c1; to[1][2] = from->c2;
	to[2][2] = from->c3; to[3][2] = from->c4;
	to[0][3] = from->d1; to[1][3] = from->d2;
	to[2][3] = from->d3; to[3][3] = from->d4;
}

float SignedDistFunc(const glm::vec3 &from, const glm::vec3 &to)
{

}