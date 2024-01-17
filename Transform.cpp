#include "Transform.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Mesh.hpp"

Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : _transformMatrix(glm::mat4(1.0f)), _position(position), _rotation(rotation), _scale(scale)
{
	setPosition(position);
	setRotation(rotation);
	setScale(scale);
}

#pragma region Getters

const glm::mat4& Transform::getTransformMatrix() const
{
	return _transformMatrix;
}

const glm::vec3& Transform::getPosition() const
{
	return _position;
}

const glm::vec3& Transform::getRotation() const
{
	return _rotation;
}

const glm::vec3& Transform::getScale() const
{
	return _scale;
}

#pragma endregion

#pragma region Setters

void Transform::setScale(const glm::vec3& value)
{
	_scale = value;
	_transformMatrix[0][0] = value.x;
	_transformMatrix[1][1] = value.y;
	_transformMatrix[2][2] = value.z;
	if (_mesh != nullptr) _mesh->updateVBO();
}

void Transform::setPosition(const glm::vec3& newPosition)
{
	glm::vec3 delta = newPosition - _position;
	translate(delta);
}

void Transform::setRotation(const glm::vec3& newRotation)
{
	_transformMatrix = glm::mat4(1.0f);
	_transformMatrix = glm::scale(_transformMatrix, _scale);
	_transformMatrix = glm::translate(_transformMatrix, _position);

	glm::vec3 rotationRadians = glm::radians(newRotation);
	_transformMatrix = glm::rotate(_transformMatrix, rotationRadians.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
	_transformMatrix = glm::rotate(_transformMatrix, rotationRadians.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
	_transformMatrix = glm::rotate(_transformMatrix, rotationRadians.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around z-axis

	_rotation = newRotation;
	if (_mesh != nullptr) _mesh->updateVBO();
}

#pragma endregion

#pragma region Actions

void Transform::attach(Mesh* mesh)
{
	_mesh = mesh;
}

void Transform::translate(const glm::vec3& offset)
{
	_position += offset;
	_transformMatrix = glm::translate(glm::mat4(1.0f), offset) * _transformMatrix;
	if (_mesh != nullptr) _mesh->updateVBO();
}

void Transform::rotate(const glm::vec3& axis, const float& degree)
{
	float radians = glm::radians(degree);
	_rotation += radians * axis;
	_transformMatrix = glm::rotate(_transformMatrix, radians, axis);
	if (_mesh != nullptr) _mesh->updateVBO();
}

void Transform::scale(const glm::vec3& value)
{
	_scale *= value;
	_transformMatrix = glm::scale(_transformMatrix, value);
	if (_mesh != nullptr) _mesh->updateVBO();
}

#pragma endregion
