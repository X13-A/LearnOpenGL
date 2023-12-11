#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <iostream>

class Vertex
{
	public:
		glm::vec4 pos;
		glm::vec4 color;
		glm::vec2 texCoord;
		static const unsigned int dataSize = 10 * sizeof(float);

		Vertex(glm::vec4 pos, glm::vec4 color, glm::vec2 texCoord) : pos(pos), color(color), texCoord(texCoord)
		{
		}

		std::vector<float> data() const
		{
			return { pos.x, pos.y, pos.z, pos.w, color.r, color.g, color.b, color.a, texCoord.x, texCoord.y };
		}

		void translate(glm::vec3 offset)
		{
			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::translate(trans, offset);
			pos = trans * pos;
		}

		static std::vector<float> concatenatedData(std::vector<Vertex> vertices)
		{
			std::vector<float> concatenatedData;
			concatenatedData.reserve(vertices.size() * Vertex::dataSize);
			for (Vertex& vertex : vertices)
			{
				std::vector<float> vertexData = vertex.data();
				concatenatedData.insert(concatenatedData.end(), vertexData.begin(), vertexData.end());
			}

			return concatenatedData;
		}
};

#endif