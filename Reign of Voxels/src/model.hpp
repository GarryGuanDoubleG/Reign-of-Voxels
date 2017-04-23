#pragma once
#include <map>

#include "mesh.hpp"
#include "joint.hpp"
#include "AnimationController.hpp"

/**
*3D model class. Supports most file formats. Imports and parses model file and stores mesh information
*to use for rendering
*/
class Model
{
private:
	AnimationController m_animController;

	Assimp::Importer m_importer;
	const aiScene *m_scene;
	glm::mat4 m_GlobalInverseTransform;

	std::vector<Mesh> m_meshes; /**<container for all meshes in model */

	bool rigged;

public:
	/** Constructor
	* Loads model data using Assimp
	*/
	Model(GLchar *filepath);

	Model(std::string &filepath);

	Model(const std::string &filepath, const std::vector<std::string> &animPaths, const std::vector<std::string> &animNames);

	/** Destructor
	*
	*/
	~Model();

	bool IsRigged();

	/**
	* @brief goes through meshes and updates them to use instanced rendering
	* @param buffer the buffer with model matrix data
	* @param amount number of times to instance
	*/
	void SetInstanceRendering(GLuint buffer, GLuint amount);

	/**
	* @brief returns the meshes that this model contains
	* @return vector container of this model's meshes
	*/
	std::vector<Mesh> *GetMesh();

	/**
	* @brief renders all the meshes of this model
	* @param shader compiled shader id to use to render
	*/
	void Draw(GLuint shader);

	/**
	* @brief renders all the meshes of this model
	* @param shader compiled shader id to use to render
	* @param time in seconds used to interpolate animation vertices
	*/
	void Draw(GLuint shader, float timeInSeconds);
private:
	/**
	*@brief Load textures that are embedded into the model (.fbx)
	*/
	void LoadEmbeddedTextures();
	void LoadAnimations(const std::vector<std::string> &animations, const std::vector<std::string> &animNames);
	/**
	*@brief Reads model file using assimp to get root node of model.
	*@param path filepath of model
	*/
	void LoadModel(std::string path);

	void LoadModel(std::string model_path, const std::vector<std::string> &animations, const std::vector<std::string> &animNames);

	/**
	*@brief Loads bones
	*@param scene, root assimp container
	*/
	void LoadBones(aiMesh*mesh, std::vector<glm::ivec4> &boneIds, std::vector<glm::vec4> &weights);
	/**
	* @brief recursively traverses assimp nodes and loads meshes attached to each node
	* @param node node to traverse and load meshes
	* @param scene root assimp node
	* @param directory directory of model that's being loaded
	*/
	void ProcessNode(aiNode * node, std::string directory);
	/**
	* @brief initializes new mesh and stores textures in model class
	* @param mesh assimp class that stores mesh verts, normals, textures, and uv coordinates
	* @oaram scene root assimp node
	*/
	Mesh ProcessMesh(aiMesh * mesh, std::string directory);
	/**
	* @brief loads texture data from file
	* @param mat assimp material type with texture data
	* @param type assimp texture type (diffuse / specular)
	* @param type_name string name of texture type
	*/
	std::vector<GLuint> LoadMaterials(aiMaterial *mat, aiTextureType type, std::string type_name, std::string directory);


};