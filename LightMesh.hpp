#ifndef LIGHT_MESH_H
#define LIGHYT_MESH_H

#include "Mesh.hpp"

class LightMesh : public Mesh
{
	public:
		glm::vec4 lightColor;

		LightMesh(glm::vec4 lightColor, glm::vec3 position, glm::vec3 rotation, std::vector<Vertex> vertices, std::vector<unsigned int> indices) : Mesh(position, rotation, vertices, indices), lightColor(lightColor)
		{

		}
};

#endif