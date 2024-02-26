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
    std::vector<glm::vec3> CreateWorleyPointsGrid(int gridSize)
    {
        std::vector<glm::vec3> points;
        points.reserve(gridSize * gridSize * gridSize);

        float cellSize = 1.0f / gridSize;
        for (size_t x = 0; x < gridSize; x++)
        {
            for (size_t y = 0; y < gridSize; y++)
            {
                for (size_t z = 0; z < gridSize; z++)
                {
                    float randomX = Utils::randomFloat(0, 1);
                    float randomY = Utils::randomFloat(0, 1);
                    float randomZ = Utils::randomFloat(0, 1);
                    glm::vec3 randomOffset = glm::vec3(randomX, randomY, randomZ) * cellSize;
                    glm::vec3 cellCorner = glm::vec3(x, y, z) * cellSize;
                    glm::vec3 point = cellCorner + randomOffset;
                    points.push_back(point);
                }
            }
        }
        return points;
    }

    std::vector<glm::vec3> CreateWorleyPoints(int n)
    {
        std::vector<glm::vec3> points;
        points.reserve(n);

        for (int i = 0; i < n; i++)
        {
            float randomX = Utils::randomFloat(0, 1);
            float randomY = Utils::randomFloat(0, 1);
            float randomZ = Utils::randomFloat(0, 1);
            points.push_back(glm::vec3(randomX, randomY, randomZ));
        }
        return points;
    }

    std::vector<glm::vec3> RepeatWorleyPoints(const std::vector<glm::vec3>& points)
    {
        std::vector<glm::vec3> repeatedPoints;
        std::size_t size = points.size() * points.size() * points.size();
        repeatedPoints.reserve(size);

        for (size_t x = 0; x < 3; x++)
        {
            for (size_t y = 0; y < 3; y++)
            {
                for (size_t z = 0; z < 3; z++)
                {
                    glm::vec3 offset = glm::vec3(x, y, z);

                    for (size_t i = 0; i < points.size(); i++)
                    {
                        repeatedPoints.push_back((points[i] + offset));
                    }
                }
            }
        }

        for (glm::vec3& point : repeatedPoints)
        {
            point -= 3.0f/2.0f;
        }

        return repeatedPoints;
    }

    GLuint ComputeWorleyTexture(std::vector <glm::vec3> worleyPoints, GLuint textureSize = 128)
    {
        ComputeShader computeShader = ComputeShader(WORLEY_COMPUTE_SHADER_PATH);

        // Create shader storage buffer
        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, worleyPoints.size() * sizeof(glm::vec3), worleyPoints.data(), GL_STATIC_DRAW);

        // Create texture
        glGenTextures(1, &worleyTexture);
        glBindTexture(GL_TEXTURE_3D, worleyTexture);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, textureSize, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Set texture wrapping parameters
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
        //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        GLuint program = glCreateProgram();

        glAttachShader(program, computeShader.ID);
        glLinkProgram(program);
        glUseProgram(program);

        GLint textureDimensionsLocation = glGetUniformLocation(program, "textureDimensions");
        glUniform3i(textureDimensionsLocation, textureSize, textureSize, textureSize);

        GLint pointsBufferLengthLocation = glGetUniformLocation(program, "pointsBufferLength");
        glUniform1i(pointsBufferLengthLocation, worleyPoints.size());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        glBindImageTexture(1, worleyTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        glDispatchCompute((GLuint) ceil(textureSize /4), (GLuint)ceil(textureSize / 4), (GLuint)ceil(textureSize / 4));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindTexture(GL_TEXTURE_3D, 0);
        return worleyTexture;
    }
};

#endif