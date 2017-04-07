#include "PhysicsUtil.hpp"

const float mod1(const float a)
{
	return a - ((int)a - ((a < 0) ? 1 : 0));
}

const double intbound(float s, const float ds)
{
	if (ds == 0)
	{
		return -1;
	}
	// Find the smallest positive t such that s+t*ds is an integer.
	if (ds < 0)
	{
		return intbound(-s, -ds);
	}
	s = mod1(s);
	// problem is now s+t*ds = 1
	return (1 - s) / ds;
}

glm::vec3 intbounds(const glm::vec3 &start, const glm::vec3 &direction)
{
	return glm::vec3(intbound(start.x, direction.x),
		intbound(start.y, direction.y),
		intbound(start.z, direction.z));

}

const double delta(const double x)
{
	return 1.0f / glm::abs(x);
}

const glm::vec3 delta(glm::vec3 dir)
{
	return glm::vec3(delta(dir.x), delta(dir.y), delta(dir.z));
}

void PhysicsUtil::ScreenPosToWorldRay(const glm::vec3& camera_pos, const sf::Vector2i& mouse_pos, 
									const glm::vec2& window_size, const glm::mat4& view_matrix, 
									const glm::mat4& projection_matrix, Ray& out_ray)
{	
	//expensive ray casting
	glm::vec4 screenPos = glm::vec4((mouse_pos.x * 2.0f / (float)SCREEN_WIDTH) - 1.0f,
		1.0f - (mouse_pos.y * 2.0f / (float)SCREEN_HEIGHT),
		1.0f,
		1.0f);
	//move cam to near plane
	glm::mat4 inverse_VP = glm::inverse((projection_matrix * view_matrix));
	glm::vec4 worldPos = inverse_VP * screenPos;
	worldPos /= worldPos.w;

	out_ray.start = camera_pos;
	out_ray.dir = glm::vec3(worldPos) - camera_pos;
}

bool BoundContains(const glm::vec3 & pos, const glm::dvec3& min, const glm::dvec3& max)
{
	return !(pos.x < min.x || pos.y < min.y || pos.z < min.z || pos.x > max.x || pos.y > max.y || pos.z > max.z);
}


//Based on the paper "A Fast Voxel Travseal Algorithm"
//http://www.cse.yorku.ca/~amana/research/grid.pdf

glm::vec3 PhysicsUtil::WorldRayCast(VoxelManager * voxelManager,
							   const Ray &ray, const float& length)
{
	//0 leads to an infinite loop
	if (ray.dir == glm::vec3(0))
		return;

	const glm::ivec3 step(glm::sign(ray.dir));

	glm::vec3 cube_pos = ray.start;//floor the ray origin by casting to int

	glm::vec3 tMax = intbounds(ray.start, ray.dir);
	glm::vec3 tDelta = delta(ray.dir);

	glm::ivec3 face;//used to track which face was hit

	const glm::vec3 min = ray.start - length;
	const glm::vec3 max = ray.start + length;

	while (// ray has not gone past bounds of world
		(step.x > 0 ? cube_pos.x < max.x : cube_pos.x > min.x) &&
		(step.y > 0 ? cube_pos.y < max.y : cube_pos.y > min.y) &&
		(step.z > 0 ? cube_pos.z < max.z : cube_pos.z > min.z))
	{
		if (BoundContains(cube_pos, min, max) && (voxelManager->BlockWorldPosActive(cube_pos)))
		{
			return cube_pos + .5f;
		}
		// tMax.x stores the t-value at which we cross a cube boundary along the
		// X axis, and similarly for Y and Z. Therefore, choosing the least tMax
		// chooses the closest cube boundary.
		sf::Uint8 i = (tMax.x < tMax.y) ? 0 : 1;
		if (tMax.z < tMax[i])
		{
			i = 2;
		}
		cube_pos[i] += step[i];
		// Adjust tMaxX to the next X-oriented boundary crossing.
		tMax[i] += tDelta[i];
		// Record the normal vector of the cube face we entered.
		face.x = face.y = face.z = 0;
		face[i] = -step[i];
	}
	// there is no cube in range
	return;

}