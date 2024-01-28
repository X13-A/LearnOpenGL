#include "Commons.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Shader.hpp"
#include "Texture.hpp"
#include "WindowManager.hpp"
#include "Color.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "PerformanceTracker.hpp"
#include "Camera.hpp"
#include "stb_image.cpp"
#include "InputManager.hpp"
#include "Time.hpp"
#include "OrbitControls.hpp"
#include "CreativeControls.hpp"
#include "Model.hpp"
#include "Light.hpp"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include "Utils.hpp"

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
/// <summary>
/// Use dedicated GPU
/// </summary>
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

void control(Model& model, float moveSpeed)
{
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::Space))
    {
        model.translate(glm::vec3(0, moveSpeed * Time::deltaTime(), 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::LSHIFT))
    {
        model.translate(glm::vec3(0, -moveSpeed * Time::deltaTime(), 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::A))
    {
        model.translate(glm::vec3(-moveSpeed * Time::deltaTime(), 0, 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::D))
    {
        model.translate(glm::vec3(moveSpeed * Time::deltaTime(), 0, 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::W))
    {
        model.translate(glm::vec3(0, 0, moveSpeed * Time::deltaTime()));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::S))
    {
        model.translate(glm::vec3(0, 0, -moveSpeed * Time::deltaTime()));
    }
}

void setBackFaceCulling(bool value)
{
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    if (value == true)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void init()
{
    stbi_set_flip_vertically_on_load(true);
    WindowManager::init(800, 600, "CustomEngine");
    PerformanceTracker performanceTracker;
    PerformanceTracker::getInstance().verbose = false;
    PerformanceTracker::getInstance().overrideTitle = true;
}

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

        void setup()
        {
            unsigned int screenWidth = WindowManager::getInstance().getWidth();
            unsigned int screenHeight = WindowManager::getInstance().getHeight();

            // Create frame buffer object
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // Create render texture
            glGenTextures(1, &TCB);
            glBindTexture(GL_TEXTURE_2D, TCB);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Create second pass render texture
            glGenTextures(1, &TCB_secondPass);
            glBindTexture(GL_TEXTURE_2D, TCB_secondPass);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Attach texture to framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TCB, 0);

            // Renderbuffer for depth and stencil
            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Attach renderbuffer to framebuffer
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            // Check if FBO is complete
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
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

        void use()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        }

        void setPass(short pass)
        {
            if (pass == 0)
            {
                glBindTexture(GL_TEXTURE_2D, TCB);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TCB, 0);
            }
            else
            {
                // Before switching to second pass, copy the content of the first texture to the second
                glBindTexture(GL_TEXTURE_2D, TCB_secondPass);
                glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, WindowManager::getInstance().getWidth(), WindowManager::getInstance().getHeight(), 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TCB_secondPass, 0);
            }
        }

        void draw(Shader& shader)
        {
            shader.use();
            glActiveTexture(GL_TEXTURE0);
            shader.setInt("mainTex", 0);

            glBindVertexArray(screenVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
};

int main()
{
    init();

    Light light = Light(glm::vec3(), glm::vec4(0, 0, 1, 1));

    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");
    Shader lightShader("shaders/lit.vert", "shaders/lit.frag");
    Shader cloudShader("shaders/cloud.vert", "shaders/cloud.frag");
    Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
    
    Texture texture1 = Texture("textures/fractal.jpg");
    Texture texture2 = Texture("textures/container.jpg");
    Texture sunTexture = Texture("textures/sun.jpg");

    Camera camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 100.0f, glm::vec3(0, 0, 5));
    CreativeControls* cameraControls = new CreativeControls(camera, 3.0f, 0.1f);
    InputManager::init(cameraControls);

    Model cat = Model("./models/Cat2.obj");
    cat.scale(glm::vec3(0.2f, 0.2f, 0.2f));
    cat.translate(glm::vec3(0, -0.5f, -3));

    Model cloud = Model("./models/Cube.obj");
    
    FrameBuffer frameBuffer;
    frameBuffer.setup();

    glEnable(GL_DEPTH_TEST);
    cloud.setRotation(glm::vec3(-90, 0, 0));
    while (!glfwWindowShouldClose(WindowManager::getInstance().getWindow()))
    {
        Time::update();
        InputManager::getInstance().update();
        WindowManager::getInstance().clear(Color(0, 0, 0, 1.0));  // Color(0.6, 0.6, 0.8, 1.0)
        cameraControls->update();

        frameBuffer.use();
        frameBuffer.setPass(0);
        setBackFaceCulling(true);

        WindowManager::getInstance().clear(Color(0, 0, 0, 1.0));  // Color(0.6, 0.6, 0.8, 1.0)
        
        float rotateRadius = 3;
        light.transform.setPosition(glm::vec3(glm::sin(glfwGetTime())* rotateRadius, glm::cos(glfwGetTime())* rotateRadius, glm::sin(glfwGetTime())* rotateRadius));
        light.setColor(glm::vec4((glm::sin(glfwGetTime()) + 1) / 2, (glm::sin(glfwGetTime() + 10) + 1) / 2, (glm::sin(glfwGetTime() + 20) + 1) / 2, 1.0f));

        texture1.use(GL_TEXTURE0);
        lightShader.use();
        lightShader.setInt("m_Texture", 0);
        lightShader.setVec4("lightColor", light.getColor());
        lightShader.setVec4("ambientColor", WHITE * 0.2f);
        lightShader.setMat4("viewMatrix", camera.viewMatrix);
        lightShader.setMat4("projectionMatrix", camera.projectionMatrix);
        lightShader.setVec3("lightPos", light.transform.getPosition());
        lightShader.setVec3("viewPos", camera.getPosition());
        cat.draw(lightShader);

        // Write to frameBuffer's second pass
        frameBuffer.setPass(1);

        setBackFaceCulling(false);
        cloudShader.use();
        cloudShader.setMat4("viewMatrix", camera.viewMatrix);
        cloudShader.setMat4("projectionMatrix", camera.projectionMatrix);
        cloudShader.setVec3("boundsMin", (cloud.getPosition() - cloud.getScale()));
        cloudShader.setVec3("boundsMax", (cloud.getPosition() + cloud.getScale()));
        cloudShader.setVec3("cameraPos", camera.getPosition());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.TCB);
        cloudShader.setInt("mainTex", 0);
        cloud.draw(cloudShader);

        //Use base render target (window)
        frameBuffer.setPass(1);

        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        frameBuffer.draw(screenShader);
        glEnable(GL_DEPTH_TEST);

        PerformanceTracker::getInstance().update();
        WindowManager::getInstance().update();
    }
    glfwTerminate();
    return 0;
}