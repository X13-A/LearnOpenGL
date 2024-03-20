#include "PostProcessLayer.hpp"

void PostProcessLayer::setup(Shader* shader, FrameBuffer* frameBuffer)
{
    _shader = shader;
    _frameBuffer = frameBuffer;

    // Setup screen quad
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PostProcessLayer::apply()
{
    glDisable(GL_DEPTH_TEST);
    _frameBuffer->storeRenderBuffer();
    _shader->use();

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->FBO); // Use base render target (window)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _frameBuffer->SampleBuffer);
    _shader->setInt("mainTex", 0);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}