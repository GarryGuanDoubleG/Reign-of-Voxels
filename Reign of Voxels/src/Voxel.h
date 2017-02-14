#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "model.h"

enum VoxelType
{
	Default = 0, /**< enum type for default blank voxel */  
	Grass, /**< enum type indicating green grass color  */  
	Dirt, /**< enum type dirt light brown color  */  
	Wood, /**<enum type wooden dark brown texture  */  
	Mineral, /**< enum voxel type of mineral resource  */  
	Energy, /**< enum voxel type for energy */  
};
/**
* Lightweight voxel container class that stores type and active flag
*/
class Voxel
{
public:
	/**
	* Constructor
	* sets active flag to false and type to default
	*/
	Voxel();
	/**
	* Destructor
	*/
	~Voxel();
	/**
	* @brief returns active flag
	* @return active flag
	*/
	bool IsActive();
	/**
	* @brief sets the active flag
	* @param active flag to set to
	*/
	void SetActive(bool active);
private:
	bool m_active; /**<flag for whether voxel should be rendered  */  
	VoxelType m_voxelType; /**<type of the voxel  */  

	static Model *m_voxel; /**<reference to vertices and uvs of a voxel  */  
};