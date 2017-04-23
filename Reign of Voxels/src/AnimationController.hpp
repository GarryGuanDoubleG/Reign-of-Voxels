#pragma once
#include <vector>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "simple_logger.h"
#include <SFML/OpenGL.hpp>

#include "3dmath.hpp"
#define NUM_BONES_PER_VERTEX 4

class AnimationController
{

public:
	std::vector<glm::mat4> m_boneTransforms; /**<container for joints used in skeletal animations*/
	std::vector<glm::mat4> m_finalTransforms;

	std::map<std::string, GLuint>m_boneMap; /**<map of bone ids*/
	GLuint m_boneCount;

private:
	std::string m_modelPath;

	float m_animationTime0;
	float m_animationTime1;

	float m_playTime;

	int m_numAnims;
	int m_currAnim;

	std::vector<Assimp::Importer *> m_importers;
	std::vector<const aiScene *> m_scenes;
	std::vector<glm::mat4> m_GlobalInverseTransforms;

	std::map<std::string, int> m_animIDMap;

public:
	AnimationController();

	void SetAnimation(float currTime, const std::string &name);
	void BoneTransform(float time);
	void LoadAnimation(const std::string &filepath, const std::string &animName);

private:

	const aiNodeAnim* FindNodeAnim(const aiAnimation* anim, const std::string NodeName);

	void InterpolateScaling(aiVector3D &out, float animTime, const aiNodeAnim *animNode);
	void InterpolateRotation(aiQuaternion & out, float animTime, const aiNodeAnim* animNode);
	void InterpolatePosition(aiVector3D& out, float animTime, const aiNodeAnim* animNode);

	void CalculateHeiarchyTransform(float animTime, aiNode *node, glm::mat4 transform);

	//void CalculateHeiarchyTransform(const aiScene *scene0, const aiScene scene1, float animTime0, float animTime1, aiNode *node0, aiNode *node1, glm::mat4 parentTransform);
};