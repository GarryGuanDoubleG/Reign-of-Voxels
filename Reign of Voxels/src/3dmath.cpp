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

bool ClipLine(const glm::vec3 &objPos, BBox aabb, const glm::vec3 &v0, const glm::vec3 &v1, float& fmin, float& fmax)
{
	// f_low and f_high are the results from all clipping so far. We'll write our results back out to those parameters.


	float f_dim_low, f_dim_high;

	//translate box to world pos
	aabb.min += objPos;
	aabb.max += objPos;

	for (int i = 0; i < 3; i++)
	{
		// Find the point of intersection in current dimension
		f_dim_low = (aabb.min[i] - v0[i]) / (v1[i] - v0[i]);
		f_dim_high = (aabb.max[i] - v0[i]) / (v1[i] - v0[i]);

		// Make sure low is less than high
		if (f_dim_high < f_dim_low)
		{
			//swap
			float swap = f_dim_high;
			f_dim_high = f_dim_low;
			f_dim_low = swap;
		}

		//definite miss
		if (f_dim_high < fmin)
			return false;

		//definite miss
		if (f_dim_low > fmax)
			return false;

		fmin = MAX(f_dim_low, fmin);
		fmax = MIN(f_dim_high, fmax);
		
		if (fmin > fmax)
			return false;
	}

	return true;
}
bool LineAABBIntersection(const glm::vec3 &objPos, const BBox &aabb, glm::vec3 v0, glm::vec3 v1, glm::vec3 &outIntersect, float &t)
{
	float fmin = 0;
	float fmax = 1;

	if (!ClipLine(objPos, aabb, v0, v1, fmin, fmax))
		return false;

	glm::vec3 ray = v1 - v0;
	glm::vec3 intersect = v0 + ray * fmin;

	t = fmin;

	return true;
}