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

void init()
{
    stbi_set_flip_vertically_on_load(true);
    WindowManager::init(800, 600, "CustomEngine");
    PerformanceTracker performanceTracker;
    PerformanceTracker::getInstance().verbose = false;
    PerformanceTracker::getInstance().overrideTitle = true;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main()
{
    init();

    Light light = Light(glm::vec3(), glm::vec4(0, 0, 1, 1));
    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");
    Shader lightShader("shaders/lit.vert", "shaders/lit.frag");
    Shader cloudShader("shaders/cloud.vert", "shaders/cloud.frag");
    Texture texture1 = Texture("textures/fractal.jpg");
    Texture texture2 = Texture("textures/container.jpg");
    Texture sunTexture = Texture("textures/sun.jpg");

    Camera camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 1000.0f, glm::vec3(0, 0, 5));
    CreativeControls* cameraControls = new CreativeControls(camera, 3.0f, 0.1f);
    InputManager::init(cameraControls);

    Model sun = Model("./models/Sphere.obj");
    sun.setScale(glm::vec3(0.25f, 0.25f, 0.25f));
    sun.translate(glm::vec3(-3, 0, 0));

    Model cat = Model("./models/Cat2.obj");
    cat.scale(glm::vec3(0.1f, 0.1f, 0.1f));
    cat.translate(glm::vec3(0, -0.5f, -3));

    Model cloud = Model("./models/Cube.obj");
    
    Model debug_minBound = Model("./models/Cube.obj");
    debug_minBound.setScale(glm::vec3(0.1f));
    debug_minBound.setPosition(cloud.getPosition() - (cloud.getScale()));

    Model debug_maxBound = Model("./models/Cube.obj");
    debug_maxBound.setScale(glm::vec3(0.1f));
    debug_maxBound.setPosition(cloud.getPosition() + (cloud.getScale()));

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(WindowManager::getInstance().getWindow()))
    {
        Time::update();
        InputManager::getInstance().update();
        WindowManager::getInstance().clear(Color(0, 0, 0, 1.0)); // Color(0.6, 0.6, 0.8, 1.0)
        float rotateRadius = 3;
        light.transform.setPosition(glm::vec3(glm::sin(glfwGetTime())* rotateRadius, glm::cos(glfwGetTime())* rotateRadius, glm::sin(glfwGetTime())* rotateRadius));
        light.setColor(glm::vec4((glm::sin(glfwGetTime()) + 1) / 2, (glm::sin(glfwGetTime() + 10) + 1) / 2, (glm::sin(glfwGetTime() + 20) + 1) / 2, 1.0f));

        //sun.setPosition(glm::vec3(glm::sin(glfwGetTime()) * rotateRadius, glm::cos(glfwGetTime())* rotateRadius, glm::sin(glfwGetTime())* rotateRadius));
        if (InputManager::getInstance().isKeyPressed(KeyboardKey::LCTRL))
        {
            control(sun, 5);
        }
        else
        {
            cameraControls->update();
        }

        texture1.use(GL_TEXTURE0);
        lightShader.use();
        lightShader.setInt("m_Texture", 0);
        lightShader.setVec4("lightColor", light.getColor());
        lightShader.setVec4("ambientColor", WHITE * 0.2f);
        lightShader.setMat4("viewMatrix", camera.viewMatrix);
        lightShader.setMat4("projectionMatrix", camera.projectionMatrix);
        lightShader.setVec3("lightPos", sun.getPosition());
        lightShader.setVec3("viewPos", camera.getPosition());
        //debug_minBound.draw(lightShader);
        //debug_maxBound.draw(lightShader);
        cat.draw(lightShader);

        //float caseSize = 20;
        //int gridSize = 10;
        //for (int x = 0; x < gridSize; x++)
        //{
        //    for (int y = 0; y < gridSize; y++)
        //    {
        //        for (int z = 0; z < gridSize; z++)
        //        {
        //            modelMatrix = glm::mat4(1);
        //            modelMatrix = glm::translate(modelMatrix, glm::vec3(x * caseSize, y * caseSize, z * caseSize));
        //            lightShader.setMat4("modelMatrix", modelMatrix);
        //            model.draw();
        //        }
        //    }
        //}

        sunShader.use();
        sunTexture.use(GL_TEXTURE0);
        sunShader.setMat4("viewMatrix", camera.viewMatrix);
        sunShader.setMat4("projectionMatrix", camera.projectionMatrix);
        //sun.draw(sunShader);

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE); // Disable depth write
        cloudShader.use();
        cloudShader.setMat4("viewMatrix", camera.viewMatrix);
        cloudShader.setMat4("projectionMatrix", camera.projectionMatrix);
        cloudShader.setVec3("boundsMin", cloud.getPosition() - cloud.getScale());
        cloudShader.setVec3("boundsMax", cloud.getPosition() + cloud.getScale());

        cloudShader.setVec3("cameraPos", camera.getPosition());
        cloud.draw(cloudShader);
        glDepthMask(GL_TRUE); // Disable depth write
        glDisable(GL_BLEND);

        PerformanceTracker::getInstance().update();
        WindowManager::getInstance().update();
    }
    glfwTerminate();
    return 0;
}