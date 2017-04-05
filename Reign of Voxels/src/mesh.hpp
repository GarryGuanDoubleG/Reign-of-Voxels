#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "3dmath.hpp"
#include "texture.hpp"


/**
* Mesh class that contains data on components of a model
*/
class Mesh
{
public:
	/**
	* Constructor
	* stores a reference to vertex, index, and texture containers
	*/
	Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures);

	std::vector<Vertex> vertices; /**<container for vertices in mesh  */  
	std::vector<GLuint> indices; /**< container for order in which to draw triangles*/  
	std::vector<Texture> textures; /**< container for textures used in this mesh*/  
	/**
	* @brief draws all the vertices and textures stored in this mesh
	* @param the shader to use for drawing
	*/
	void Draw(GLuint shader) const;
	/**
	* @brief binds all the vertex data to a vertex array object
	*/
	 void MeshInit();
	 /**
	 * @brief additional buffer for instance rendering 
	 */
	 void MeshSetInstance(GLuint instanceBuffer, GLuint amount);
protected:
	GLuint m_vao, /**< vertex array object, stores gpu state for drawing this mesh */ 
		m_vbo, /**<vertex buffer object, stores the vertex data of this mesh */ 
		m_ebo;/**<element buffer object, stores the order to draw the triangles  */ 

	GLboolean m_instanced;
	GLuint	m_amount;
};
