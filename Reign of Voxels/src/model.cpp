#include "model.hpp"
#include "simple_logger.h"
/** Constructor
* Loads model data using Assimp
*/
Model::Model(GLchar * filepath)
{
	this->LoadModel(filepath);
}

Model::Model(std::string &filepath)
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
* @brief goes through m_meshes and updates them to use instanced rendering
* @param buffer the buffer with model matrix data
* @param amount number of times to instance
*/
void Model::SetInstanceRendering(GLuint buffer, GLuint amount)
{
	for (GLuint i = 0; i < this->m_meshes.size(); i++)
		this->m_meshes[i].MeshSetInstance(buffer, amount);
}

std::vector<Mesh> *Model::GetMesh()
{
	return &this->m_meshes;
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
	std::string directory = path.substr(0, path.find_last_of('\\') + 1);
   	this->ProcessNode(scene->mRootNode, scene, directory);

	for (int i = 0; i < scene->mNumTextures; i++)
	{
		aiTexture* tex = scene->mTextures[i];


		GLuint texture_id = LoadTexture(tex);
		
		//add texture id to child meshes
		for (int i = 0; i < m_meshes.size(); i++)
			m_meshes[i].textures.push_back(texture_id);

	}


}
/**
* @brief renders all the m_meshes of this model
* @param shader compiled shader id to use to render
*/
void Model::Draw(GLuint shader)
{
	for (GLuint i = 0; i < this->m_meshes.size(); i++)
		this->m_meshes[i].Draw(shader);
}
/**
* @brief loads texture data from file
* @param mat assimp material type with texture data
* @param type assimp texture type (diffuse / specular)
* @param type_name string name of texture type
*/
std::vector<GLuint> Model::LoadMaterials(aiMaterial *mat, aiTextureType type, std::string type_name, std::string directory)
{
	std::vector<GLuint> textures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;

		GLuint texture_id;

		texture_id = LoadTexture(str.C_Str(), directory.c_str());

		//check if load failed
		if(texture_id != 0)
			textures.push_back(texture_id);	
	}
	return textures;
}
/**
* @brief recursively traverses assimp nodes and loads m_meshes attached to each node
* @param node node to traverse and load m_meshes
* @oaram scene root assimp node
*/
void Model::ProcessNode(aiNode *node, const aiScene *scene, std::string directory)
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->m_meshes.push_back(this->ProcessMesh(mesh, scene, directory));
	}

	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, directory);
	}
}
/**
* @brief initializes new mesh and stores textures in model class
* @param mesh assimp class that stores mesh verts, normals, textures, and uv coordinates
* @oaram scene root assimp node
*/
Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, std::string directory)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<GLuint> textures;

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


		std::vector<GLuint>diffuse_maps = this->LoadMaterials(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
	}

	return Mesh(vertices, indices, textures);
}