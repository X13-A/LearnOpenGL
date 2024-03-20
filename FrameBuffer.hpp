#pragma once
#include "Commons.hpp"
#include "WindowManager.hpp"
#include "Shader.hpp"

class FrameBuffer
{
public:
    GLuint FBO; // Frame Buffer Object
    GLuint SampleBuffer; // Texture Color Buffer (TCB)
    GLuint RenderBuffer; // Texture Color Buffer (TCB)
    GLuint RBO; // Render Buffer Object

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
    void storeRenderBuffer();
    void drawToWindow(Shader& shader);
};

