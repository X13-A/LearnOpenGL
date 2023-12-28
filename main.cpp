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
        mesh.translate(glm::vec3(0, moveSpeed * Time::deltaTime(), 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::LSHIFT))
    {
        mesh.translate(glm::vec3(0, -moveSpeed * Time::deltaTime(), 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::A))
    {
        mesh.translate(glm::vec3(-moveSpeed * Time::deltaTime(), 0, 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::D))
    {
        mesh.translate(glm::vec3(moveSpeed * Time::deltaTime(), 0, 0));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::W))
    {
        mesh.translate(glm::vec3(0, 0, moveSpeed * Time::deltaTime()));
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::S))
    {
        mesh.translate(glm::vec3(0, 0, -moveSpeed * Time::deltaTime()));
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

    std::vector<Vertex> vertices2 = {
        // Front face
        Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), BACKWARDS),
        Vertex(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), BACKWARDS),
        Vertex(glm::vec4(0.5f,  0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), BACKWARDS),
        Vertex(glm::vec4(0.5f,  0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), BACKWARDS),
        Vertex(glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), BACKWARDS),
        Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), BACKWARDS),

        // Back face
        Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), FORWARD),
        Vertex(glm::vec4(0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), FORWARD),
        Vertex(glm::vec4(0.5f,  0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), FORWARD),
        Vertex(glm::vec4(0.5f,  0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), FORWARD),
        Vertex(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), FORWARD),
        Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), FORWARD),

        // Left face
        Vertex(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), LEFT),
        Vertex(glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), LEFT),
        Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), LEFT),
        Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), LEFT),
        Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), LEFT),
        Vertex(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), LEFT),

        // Right face
        Vertex(glm::vec4(0.5f,  0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), RIGHT),
        Vertex(glm::vec4(0.5f,  0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), RIGHT),
        Vertex(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), RIGHT),
        Vertex(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), RIGHT),
        Vertex(glm::vec4(0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), RIGHT),
        Vertex(glm::vec4(0.5f,  0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), RIGHT),

        // Top face
        Vertex(glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), UP),
        Vertex(glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), UP),
        Vertex(glm::vec4(0.5f, 0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), UP),
        Vertex(glm::vec4(0.5f, 0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), UP),
        Vertex(glm::vec4(-0.5f, 0.5f,  0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), UP),
        Vertex(glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), UP),

        // Bottom face
        Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), DOWN),
        Vertex(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), DOWN),
        Vertex(glm::vec4(0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), DOWN),
        Vertex(glm::vec4(0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), DOWN),
        Vertex(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), DOWN),
        Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), DOWN)
    };
    std::vector<unsigned int> indices2 =
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

    std::vector<Vertex> vertices =
    {
        Vertex(glm::vec4(0.5f,  0.5f, 0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), FORWARD + UP + RIGHT),
        Vertex(glm::vec4(0.5f, -0.5f, 0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), FORWARD + DOWN + RIGHT),
        Vertex(glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), FORWARD + DOWN + LEFT),
        Vertex(glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), FORWARD + UP + LEFT),

        Vertex(glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 1.0f), BACKWARDS + UP + RIGHT),
        Vertex(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(1.0f, 0.0f), BACKWARDS + DOWN + RIGHT),
        Vertex(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 0.0f), BACKWARDS + DOWN + LEFT),
        Vertex(glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), WHITE, glm::vec2(0.0f, 1.0f), BACKWARDS + UP + LEFT),
    };
    std::vector<unsigned int> indices =
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

    std::vector<Mesh> meshes =
    {
        //Mesh(glm::vec3(-4.0f, 0.0f, 0.0f), glm::vec3(45.0f, 0.0f, 0.0f), vertices, indices),
        //Mesh(glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(90.0f, 0.0f, 0.0f), vertices, indices),
        //Mesh(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 45.0f, 0.0f), vertices, indices),
        //Mesh(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), vertices, indices),
        //Mesh(glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f), vertices, indices),
    };

    int n = 4;
    for (int i = 0; i < n; i++)
    {
        meshes.push_back(Mesh(glm::vec3(-n + i * 2, 0.0f, 0.0f), glm::vec3(5.0f * i, 10.0f * i, 15.0f * i), vertices, indices)); // 
    }
    meshes[0].paint(RED);
    meshes[1].paint(BLUE);
    meshes[2].paint(GREEN);
    meshes[3].paint(WHITE);

    LightMesh sun = LightMesh(WHITE, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, indices);
    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");
    Shader lightShader("shaders/lit.vert", "shaders/lit.frag");

    Texture texture1 = Texture("textures/fractal.jpg");
    Texture texture2 = Texture("textures/container.jpg");
    Texture sunTexture = Texture("textures/sun.jpg");

    Camera camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 100.0f, glm::vec3(0, 0, 3));
    CreativeControls* cameraControls = new CreativeControls(camera, 3.0f, 0.1f);
    InputManager::init(cameraControls);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(WindowManager::getInstance().getWindow()))
    {
        Time::update();
        InputManager::getInstance().update();
        WindowManager::getInstance().clear(Color(0, 0, 0, 0)); // Color(0.6, 0.6, 0.8, 1.0)
        float rotateRadius = 6;
        //sun.setPosition(glm::vec3(glm::sin(glfwGetTime()) * rotateRadius, sun.position.y, sun.position.z));
        //sun.setPosition(glm::vec3(sun.position.x, glm::cos(glfwGetTime()) * rotateRadius, sun.position.z));
        //sun.setPosition(glm::vec3(sun.position.x, sun.position.y, glm::sin(glfwGetTime()) * rotateRadius));
        sun.lightColor = glm::vec4((glm::sin(glfwGetTime()) + 1) / 2, (glm::sin(glfwGetTime() + 10) + 1) / 2, (glm::sin(glfwGetTime() + 20) + 1) / 2, 1.0f);
        sun.paint(sun.lightColor);
        control(sun, 5);
        //cameraControls->update();

        lightShader.use();
        lightShader.setInt("m_Texture", 0);
        lightShader.setVec4("lightColor", sun.lightColor);
        lightShader.setVec4("ambientColor", WHITE * 0.0f);
        lightShader.setMat4("modelMatrix", sun.transformMatrix);
        lightShader.setMat4("viewMatrix", camera.viewMatrix);
        lightShader.setMat4("projectionMatrix", camera.projectionMatrix);
        lightShader.setVec3("lightPos", sun.position);
        texture1.use(GL_TEXTURE0);

        for (Mesh mesh : meshes)
        {
            lightShader.setMat4("modelMatrix", mesh.transformMatrix);
            mesh.draw();
        }

        sunTexture.use(GL_TEXTURE0);
        sunShader.use();
        sunShader.setMat4("modelMatrix", sun.transformMatrix);
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