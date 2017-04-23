#include <fstream>
#include "ResourceManager.hpp"

#define RESOURCE_PATH "Resources\\resources.json"

ResourceManager::ResourceManager()
{

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

	Json model_data;

	//load models json
	std::string path = resources["models"];
	in.open(path);
	
	if (in.is_open())
	{
		in >> model_data;
		LoadModels(model_data);
	}

	in.close();
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

}
void ResourceManager::LoadFonts(Json &data)
{

}