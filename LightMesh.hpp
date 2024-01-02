#ifndef LIGHT_MESH_H
#define LIGHYT_MESH_H

#include "Mesh.hpp"

class LightMesh : public Mesh
{
	private:
		glm::vec4 _lightColor;
	
	public:
		LightMesh(glm::vec4 lightColor, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::vector<Vertex> vertices, std::vector<unsigned int> indices) : Mesh(position, rotation, scale, vertices, indices), _lightColor(lightColor)
		{

		}

		glm::vec4 getLightColor()
		{
			return _lightColor;
		}

		void setLightColor(const glm::vec4& value)
		{
			_lightColor = value;
		}
};

#endif