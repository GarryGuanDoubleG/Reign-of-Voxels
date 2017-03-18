#include "model.hpp"
#include "simple_logger.h"
/** Constructor
* Loads model data using Assimp
*/
Model::Model(GLchar * filepath)
{
	this->LoadModel(filepath);
}
/** Destructor
*
*/
Model::~Model()
{

}
/**
* @brief goes through meshes and updates them to use instanced rendering
* @param buffer the buffer with model matrix data
* @param amount number of times to instance
*/
void Model::SetInstanceRendering(GLuint buffer, GLuint amount)
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].MeshSetInstance(buffer, amount);
}

std::vector<Mesh> *Model::GetMesh()
{
	return &this->meshes;
}

/**
*@brief Reads model file using assimp to get root node of model.
*@param filepath of model
*/
void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		slog("ASSIMP ERROR: %s", importer.GetErrorString());
		return;
	}
	this->directory = path.substr(0, path.find_last_of('\\') + 1);
	this->ProcessNode(scene->mRootNode, scene);
}
/**
* @brief renders all the meshes of this model
* @param shader compiled shader id to use to render
*/
void Model::Draw(GLuint shader)
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Draw(shader);
}
/**
* @brief loads texture data from file
* @param mat assimp material type with texture data
* @param type assimp texture type (diffuse / specular)
* @param type_name string name of texture type
*/
std::vector<Texture> Model::LoadMaterials(aiMaterial *mat, aiTextureType type, std::string type_name)
{
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;

		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = LoadTexture(str.C_Str(), this->directory.c_str());
			texture.type = type_name;
			texture.path = str;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}		
	}
	return textures;
}
/**
* @brief recursively traverses assimp nodes and loads meshes attached to each node
* @param node node to traverse and load meshes
* @oaram scene root assimp node
*/
void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}
/**
* @brief initializes new mesh and stores textures in model class
* @param mesh assimp class that stores mesh verts, normals, textures, and uv coordinates
* @oaram scene root assimp node
*/
Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vert, normal;
		glm::vec2 uv;

		vert.x = mesh->mVertices[i].x;
		vert.y = mesh->mVertices[i].y;
		vert.z = mesh->mVertices[i].z;

		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;
		}
		else
			uv = glm::vec2(0.0f, 0.0f);

		vertex.position = vert;
		vertex.normal = normal;
		vertex.uv = uv;

		vertices.push_back(vertex);
	}
	//store the indices which define the order we draw the triangles
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture>diffuse_maps = this->LoadMaterials(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		std::vector<Texture> specular_maps = this->LoadMaterials(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh(vertices, indices, textures);
}