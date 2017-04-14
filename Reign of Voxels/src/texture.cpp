#include <GL\glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texture.hpp"
#include "simple_logger.h"
/**
* @brief loads texture and sets it to use linear interpolation and generate mipmaps
* @param path to texture
* @return id of new generated texture
*/
GLuint LoadTexture(const char *filepath)
{
	sf::Image *img_data = new sf::Image();
	sf::Vector2u size;

	if (!img_data->loadFromFile(filepath))
	{
		slog("Could not load image %s", filepath);
		return 0;
	}

	size = img_data->getSize();

	GLuint textureID;
	//now generate texture with data
 	glGenTextures(1, &textureID);
  	glBindTexture(GL_TEXTURE_2D, textureID);
	//use GL_BGR because thats how bmp files store color
	//give image to opengl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data->getPixelsPtr());

	//trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	delete img_data;
	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}
/*
* @brief helper function that appends filepath to filenaem
* @param filename name of texture to load
* @param filepath path to the texture directory
*/

GLuint LoadTexture(aiTexture *texture)
{
	
	GLuint textureID;

	//now generate texture with data
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//texture compressed. uncompress.
	if (texture->mHeight == 0) 
	{
		sf::Image *image = new sf::Image();
		image->loadFromMemory(texture->pcData, texture->mWidth);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getSize().x, image->getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->getPixelsPtr());
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->mWidth, texture->mHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture->pcData);
	}


	//trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


GLuint LoadTexture(const char *filename, const char * filepath)
{
	std::string name = filepath;
	name += filename;

	return LoadTexture(name.c_str());
}