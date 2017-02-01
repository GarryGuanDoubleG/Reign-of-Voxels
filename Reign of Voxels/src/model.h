#pragma once
#include "mesh.h"

/*
Container class for Models. Imports and parses model file and stores mesh information
to use for rendering
*/

class Model
{
public:
	Model(GLchar * filepath)
	{
		this->LoadModel(filepath);
	}
	void Draw(GLuint shader);
private:
	std::vector<Texture>textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	void LoadModel(std::string path);
	void ProcessNode(aiNode * node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterials(aiMaterial *mat, aiTextureType type, std::string type_name);
};