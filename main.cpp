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
#include "FrameBuffer.hpp"
#include "CloudNoiseGenerator.hpp"

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

    Camera camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 100000.0f, glm::vec3(0, 0, 5));
    CreativeControls* cameraControls = new CreativeControls(camera, 3.0f, 0.1f);
    InputManager::init(cameraControls);
    camera.translateGlobal(glm::vec3(0.5f, 0.5f, 0.5f));
    Model cat = Model("./models/Cat2.obj");
    cat.scale(glm::vec3(0.2f, 0.2f, 0.2f));
    cat.translate(glm::vec3(0, -0.5f, -3));

    Model cloud = Model("./models/Cube.obj");
    cloud.translate(glm::vec3(0.5f));
    cloud.scale(glm::vec3(100, 0.5, 100));
    FrameBuffer frameBuffer;
    frameBuffer.setup();

    CloudNoiseGenerator generator;
    GLuint points_n = 5;
    GLuint worleyRes = 128;
    std::vector<glm::vec3> worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPoints(points_n));
    GLuint worleyTexture = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
    
    Model pointModel = Model("./models/Sphere.obj");
    pointModel.scale(glm::vec3(0.05, 0.05, 0.05));

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(WindowManager::getInstance().getWindow()))
    {
        Color clearColor = Color(0.6, 0.6, 0.8, 1.0);
        //clearColor = Color(0, 0, 0, 1);
        Time::update();
        InputManager::getInstance().update();
        WindowManager::getInstance().clear(clearColor);  // Color(0.6, 0.6, 0.8, 1.0)
        cameraControls->update();

        if (InputManager::getInstance().isKeyPressed(KeyboardKey::G))
        {
            worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPointsGrid(3));
            worleyTexture = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
        }
        else if (InputManager::getInstance().isKeyPressed(KeyboardKey::F))
        {
            worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPoints(points_n));
            worleyTexture = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
        }

        frameBuffer.use();
        frameBuffer.setPass(0);
        setBackFaceCulling(true);

        WindowManager::getInstance().clear(clearColor);  // Color(0.6, 0.6, 0.8, 1.0)

        float rotateRadius = 3;
        light.transform.setPosition(glm::vec3(glm::sin(glfwGetTime()) * rotateRadius, glm::cos(glfwGetTime()) * rotateRadius, glm::sin(glfwGetTime()) * rotateRadius));
        light.setColor(glm::vec4((glm::sin(glfwGetTime()) + 1) / 2, (glm::sin(glfwGetTime() + 10) + 1) / 2, (glm::sin(glfwGetTime() + 20) + 1) / 2, 1.0f));

        for (const glm::vec3& point : worleyPoints)
        {
            pointModel.setPosition(point);
            texture1.use(GL_TEXTURE0);
            lightShader.use();
            lightShader.setInt("m_Texture", 0);
            lightShader.setVec4("lightColor", glm::vec4(0));
            if (point.x >= 0 && point.x <= 1 && point.y >= 0 && point.y <= 1 && point.z >= 0 && point.z <= 1)
            {
                lightShader.setVec4("ambientColor", WHITE);

            }
            else if (point.x < 0)
            {
                lightShader.setVec4("ambientColor", GREEN);
            }
            else
            {
                lightShader.setVec4("ambientColor", RED);
            }
            lightShader.setMat4("viewMatrix", camera.viewMatrix);
            lightShader.setMat4("projectionMatrix", camera.projectionMatrix);
            lightShader.setVec3("lightPos", light.transform.getPosition());
            lightShader.setVec3("viewPos", camera.getPosition());
            pointModel.draw(lightShader);
        }


        // Write to frameBuffer's second pass
        frameBuffer.setPass(1);

        setBackFaceCulling(false);
        cloudShader.use();
        cloudShader.setMat4("viewMatrix", camera.viewMatrix);
        cloudShader.setMat4("projectionMatrix", camera.projectionMatrix);
        cloudShader.setVec3("boundsMin", (cloud.getPosition() - cloud.getScale()));
        cloudShader.setVec3("boundsMax", (cloud.getPosition() + cloud.getScale()));
        cloudShader.setVec3("cameraPos", camera.getPosition());
        cloudShader.setInt("numSteps", 100);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.TCB);
        cloudShader.setInt("mainTex", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, worleyTexture);
        cloudShader.setInt("noiseTex", 1);
        cloud.draw(cloudShader);

        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Use base render target (window)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.TCB_secondPass); // Draw second pass texture
        frameBuffer.draw(screenShader);
        glEnable(GL_DEPTH_TEST);

        PerformanceTracker::getInstance().update();
        WindowManager::getInstance().update();
    }
    glfwTerminate();
    return 0;
}