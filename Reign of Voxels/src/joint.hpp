#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SFML/Graphics.hpp>
#include "3dmath.hpp"

#define BONE_WEIGHTS_SIZE 4

class Joint
{
	friend class Model;
	friend class Mesh;

public:
	GLuint m_id;
	std::string name;

private:
	void InterpolateRotation(glm::mat4 outRotation, float animationTime, aiNodeAnim *nodeAnim);
	void InterpolateScale(glm::mat4 outRotation, float animationTime, aiNodeAnim *nodeAnim);
	void InterpolateTranslation(glm::mat4 outRotation, float animationTime, aiNodeAnim *nodeAnim);


	aiMatrix4x4 m_boneTransform; /**<local bone space transformation matrix*/
	aiMatrix4x4 m_finalTransform; /**<transformation matrix in model space to apply to vertices*/

	std::vector<GLuint> m_vertexIDs;
	std::vector<GLuint> m_weights;

};