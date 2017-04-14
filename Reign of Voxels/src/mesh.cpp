#include "mesh.hpp"
/**
* Constructor
* stores a reference to vertex, index, and texture containers
*/
Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<GLuint> &textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->MeshInit();
	
	this->m_instanced = GL_FALSE;
}
/**
* @brief draws all the vertices and textures stored in this mesh
* @param the shader to use for drawing
*/
void Mesh::Draw(GLuint shader) const
{
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
										  // Retrieve texture number (the N in diffuse_textureN)

		std::string textUniform = "texture_diffuse" + std::to_string(i);

		GLuint location = glGetUniformLocation(shader, textUniform.c_str());
		// Now set the sampler to the correct texture unit
		glUniform1i(location, i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i]);
	}

	// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
	glUniform1f(glGetUniformLocation(shader, "material.shininess"), 16.0f);

	// Draw mesh
	glBindVertexArray(this->m_vao);
	if (m_instanced)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, m_instanceCount);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	// Unbind the textures after drawing
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
/**
* @brief binds all the vertex data to a vertex array object
*/
void Mesh::MeshInit()
{
	glGenVertexArrays(1, &this->m_vao);
	glGenBuffers(1, &this->m_vbo);
	glGenBuffers(1, &this->m_ebo);

	glBindVertexArray(this->m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	//setup for indices drawing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	//location 0 should be verts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	//now normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	//now textures
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
/**
* @brief additional buffer for instance rendering
* @param intstanceBuffer bind buffer with instance world position data
* @param amount number of times to instance render this mesh
*/
void Mesh::MeshSetInstance(GLuint instanceBuffer, GLuint amount)
{
	m_instanceCount = amount;
	m_instanced = GL_TRUE;

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

	GLsizei vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}