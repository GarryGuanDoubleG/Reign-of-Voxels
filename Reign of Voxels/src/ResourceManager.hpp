#pragma once
#include <vector>
#include <GL/glew.h>
#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>
#include "Json.hpp"
#include "Model.hpp"
#include "3dmath.hpp"

using Json = nlohmann::json;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	//loads resources
	void LoadResources();
	void LoadModels(Json data);
	void LoadTextures(Json &data);
	void LoadFonts(Json &data);

	//getters
	GLint GetModelID(std::string name);
	Model * GetModel(GLint id);

	GLuint GetTextureID(std::string name);
	GLuint GetNormalMapID(std::string name);
private:
	std::map<std::string, GLint> m_model_keys; //map of model names to their id
	std::vector<Model *> m_models; //vector of models
	
	std::map<std::string, GLuint> m_textures;
	std::map<std::string, GLuint> m_normalMaps;
	size_t m_totalMemory;
};

GLuint GetTextureID(std::string name);
GLuint GetNormalMapID(std::string name);
GLint GetModelID(std::string name);
Model * GetModel(GLint id);