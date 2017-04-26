#pragma once
#include "3dmath.hpp"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>



std::string GenerateTerrainMap(int resolution);

//float GetPerlinMapValue(float x, float z);
float GetPerlinMapValue(float x, float z);

sf::Color GetPerlinColorValue(float x, float z);