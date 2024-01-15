#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <iostream>

class Vertex
{
	public:
		glm::vec3 pos;
		glm::vec4 color;
		glm::vec2 texCoord;
		glm::vec3 normal;

		Vertex(glm::vec3 pos, glm::vec4 color, glm::vec2 texCoord, glm::vec3 normal) : pos(pos), color(color), texCoord(texCoord), normal(glm::normalize(normal))
		{
		}

		void translate(glm::vec3 offset)
		{
			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::translate(trans, offset);
			pos = trans * glm::vec4(pos, 1);
		}
};

#endif