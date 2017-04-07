#pragma once
#include "3dmath.hpp"
#include "VoxelManager.hpp"

namespace PhysicsUtil
{
	void ScreenPosToWorldRay
	(
		const glm::vec3 &camera_pos,
		const sf::Vector2i& mouse_pos,
		const glm::vec2& window_size,
		const glm::mat4& view_matrix,
		const glm::mat4& projection_matrix,
		Ray& out_ray
	);

	glm::vec3 WorldRayCast
	(
		VoxelManager *manager,
		const Ray &ray,
		const float& length
	);

}