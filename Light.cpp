#include "Light.hpp"
#include <glm/glm.hpp>
#include "Transform.hpp"

Light::Light(glm::vec3 pos, glm::vec4 color) : _color(color), transform(Transform(pos, glm::vec3(), glm::vec3()))
{

}

glm::vec4 Light::getColor() const
{
	return _color;
}
void Light::setColor(glm::vec4 value)
{;
	_color = value;
};