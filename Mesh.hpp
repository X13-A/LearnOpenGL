#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <glad/glad.h>
#include "Vertex.hpp"

class Mesh
{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

	private:
		std::vector<float> verticesData;
		unsigned int VBO;
		unsigned int EBO;
		unsigned int VAO;

	public:
		Mesh(glm::vec3 position, glm::vec3 rotation, std::vector<Vertex> vertices, std::vector<unsigned int> indices) : position(position), rotation(rotation), vertices(vertices), indices(indices)
		{
			// Init Vertex, Indice and Vertex Array objects
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			glGenVertexArrays(1, &VAO);
			verticesData = Vertex::concatenatedData(this->vertices);

			use();
			updateEBO();
			updateVBO();
			updateVertexAttributes();
		}

		void use()
		{
			glBindVertexArray(VAO);
		}

		void setVertices(std::vector<Vertex> data)
		{
			vertices = data;
			verticesData = Vertex::concatenatedData(this->vertices);
			use();
			updateVBO();
		}

		void setIndexes(std::vector<unsigned int> data)
		{
			indices = data;
			use();
			updateEBO();
		}

		void updateBuffers()
		{
			verticesData = Vertex::concatenatedData(this->vertices);
			use();
			updateVBO();
			updateEBO();
		}

	private:

		/// <summary>
		/// WARNING: This methods needs updating everytime the "Vertex" structure is changed
		/// </summary>
		void updateVertexAttributes()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::dataSize, (void*)0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, Vertex::dataSize, (void*)(4 * sizeof(float)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::dataSize, (void*)(7 * sizeof(float)));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}

		void updateVBO()
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * Vertex::dataSize, verticesData.data(), GL_STATIC_DRAW);
		}

		void updateEBO()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		}
};

#endif