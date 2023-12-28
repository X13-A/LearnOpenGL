#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Commons.hpp"
#include "Vertex.hpp"
#include <iostream>

class Mesh
{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		glm::mat4 transformMatrix = glm::mat4(1.0f);

	private:
		std::vector<float> verticesData;
		unsigned int VBO;
		unsigned int EBO;
		unsigned int VAO;


	public:
		Mesh(glm::vec3 position, glm::vec3 rotation, std::vector<Vertex> vertices, std::vector<unsigned int> indices) : position(position), rotation(rotation), vertices(vertices), indices(indices)
		{
			// TODO: Translate and rotate vertices
			setPosition(position);
			setRotation(rotation);

			// Init Vertex, Indice and Vertex Array objects
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			glGenVertexArrays(1, &VAO);

			use();
			updateEBO();
			updateVBO();
			updateVertexAttributes();
		}

		void use()
		{
			glBindVertexArray(VAO);
		}

		void draw()
		{
			use();
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
		}

		void setVertices(std::vector<Vertex> data)
		{
			vertices = data;
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
			use();
			updateVBO();
			updateEBO();
		}

		void translate(const glm::vec3& offset) 
		{
			position += offset;
			transformMatrix = glm::translate(glm::mat4(1.0f), offset) * transformMatrix;
			updateVBO();
		}

		void rotate(const glm::vec3& axis, float degree) 
		{
			float radians = glm::radians(degree);
			rotation += radians * axis;
			transformMatrix = glm::rotate(transformMatrix, radians, axis);
			updateVBO();
		}

		void setPosition(const glm::vec3& newPosition) 
		{
			glm::vec3 delta = newPosition - position;
			translate(delta);
		}

		void setRotation(const glm::vec3& newRotation) 
		{
			transformMatrix = glm::mat4(1.0f);
			transformMatrix = glm::translate(transformMatrix, position);

			glm::vec3 rotationRadians = glm::radians(newRotation);
			transformMatrix = glm::rotate(transformMatrix, rotationRadians.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
			transformMatrix = glm::rotate(transformMatrix, rotationRadians.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
			transformMatrix = glm::rotate(transformMatrix, rotationRadians.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around z-axis

			rotation = newRotation;
			updateVBO();
		}

		void paint(glm::vec4 color)
		{
			for (int i = 0; i < vertices.size(); i++)
			{
				vertices[i].color = color;
			}
			use();
			updateVBO();
		}

	private:

		/// <summary>
		/// WARNING: This methods needs updating everytime the "Vertex" structure is changed
		/// </summary>
		void updateVertexAttributes()
		{
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, Vertex::dataSize, (void*)(0 * sizeof(float)));
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, Vertex::dataSize, (void*)(4 * sizeof(float)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::dataSize, (void*)(8 * sizeof(float)));
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, Vertex::dataSize, (void*)(10 * sizeof(float)));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
		}

		void updateVBO()
		{
			verticesData = Vertex::concatenatedData(this->vertices);
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