#pragma once
#include "3dmath.hpp"

float Density_Func(const glm::vec3& worldPosition);
float Density_Func(const glm::vec3& worldPosition, const std::vector<glm::vec3> &csgOperationPos);
float Density_Func(const glm::vec3& worldPosition, const glm::vec3 &csgOperationPos);

float Density_PlanarFunc(const glm::vec3 &worldPosition);
