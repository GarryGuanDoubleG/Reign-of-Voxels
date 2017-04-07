#include "PhysicsUtil.hpp"


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


void PhysicsUtil::WorldRayCast(const VoxelManager * voxelManager, 
							   const Ray &ray, const float& length)
{



}