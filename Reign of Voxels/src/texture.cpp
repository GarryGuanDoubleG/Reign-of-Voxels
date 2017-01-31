#include <stdio.h>
#include <GL\glew.h>

#include "texture.hpp"
#include "simple_logger.h"

GLuint loadBMP_custom(const char *filepath)
{
	printf("Loading bmp %s\n", filepath);

	//data to store bmp files
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;

	//RGB data
	unsigned char * data;

	FILE * file = fopen(filepath, "rb");
	if (!file)
	{
		slog("Error: Texture could not open %s", filepath);
	}

	if (fread(header, 1, 54, file) != 54)
	{
		slog("Not correct BMP file");
	}

	//get data of image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0) imageSize = width * height * 3;
 	if (dataPos == 0) dataPos = 54;

	//read actual data into buffer
	data = new unsigned char[imageSize];
 	fread(data, 1, imageSize, file);
 	fclose(file);

	GLuint textureID;
	//now generate texture with data
 	glGenTextures(1, &textureID);
  	glBindTexture(GL_TEXTURE_2D, textureID);
	//use GL_BGR because thats how bmp files store color
	//give image to opengl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	//trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	delete[] data;
	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}