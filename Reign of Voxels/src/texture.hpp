#pragma once
#include <assimp/scene.h>
#include "SFML\Graphics.hpp"
/**
* Texture struct that stores everything needed to 
*/
struct Texture
{
	GLuint id; /**<id of binded texture */  
	std::string type; /**<type of the texture  */  
	aiString path; /**<filepath to texture  */  
};
/**
* @brief loads texture and sets it to use linear interpolation and generate mipmaps
* @param path to texture
* @return id of new generated texture
*/
GLuint LoadTexture(const char * filepath);
/*
* @brief helper function that appends filepath to filenaem
* @param filename name of texture to load
* @param filepath path to the texture directory
*/
GLuint LoadTexture(const char *filename, const char * filepath);
