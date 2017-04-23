#include "model.hpp"
#include "simple_logger.h"
/** Constructor
* Loads model data using Assimp
*/

Model::Model(GLchar *filepath)
{

}

Model::Model(std::string &filepath)
{
	this->LoadModel(filepath);
}

Model::Model(const std::string &filepath, const std::vector<std::string> &animPaths, const std::vector<std::string> &animNames)
{
	this->LoadModel(filepath, animPaths, animNames);
}

/** Destructor
*
*/
Model::~Model()
{

}

bool Model::IsRigged()
{
	return rigged;
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

void Model::LoadEmbeddedTextures()
{
	//add embedded textures
	for (int i = 0; i < m_scene->mNumTextures; i++)
	{
		aiTexture* tex = m_scene->mTextures[i];

		GLuint texture_id = LoadTexture(tex);

		//add texture id to child meshes
		for (int i = 0; i < m_meshes.size(); i++)
			m_meshes[i].textures.push_back(texture_id);
	}
}

/**
*@brief Reads model file using assimp to get root node of model.
*@param filepath of model
*/
void Model::LoadModel(std::string model_path)
{
	m_scene = m_importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
	{
		slog("ASSIMP ERROR: %s", m_importer.GetErrorString());
		return;
	}
	
	std::string directory = model_path.substr(0, model_path.find_last_of('\\') + 1);

   	this->ProcessNode(m_scene->mRootNode, directory);

	LoadEmbeddedTextures();
}

void Model::LoadAnimations(const std::vector<std::string> &animations, const std::vector<std::string> &animNames)
{
	for (int i = 0; i < animations.size(); i++)
	{
		m_animController.LoadAnimation(animations[i], animNames[i]);
	}
}

void Model::LoadModel(std::string model_path, const std::vector<std::string> &animations, const std::vector<std::string> &animNames)
{
	m_scene = m_importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
	{
		slog("ASSIMP ERROR: %s", m_importer.GetErrorString());
		return;
	}

	std::string directory = model_path.substr(0, model_path.find_last_of('\\') + 1);

	aiMatrix4x4 globalTransform = m_scene->mRootNode->mTransformation;
	globalTransform.Inverse();

	assimpToGLMMat4(&globalTransform, m_GlobalInverseTransform);

	this->ProcessNode(m_scene->mRootNode, directory);

	LoadEmbeddedTextures();

	LoadAnimations(animations, animNames);

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

void Model::Draw(GLuint shader, float timeInSeconds)
{
	m_animController.SetAnimation(timeInSeconds, "walking");
	m_animController.BoneTransform(timeInSeconds);

	glUniformMatrix4fv(glGetUniformLocation(shader, "gBones"), 
						m_animController.m_finalTransforms.size(), GL_FALSE, 
						glm::value_ptr(m_animController.m_finalTransforms[0]));

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
* @oaram m_scene root assimp node
*/
void Model::ProcessNode(aiNode *node, std::string directory)
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_scene->mMeshes[node->mMeshes[i]];
		this->m_meshes.push_back(this->ProcessMesh(mesh, directory));
	}

	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i],  directory);
	}
}

void Model::LoadBones(aiMesh*mesh, std::vector<glm::ivec4> &boneIds, std::vector<glm::vec4> &weights)
{
	m_animController.m_boneTransforms.resize(m_animController.m_boneTransforms.size() + mesh->mNumBones);
	weights.resize(mesh->mNumVertices);
	boneIds.resize(mesh->mNumVertices);

	std::fill(boneIds.begin(), boneIds.end(), glm::ivec4(0));
	std::fill(weights.begin(), weights.end(), glm::vec4(0));

	for (int i = 0; i < mesh->mNumBones; i++)
	{
		int boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.data);

		if (m_animController.m_boneMap.find(boneName) == m_animController.m_boneMap.end())
		{
			boneIndex = m_animController.m_boneCount;
			++m_animController.m_boneCount;
		}
		else
		{
			boneIndex = m_animController.m_boneMap[boneName];
		}

		m_animController.m_boneMap[boneName] = boneIndex;

		glm::mat4 offsetMat(0);
		assimpToGLMMat4(&mesh->mBones[i]->mOffsetMatrix, offsetMat);

		m_animController.m_boneTransforms[boneIndex] = offsetMat;

		for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;

			//find unused weight and assign
			for (int k = 0; k < 4; k++)
			{
				if (weights[vertexID][k] == 0)
				{
					boneIds[vertexID][k] = boneIndex;
					weights[vertexID][k] = weight;
					break;
				}
			}

		}

	}
}

std::vector<Vertex> LoadVertices(aiMesh*mesh)
{
	std::vector<Vertex> vertices;
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

	return vertices;
}

std::vector<BoneVertex> LoadBoneVertices(aiMesh *mesh, std::vector<glm::ivec4> boneIds, 
											std::vector<glm::vec4> weights)
{
	std::vector<BoneVertex> vertices;
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		BoneVertex vertex;

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
		vertex.boneIds = boneIds[i];
		vertex.weights = weights[i];

		vertices.push_back(vertex);
	}

	return vertices;
}

/**
* @brief initializes new mesh and stores textures in model class
* @param mesh assimp class that stores mesh verts, normals, textures, and uv coordinates
* @oaram m_scene root assimp node
*/
Mesh Model::ProcessMesh(aiMesh *mesh,  std::string directory)
{
	//vertex
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<GLuint> textures;

	//bones
	std::vector<glm::ivec4> boneIds;
	std::vector<glm::vec4> weights;


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
		aiMaterial *material = m_scene->mMaterials[mesh->mMaterialIndex];

		std::vector<GLuint>diffuse_maps = this->LoadMaterials(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
	}

	//load skeletal data
	if (mesh->HasBones())
	{
		rigged = true;
		LoadBones(mesh, boneIds, weights);

		std::vector<BoneVertex> boneVertices = LoadBoneVertices(mesh, boneIds, weights);
		return Mesh(boneVertices, indices, textures);
	}
	else
	{
		vertices = LoadVertices(mesh);
	}

	return Mesh(vertices, indices, textures);
}
