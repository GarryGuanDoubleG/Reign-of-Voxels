#pragma once
#include "GL\glew.h"

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
	static float size; //size of each side
	VoxelType m_voxelType;

	static GLuint m_vao, m_vbo;
};