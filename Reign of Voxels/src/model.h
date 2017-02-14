#pragma once
#include "mesh.h"
/**
*3D model class. Supports most file formats. Imports and parses model file and stores mesh information
*to use for rendering
*/
class Model
{
public:
	/** Constructor
	* Loads model data using Assimp
	*/
	Model(GLchar * filepath);
	/** Destructor
	*
	*/
	~Model();
	/**
	* @brief renders all the meshes of this model
	* @param shader compiled shader id to use to render
	*/
	void Draw(GLuint shader);
private:
	std::vector<Texture>textures_loaded; /**<vector of loaded textures */  
	std::vector<Mesh> meshes; /**<container for all meshes in model */  
	std::string directory;/**< the file directory of model*/  

	/**
	*@brief Reads model file using assimp to get root node of model.
	*@param filepath of model
	*/
	void LoadModel(std::string path);
	/**
	* @brief recursively traverses assimp nodes and loads meshes attached to each node
	* @param node node to traverse and load meshes
	* @oaram scene root assimp node
	*/
	void ProcessNode(aiNode * node, const aiScene* scene);
	/**
	* @brief initializes new mesh and stores textures in model class
	* @param mesh assimp class that stores mesh verts, normals, textures, and uv coordinates
	* @oaram scene root assimp node
	*/
	Mesh ProcessMesh(aiMesh * mesh, const aiScene* scene);
	/**
	* @brief loads texture data from file
	* @param mat assimp material type with texture data
	* @param type assimp texture type (diffuse / specular)
	* @param type_name string name of texture type
	*/
	std::vector<Texture> LoadMaterials(aiMaterial *mat, aiTextureType type, std::string type_name);
};