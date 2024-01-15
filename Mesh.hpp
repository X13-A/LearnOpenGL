#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Commons.hpp"
#include "Vertex.hpp"
#include <iostream>
#include "Shader.hpp"

class Mesh
{
	private:
		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;
		glm::mat4 _transformMatrix = glm::mat4(1.0f);
		glm::vec3 _position;
		glm::vec3 _rotation;
		glm::vec3 _scale;

		std::vector<float> _verticesData;
		unsigned int _VBO;
		unsigned int _EBO;
		unsigned int _VAO;

	public:

		Mesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::vector<Vertex> vertices, std::vector<unsigned int> indices) : _position(position), _rotation(rotation), _scale(scale), _vertices(vertices), _indices(indices)
		{
			// TODO: Translate and rotate vertices
			setPosition(position);
			setRotation(rotation);

			// Init Vertex, Indice and Vertex Array objects
			glGenBuffers(1, &_VBO);
			glGenBuffers(1, &_EBO);
			glGenVertexArrays(1, &_VAO);

			use();
			updateEBO();
			updateVBO();
			updateVertexAttributes();
		}

		#pragma region getters

		std::vector<Vertex> getVertices() const
		{
			return _vertices;
		}

		std::vector<unsigned int> getIndices() const
		{
			return _indices;
		}

		glm::mat4 getTransformMatrix() const
		{
			return _transformMatrix;
		}

		glm::vec3 getPosition() const
		{
			return _position;
		}

		glm::vec3 getRotation() const
		{
			return _rotation;
		}

		glm::vec3 getScale() const
		{
			return _scale;
		}

		#pragma endregion

		#pragma region setters

		void setVertices(std::vector<Vertex> data)
		{
			_vertices = data;
			use();
			updateVBO();
		}

		void setIndexes(std::vector<unsigned int> data)
		{
			_indices = data;
			use();
			updateEBO();
		}

		void setScale(const glm::vec3& value)
		{
			_scale = value;
			_transformMatrix[0][0] = value.x;
			_transformMatrix[1][1] = value.y;
			_transformMatrix[2][2] = value.z;
		}

		void setPosition(const glm::vec3& newPosition)
		{
			glm::vec3 delta = newPosition - _position;
			translate(delta);
		}

		void setRotation(const glm::vec3& newRotation)
		{
			_transformMatrix = glm::mat4(1.0f);
			_transformMatrix = glm::scale(_transformMatrix, _scale);
			_transformMatrix = glm::translate(_transformMatrix, _position);

			glm::vec3 rotationRadians = glm::radians(newRotation);
			_transformMatrix = glm::rotate(_transformMatrix, rotationRadians.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
			_transformMatrix = glm::rotate(_transformMatrix, rotationRadians.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
			_transformMatrix = glm::rotate(_transformMatrix, rotationRadians.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around z-axis

			_rotation = newRotation;
			updateVBO();
		}

		#pragma endregion

		#pragma region actions
		void use()
		{
			glBindVertexArray(_VAO);
		}

		void draw()
		{
			use();
			glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, (void*)0);
		}

		void updateBuffers()
		{
			use();
			updateVBO();
			updateEBO();
		}

		void translate(const glm::vec3& offset)
		{
			_position += offset;
			_transformMatrix = glm::translate(glm::mat4(1.0f), offset) * _transformMatrix;
			updateVBO();
		}

		void rotate(const glm::vec3& axis, const float& degree)
		{
			float radians = glm::radians(degree);
			_rotation += radians * axis;
			_transformMatrix = glm::rotate(_transformMatrix, radians, axis);
			updateVBO();
		}

		void scale(const glm::vec3& value)
		{
			_scale *= value;
			_transformMatrix = glm::scale(_transformMatrix, value);
		}

		void paint(glm::vec4 color)
		{
			for (int i = 0; i < _vertices.size(); i++)
			{
				_vertices[i].color = color;
			}
			use();
			updateVBO();
		}

		#pragma endregion

	private:

		/// <summary>
		/// WARNING: This methods needs updating everytime the "Vertex" structure is changed
		/// </summary>
		void updateVertexAttributes()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0 * sizeof(float)));
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(float)));
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(float)));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
		}

		void updateVBO()
		{
			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);
		}

		void updateEBO()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);
		}
};

#endif