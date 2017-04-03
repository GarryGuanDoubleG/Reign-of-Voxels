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
