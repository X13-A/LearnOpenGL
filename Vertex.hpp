#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

class Vertex
{
	public:
		glm::vec3 pos;
		glm::vec4 color;
		glm::vec2 texCoord;
		static const unsigned int dataSize = 9 * sizeof(float);
		Vertex(glm::vec3 pos, glm::vec4 color, glm::vec2 texCoord) : pos(pos), color(color), texCoord(texCoord)
		{
		}

		std::vector<float> data() const
		{
			return { pos.r, pos.g, pos.b, color.r, color.g, color.b, color.a, texCoord.r, texCoord.g };
		}

		static std::vector<float> concatenatedData(std::vector<Vertex> vertices)
		{
			std::vector<float> concatenatedData;
			concatenatedData.reserve(vertices.size() * Vertex::dataSize);

			for (const Vertex& vertex : vertices)
			{
				std::vector<float> vertexData = vertex.data();
				concatenatedData.insert(concatenatedData.end(), vertexData.begin(), vertexData.end());
			}

			return concatenatedData;
		}

};

#endif