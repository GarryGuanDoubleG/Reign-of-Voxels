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
		LoadSkybox(data);
		in.close();
	}

	std::string entity_def = resources["entity"];
	in.open(entity_def);

	if (in.is_open())
	{
		data.clear();
		in >> data;
		LoadEntityDef(data);
		in.close();
	}

	std::string struct_def = resources["structures"];
	in.open(struct_def);

	if (in.is_open())
	{
		data.clear();
		in >> data;
		LoadStructureDefs(data);
		in.close();
	}

	std::string sound_def = resources["sounds"];
	in.open(sound_def);

	if (in.is_open())
	{
		data.clear();
		in >> data;
		
		m_music_defs = data["music"];
		m_sound_defs = data["sound"];

		in.close();
	}

}

void ResourceManager::LoadConfig()
{
	Json resources;

	std::ifstream in(RESOURCE_PATH);

	if (in.is_open())
		in >> resources;
	in.close();

	Json data;

	//load models json
	std::string json_path = resources["config"];
	in.open(json_path);

	if (in.is_open())
	{
		in >> data;
		
		for (Json::iterator it = data.begin(); it != data.end(); ++it)
		{
			m_configSettings.insert(std::pair<std::string, int>(it.key(), it.value()));
		}

		in.close();
	}

}

void ResourceManager::LoadModels(Json &data)
{
	//goes through every object in model.json and loads into globals
	for (Json::iterator it = data.begin(); it != data.end(); ++it)
	{
		Json model = *it;

		std::string path = model["path"];
		std::vector<std::string> animPaths;
		std::vector<std::string> animNames;
		glm::vec3 scale(1.0f);
		
		if (model.find("scale") != model.end())
		{
			scale = glm::vec3(model["scale"][0], model["scale"][1], model["scale"][2]);
			m_modelScale.push_back(scale);
		}
		if (model.find("frames") != model.end())
		{
			std::string modelName = it.key();
			int found = path.find_last_of("\\");
			LoadModelFrames(model["frames"], it.key(), path.substr(0, found+1), scale);
		}
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

		m_model_keys.insert(std::pair<std::string, int>(it.key(), m_models.size() - 1));
	}
}

void ResourceManager::LoadModelFrames(Json &data, std::string modelName, std::string dir, glm::vec3 scale)
{
	int i = 1;
	for (Json::iterator it = data.begin(); it != data.end(); ++it)
	{
		std::string filename = it.value();
		std::string key = modelName + std::to_string(i);

		Model *model = new Model(dir + filename);

		m_models.push_back(model);
		m_modelScale.push_back(scale);
		m_model_keys.insert(std::pair<std::string, int>(key, m_models.size() - 1));
		i++;
	}

}

void ResourceManager::LoadEntityDef(Json &data)
{
	m_entity_defs = data;
}

void ResourceManager::LoadSkybox(Json &data)
{
	std::string path = data["skybox"]["path"];
	GLuint tex = LoadSkyBox(path.c_str());
	m_textures.insert(std::pair<std::string, GLuint>("skybox", tex));
}

void ResourceManager::LoadTextures(Json &data)
{
	for (Json::iterator it = data.begin(); it != data.end(); ++it)
	{
		Json obj = *it;

		if (it.key() == "skybox")
		{
			continue;
		}

		std::string path = obj["path"];
		m_textures.insert(std::pair<std::string, GLuint>(it.key(), LoadTexture(path.c_str())));

		if (obj.find("normal") != obj.end())
		{
			std::string normal = obj["normal"];
			m_normalMaps.insert(std::pair<std::string, GLuint>(it.key(), LoadTexture(normal.c_str())));
		}
	}
}

void ResourceManager::LoadFonts(Json &data)
{

}
void ResourceManager::LoadStructureDefs(Json &data)
{
	m_structureDefs = data;
}

GLint ResourceManager::GetModelID(std::string name)
{
	return m_model_keys[name];
}

Model * ResourceManager::GetModel(GLint id)
{
	return m_models[id];
}

GLuint ResourceManager::GetTextureID(std::string name)
{
	return m_textures[name];
}

GLuint ResourceManager::GetNormalMapID(std::string name)
{
	return m_normalMaps[name];
}

Json ResourceManager::GetEntityDef(std::string key)
{
	return m_entity_defs[key];
}
int ResourceManager::GetConfigSetting(std::string key)
{
	return m_configSettings[key];
}



int GetConfigSetting(std::string key)
{
	return g_resourceManager->GetConfigSetting(key);
}
/*****store things as Json objects or map and use the key to get its value in the definition file ****/


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
glm::vec3 GetModelScale(GLint id)
{
	return g_resourceManager->m_modelScale[id];
}
Model * GetModel(GLint id)
{
	return g_resourceManager->GetModel(id);
}

Json GetStructDef(std::string key)
{
	return g_resourceManager->m_structureDefs[key];
}
Json GetEntityDef(std::string key)
{
	return g_resourceManager->GetEntityDef(key);
}

std::string GetSoundDef(std::string key)
{
	return g_resourceManager->m_sound_defs[key];
}

std::string GetMusicDef(std::string key)
{
	Json data = g_resourceManager->m_music_defs;

	for (Json::iterator it = data.begin(); it != data.end(); ++it)
	{
		std::cout << "data: " << it.key() << std::endl;
		std::cout << "data: " << it.value() << std::endl;
	}

	return g_resourceManager->m_music_defs[key];
}