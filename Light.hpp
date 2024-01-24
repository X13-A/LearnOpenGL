#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include "Transform.hpp"

class Light
{
private:
	glm::vec4 _color;

public:
	Transform transform;
	Light(glm::vec3 pos, glm::vec4 color);

	glm::vec4 getColor() const;
	void setColor(glm::vec4 value);
};

#endif