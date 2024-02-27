#ifndef COLOR_H
#define COLOR_H

#include <glm/glm.hpp>

static const glm::vec4 WHITE = glm::vec4(1, 1, 1, 1);
static const glm::vec4 RED = glm::vec4(1, 0, 0, 1);
static const glm::vec4 GREEN = glm::vec4(0, 1, 0, 1);
static const glm::vec4 BLUE = glm::vec4(0, 0, 1, 1);
static const glm::vec4 BLACK = glm::vec4(0, 0, 0, 1);
static const glm::vec4 TRANSPARENT = glm::vec4(0, 0, 0, 0);
static const glm::vec4 AMBIENT_LIGHT = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

static const glm::vec3 LEFT = glm::vec3(-1, 0, 0);
static const glm::vec3 RIGHT = glm::vec3(1, 0, 0);
static const glm::vec3 UP = glm::vec3(0, 1, 0);
static const glm::vec3 DOWN = glm::vec3(0, -1, 0);
static const glm::vec3 FORWARD = glm::vec3(0, 0, 1);
static const glm::vec3 BACKWARDS = glm::vec3(0, 0, -1);

class Color
{
public:
	float r;
	float g;
	float b;
	float a;

	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

#endif