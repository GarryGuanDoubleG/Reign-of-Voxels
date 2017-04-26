#include <fstream>
#include "ResourceManager.hpp"

#define RESOURCE_PATH "Resources\\resources.json"

static ResourceManager *g_resourceManager;

ResourceManager::ResourceManager()
{
	g_resourceManager = this;
}

ResourceManager::~ResourceManager()
{

}


void ResourceManager::LoadResources()
{
	Json resources;

	std::ifstream in(RESOURCE_PATH);
	
	if (in.is_open())
		in >> resources;
	in.close();

	Json data;

	//load models json
	std::string json_path = resources["models"];
	in.open(json_path);
	
	if (in.is_open())
	{
		in >> data;
		LoadModels(data);
		in.close();
	}

	std::string texture_path = resources["textures"];
	in.open(texture_path);

	if (in.is_open())
	{
		data.clear();
		in >> data;
		LoadTextures(data);
		in.close();
	}


}

GLint ResourceManager::GetModelID(std::string name)
{
	return m_model_keys[name];
}

Model * ResourceManager::GetModel(GLint id)
{
	return m_models[id];
}

void ResourceManager::LoadModels(Json data)
{
	int i = 0;

	//goes through every object in model.json and loads into globals
	for (Json::iterator it = data.begin(); it != data.end(); ++it)
	{
		Json model = *it;

		std::string path = model["path"];
		std::vector<std::string> animPaths;
		std::vector<std::string> animNames;

		if (model.find("anim") != model.end())
		{
			for (Json::iterator it_anim = model["anim"].begin(); it_anim != model["anim"].end(); ++it_anim)
			{
				animPaths.push_back(it_anim.value());
				animNames.push_back(it_anim.key());
			}

			m_models.push_back(new Model(path, animPaths, animNames));
		}
		else
		{
			m_models.push_back(new Model(path));
		}

		m_model_keys.insert(std::pair<std::string, int>(it.key(), i));
		++i;
	}
}

void ResourceManager::LoadTextures(Json &data)
{
	for (Json::iterator it = data.begin(); it != data.end(); ++it)
	{
		Json obj = *it;
		std::string path = obj["path"];
		m_textures.insert(std::pair<std::string, GLuint>(it.key(), LoadTexture(path.c_str())));

		std::string normal = obj["normal"];
		m_normalMaps.insert(std::pair<std::string, GLuint>(it.key(), LoadTexture(normal.c_str())));
	}
}

GLuint ResourceManager::GetTextureID(std::string name)
{
	return m_textures[name];
}

GLuint ResourceManager::GetNormalMapID(std::string name)
{
	return m_normalMaps[name];
}


void ResourceManager::LoadFonts(Json &data)
{

}

GLuint GetTextureID(std::string name)
{
	return g_resourceManager->GetTextureID(name);
}
GLuint GetNormalMapID(std::string name)
{
	return g_resourceManager->GetNormalMapID(name);
}
GLint GetModelID(std::string name)
{
	return g_resourceManager->GetModelID(name);
}

Model * GetModel(GLint id)
{
	return g_resourceManager->GetModel(id);
}