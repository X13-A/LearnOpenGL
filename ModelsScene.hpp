#pragma once

#include "Scene.hpp"
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

class ModelsScene : Scene
{
private:
    // Models
    Model catModel;

    // Rendering
    Camera camera;
    FrameBuffer frameBuffer;
    Color clearColor = Color(0, 0, 0, 1.0);

    // Shaders
    Shader* screenShader;
    Shader* lightShader;

    // Inputs
    CreativeControls* cameraControls;
    float cameraSpeed = 30;

    // Lighting
    glm::vec4 ambientColor = glm::vec4(0.2, 0.2, 0.2, 1);

public:
    ModelsScene()
    {
        setup();
    }

    void setup() override
    {
        // Models
        catModel.loadModel("./models/Cat.obj");
        catModel.setScale(glm::vec3(1));
        catModel.setPosition(glm::vec3(0));
        // Framebuffer
        frameBuffer.setup();

        // Shaders
        screenShader = new Shader("shaders/screen.vert", "shaders/screen.frag");
        lightShader = new Shader("shaders/lit.vert", "shaders/lit.frag");

        // Camera
        camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 100000.0f, glm::vec3(0, 0, 5));
        camera.translateGlobal(glm::vec3(0.0f, 0.0f, 3.0f));

        // Inputs
        cameraControls = new CreativeControls(camera, cameraSpeed, 0.1f);
        InputManager::init(cameraControls);
    }

    void update() override
    {
        Time::update();
        InputManager::getInstance().update();
        cameraControls->update();
    }

    void lateUpdate() override
    {
        PerformanceTracker::getInstance().update();
        WindowManager::getInstance().update();
    }

    void draw() override
    {
        frameBuffer.use();
        frameBuffer.setPass(0);

        WindowManager::getInstance().clear(clearColor);
        glm::vec3 lightPos = glm::vec3(0, 20, 0);

        lightShader->use();
        lightShader->setVec4("lightColor", glm::vec4(1));
        lightShader->setMat4("viewMatrix", camera.viewMatrix);
        lightShader->setMat4("projectionMatrix", camera.projectionMatrix);
        lightShader->setVec3("lightPos", lightPos);
        lightShader->setVec3("viewPos", camera.getPosition());
        lightShader->setVec4("ambientColor", ambientColor);
        catModel.draw(*lightShader);

        // Write to frameBuffer's second pass
        frameBuffer.setPass(1);
        frameBuffer.drawToWindow(*screenShader);
    }
};

