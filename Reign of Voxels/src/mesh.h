#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "graphics.h"
#include "glm.h"
#include "texture.hpp"

//should probably move these into a utility file
typedef struct
{
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
}Vertex;


class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures);

	// Render the mesh
	void Draw(GLuint shader) const;
	void MeshInit();

private:
	GLuint vao, vbo, ebo;
};
