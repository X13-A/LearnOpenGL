#ifndef CLOUD_NOISE_GENERATOR_HPP
#define CLOUD_NOISE_GENERATOR_HPP

#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include "Utils.hpp"
#include "ComputeShader.hpp"

static const char* WORLEY_COMPUTE_SHADER_PATH = "./shaders/compute_worley.glsl";

class CloudNoiseGenerator 
{
private:
    GLuint worleyTexture;

public:
    std::vector<glm::vec3> CreateWorleyPointsGrid(int gridSize);
    std::vector<glm::vec3> CreateWorleyPoints(int n);
    std::vector<glm::vec3> RepeatWorleyPoints(const std::vector<glm::vec3>& points);
    GLuint ComputeWorleyTexture(std::vector <glm::vec3> worleyPoints, GLuint textureSize = 128);
};

#endif