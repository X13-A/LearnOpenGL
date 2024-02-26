#pragma once
#include "Commons.hpp"
#include "WindowManager.hpp"
#include "Shader.hpp"

class FrameBuffer
{
public:
    GLuint FBO;
    GLuint TCB; // Texture Color Buffer
    GLuint TCB_secondPass; // Texture Color Buffer
    GLuint RBO;
    GLuint screenVAO;
    GLuint screenVBO;

    const float screenVertices[6 * 4] =
    {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    void setup();
    void use();
    void setPass(short pass);
    void draw(Shader& shader);
};

