#pragma once
#include "Commons.hpp"
#include "WindowManager.hpp"
#include "Shader.hpp"
#include "FrameBuffer.hpp"

class PostProcessLayer
{
private:
	Shader* _shader;
	FrameBuffer* _frameBuffer;

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

public:
    void setup(Shader* shader, FrameBuffer* frameBuffer);
	void apply();
};

