#include <stdio.h>
#include <GL\glew.h>

#include "texture.hpp"
#include "simple_logger.h"

GLuint loadBMP_custom(const char *filepath)
{
	printf("Loading bmp %s", filepath);

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
		slog("Error: Texture could not open %s\n", filepath);
	}

	return 0;
}