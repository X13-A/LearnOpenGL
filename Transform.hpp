#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
class Mesh;

class Transform
{
private:
	glm::mat4 _transformMatrix;
	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 _scale;
	Mesh* _mesh;

public:
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

	const glm::mat4& getTransformMatrix() const;
	const glm::vec3& getPosition() const;
	const glm::vec3& getRotation() const;
	const glm::vec3& getScale() const;

	void setScale(const glm::vec3& value);
	void setPosition(const glm::vec3& newPosition);
	void setRotation(const glm::vec3& newRotation);

	void attach(Mesh* mesh);
	void translate(const glm::vec3& offset);
	void rotate(const glm::vec3& axis, const float& degree);
	void scale(const glm::vec3& value);
};


#endif