#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "model.h"


enum VoxelType
{
	Default = 0,
	Grass,
	Dirt,
	Wood,
	Mineral,
	Energy,
};

class Voxel
{
public:
	Voxel();
	~Voxel();

	bool IsActive();
	void SetActive(bool active);

private:
	bool m_active;
	VoxelType m_voxelType;
	//static float size; //size of each side
	static Model *m_voxel;

};