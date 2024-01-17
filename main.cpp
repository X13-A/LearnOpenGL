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
#include "LightMesh.hpp"
#include "Model.hpp"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

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

void control(Mesh& mesh, float moveSpeed)
{
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::Space))
    {
        mesh.transform.translate(glm::vec3(0, moveSpeed * Time::deltaTime(), 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::LSHIFT))
    {
        mesh.transform.translate(glm::vec3(0, -moveSpeed * Time::deltaTime(), 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::A))
    {
        mesh.transform.translate(glm::vec3(-moveSpeed * Time::deltaTime(), 0, 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::D))
    {
        mesh.transform.translate(glm::vec3(moveSpeed * Time::deltaTime(), 0, 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::W))
    {
        mesh.transform.translate(glm::vec3(0, 0, moveSpeed * Time::deltaTime()));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::S))
    {
        mesh.transform.translate(glm::vec3(0, 0, -moveSpeed * Time::deltaTime()));
    }
}

int main()
{
    stbi_set_flip_vertically_on_load(true);

    WindowManager::init(800, 600, "CustomEngine");
    PerformanceTracker performanceTracker;
    PerformanceTracker::getInstance().verbose = false;
    PerformanceTracker::getInstance().overrideTitle = true;

#pragma region Vertices

    std::vector<Vertex> vertices = {
        // Front face
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 0.0f), BACKWARDS),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), WHITE, glm::vec2(1.0f, 0.0f), BACKWARDS),
        Vertex(glm::vec3(0.5f,  0.5f, -0.5f), WHITE, glm::vec2(1.0f, 1.0f), BACKWARDS),
        Vertex(glm::vec3(0.5f,  0.5f, -0.5f), WHITE, glm::vec2(1.0f, 1.0f), BACKWARDS),
        Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), BACKWARDS),
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 0.0f), BACKWARDS),
                       
        // Back face   3
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), WHITE, glm::vec2(0.0f, 0.0f), FORWARD),
        Vertex(glm::vec3(0.5f, -0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), FORWARD),
        Vertex(glm::vec3(0.5f,  0.5f,  0.5f), WHITE, glm::vec2(1.0f, 1.0f), FORWARD),
        Vertex(glm::vec3(0.5f,  0.5f,  0.5f), WHITE, glm::vec2(1.0f, 1.0f), FORWARD),
        Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), WHITE, glm::vec2(0.0f, 1.0f), FORWARD),
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), WHITE, glm::vec2(0.0f, 0.0f), FORWARD),

        // Left face
        Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), LEFT),
        Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), WHITE, glm::vec2(1.0f, 1.0f), LEFT),
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), LEFT),
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), LEFT),
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), WHITE, glm::vec2(0.0f, 0.0f), LEFT),
        Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), LEFT),

        // Right face
        Vertex(glm::vec3(0.5f,  0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), RIGHT),
        Vertex(glm::vec3(0.5f,  0.5f, -0.5f), WHITE, glm::vec2(1.0f, 1.0f), RIGHT),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), RIGHT),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), RIGHT),
        Vertex(glm::vec3(0.5f, -0.5f,  0.5f), WHITE, glm::vec2(0.0f, 0.0f), RIGHT),
        Vertex(glm::vec3(0.5f,  0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), RIGHT),
                       
        // Top face    
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), UP),
        Vertex(glm::vec3(0.5f, 0.5f, -0.5f), WHITE, glm::vec2(1.0f, 1.0f), UP),
        Vertex(glm::vec3(0.5f, 0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), UP),
        Vertex(glm::vec3(0.5f, 0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), UP),
        Vertex(glm::vec3(-0.5f, 0.5f,  0.5f), WHITE, glm::vec2(0.0f, 0.0f), UP),
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), UP),
                       
        // Bottom face 
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), DOWN),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), WHITE, glm::vec2(1.0f, 1.0f), DOWN),
        Vertex(glm::vec3(0.5f, -0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), DOWN),
        Vertex(glm::vec3(0.5f, -0.5f,  0.5f), WHITE, glm::vec2(1.0f, 0.0f), DOWN),
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), WHITE, glm::vec2(0.0f, 0.0f), DOWN),
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), DOWN)
    };
    std::vector<unsigned int> indices =
    {
        // Front face
        0, 1, 2,
        3, 4, 5,

        // Back face
        6, 7, 8,
        9, 10, 11,

        // Top face
        12, 13, 14,
        15, 16, 17,

        // Bottom face
        18, 19, 20,
        21, 22, 23,

        // Right face
        24, 25, 26,
        27, 28, 29,

        // Left face
        30, 31, 32,
        33, 34, 35,
    };

    std::vector<Vertex> vertices2 =
    {
        Vertex(glm::vec3(0.5f,  0.5f, 0.5f), WHITE, glm::vec2(1.0f, 1.0f), FORWARD + UP + RIGHT),
        Vertex(glm::vec3(0.5f, -0.5f, 0.5f), WHITE, glm::vec2(1.0f, 0.0f), FORWARD + DOWN + RIGHT),
        Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), WHITE, glm::vec2(0.0f, 0.0f), FORWARD + DOWN + LEFT),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), WHITE, glm::vec2(0.0f, 1.0f), FORWARD + UP + LEFT),
                       
        Vertex(glm::vec3(0.5f, 0.5f, -0.5f), WHITE, glm::vec2(1.0f, 1.0f), BACKWARDS + UP + RIGHT),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), WHITE, glm::vec2(1.0f, 0.0f), BACKWARDS + DOWN + RIGHT),
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), WHITE, glm::vec2(0.0f, 0.0f), BACKWARDS + DOWN + LEFT),
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), WHITE, glm::vec2(0.0f, 1.0f), BACKWARDS + UP + LEFT),
    };
    std::vector<unsigned int> indices2 =
    {
        // Front face
        0, 3, 1,
        1, 3, 2,

        // Back face
        5, 6, 7,
        7, 4, 5,

        // Top face
        0, 4, 7,
        7, 3, 0,

        // Bottom face
        1, 6, 5,
        1, 2, 6,

        // Right face
        5, 4, 0,
        0, 1, 5,

        // Left face
        3, 7, 6,
        6, 2, 3,
    };

#pragma endregion

    LightMesh sun = LightMesh(WHITE, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1), vertices, indices);
    sun.transform.scale(glm::vec3(0.25f, 0.25f, 0.25f));
    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");
    Shader lightShader("shaders/lit.vert", "shaders/lit.frag");

    Texture texture1 = Texture("textures/fractal.jpg");
    Texture texture2 = Texture("textures/container.jpg");
    Texture sunTexture = Texture("textures/sun.jpg");

    Camera camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 1000.0f, glm::vec3(0, 0, 3));
    CreativeControls* cameraControls = new CreativeControls(camera, 3.0f, 0.1f);
    InputManager::init(cameraControls);

    Model model = Model("./models/Cat2.obj");
    
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(WindowManager::getInstance().getWindow()))
    {
        Time::update();
        InputManager::getInstance().update();
        WindowManager::getInstance().clear(Color(0, 0, 0, 0)); // Color(0.6, 0.6, 0.8, 1.0)
        float rotateRadius = 3;
        sun.transform.setPosition(glm::vec3(glm::sin(glfwGetTime()) * rotateRadius, sun.transform.getPosition().y, sun.transform.getPosition().z));
        sun.transform.setPosition(glm::vec3(sun.transform.getPosition().x, glm::cos(glfwGetTime())* rotateRadius, sun.transform.getPosition().z));
        sun.transform.setPosition(glm::vec3(sun.transform.getPosition().x, sun.transform.getPosition().y, glm::sin(glfwGetTime()) * rotateRadius));
        sun.setLightColor(glm::vec4((glm::sin(glfwGetTime()) + 1) / 2, (glm::sin(glfwGetTime() + 10) + 1) / 2, (glm::sin(glfwGetTime() + 20) + 1) / 2, 1.0f));
        sun.paint(sun.getLightColor());

        //control(sun, 5);
        cameraControls->update();

        texture1.use(GL_TEXTURE0);
        lightShader.use();
        lightShader.setInt("m_Texture", 0);
        lightShader.setVec4("lightColor", sun.getLightColor());
        lightShader.setVec4("ambientColor", WHITE * 0.2f);
        lightShader.setMat4("viewMatrix", camera.viewMatrix);
        lightShader.setMat4("projectionMatrix", camera.projectionMatrix);
        lightShader.setVec3("lightPos", sun.transform.getPosition());
        lightShader.setVec3("viewPos", camera.getPosition());
        model.draw(lightShader);

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

        sunTexture.use(GL_TEXTURE0);
        sunShader.use();
        sunShader.setMat4("modelMatrix", sun.transform.getTransformMatrix());
        sunShader.setMat4("viewMatrix", camera.viewMatrix);
        sunShader.setMat4("projectionMatrix", camera.projectionMatrix);
        sunShader.setInt("m_Texture", 0);
        sun.draw();

        PerformanceTracker::getInstance().update();
        WindowManager::getInstance().update();
    }
    glfwTerminate();
    return 0;
}