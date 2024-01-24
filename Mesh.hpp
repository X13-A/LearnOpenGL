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
#include "Utils.hpp"
#include "Transform.hpp"
#include <memory>

class Mesh : public std::enable_shared_from_this<Mesh>
{
	private:
		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;
		unsigned int _VBO;
		unsigned int _EBO;
		unsigned int _VAO;

	public:
		Transform transform;
		friend class Transform;

		Mesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::vector<Vertex> vertices, std::vector<unsigned int> indices) : transform(Transform(position, rotation, scale)), _vertices(vertices), _indices(indices)
		{
			transform.attach(this);

			// Init Vertex, Indice and Vertex Array objects
			glGenBuffers(1, &_VBO);
			glGenBuffers(1, &_EBO);
			glGenVertexArrays(1, &_VAO);

			use();
			updateEBO();
			updateVBO();
			updateVertexAttributes();
		}

		Mesh(const Mesh& mesh) : transform(Transform(mesh.transform.getPosition(), mesh.transform.getRotation(), mesh.transform.getScale())), _vertices(mesh.getVertices()), _indices(mesh.getIndices())
		{
			transform.attach(this);
		}

		#pragma region getters

		const std::vector<Vertex>& getVertices() const
		{
			return _vertices;
		}

		const std::vector<unsigned int>& getIndices() const
		{
			return _indices;
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