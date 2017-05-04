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
private:
	GLuint m_screen_width;
	GLuint m_screen_height;
	GLuint m_worldSize;
public:
	Json m_entity_defs;
	Json m_structureDefs;

	std::map<std::string, GLuint> m_textures;
	std::map<std::string, GLuint> m_normalMaps;
	std::map<std::string, GLuint> m_configSettings;

	std::vector<glm::vec3> m_modelScale;
	size_t m_totalMemory;
public:
	ResourceManager();
	~ResourceManager();


	//loads resources
	void LoadConfig();
	void LoadResources();

	int GetConfigSetting(std::string key);

	//getters
	GLint GetModelID(std::string name);
	Model * GetModel(GLint id);

	GLuint GetTextureID(std::string name);
	GLuint GetNormalMapID(std::string name);
	Json	GetEntityDef(std::string key);
private:
	void LoadSkybox(Json &data);
	void LoadTextures(Json &data);
	void LoadFonts(Json &data);

	void LoadModels(Json &data);
	void LoadModelFrames(Json &data, std::string modelName, std::string dir, glm::vec3 scale);

	void LoadEntityDef(Json &data);
	void LoadStructureDefs(Json &data);

	std::map<std::string, GLint> m_model_keys; //map of model names to their id
	std::vector<Model *> m_models; //vector of models
};

//texures
GLuint GetTextureID(std::string name);
GLuint GetNormalMapID(std::string name);

//model
GLint GetModelID(std::string name);
Model * GetModel(GLint id);
glm::vec3 GetModelScale(GLint id);

//entity
Json GetEntityDef(std::string key);
Json GetStructDef(std::string key);


int GetConfigSetting(std::string key);