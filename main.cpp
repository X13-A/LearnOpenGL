#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Shader.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "Color.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "PerformanceTracker.hpp"
#include "Camera.hpp"
#include "stb_image.cpp"
#include "InputManager.hpp"
#include "Time.hpp"

/// <summary>
/// Use dedicated GPU
/// </summary>
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{
    Window window = Window(800, 600, "CustomEngine");
    PerformanceTracker performanceTracker;
    performanceTracker.verbose = true;
    performanceTracker.overrideTitle = true;

    #pragma region Vertices

    std::vector<Vertex> vertices =
    {
        Vertex(glm::vec4(0.5f,  0.5f, 0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec4(0.25f, -0.5f, 0.25f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec4(-0.25f, -0.5f, 0.25f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),

        Vertex(glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec4(0.25f, -0.5f, -0.25f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec4(-0.25f, -0.5f, -0.25f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
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
    Mesh mesh = Mesh(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, indices);
    Shader shader("shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    Texture texture1 = Texture("textures/fractal.jpg");
    Texture texture2 = Texture("textures/container.jpg");
    Camera camera = Camera(45, window.getWidth() / window.getHeight(), 0.1f, 100.0f, glm::vec3(0, 0, 3));
    InputManager inputManager;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        Time::update();
        window.clear(Color(0.6, 0.6, 0.8, 1.0));
        inputManager.retrieveInputs(window.getWindow());

        float cameraSpeed = 5.0f;
        if (inputManager.isKeyPressed(KeyboardKey::Space))
        {
            camera.translateLocal(glm::vec3(0, cameraSpeed * Time::deltaTime(), 0));
        }
        if (inputManager.isKeyPressed(KeyboardKey::LSHIFT))
        {
            camera.translateLocal(glm::vec3(0, -cameraSpeed * Time::deltaTime(), 0));
        }
        if (inputManager.isKeyPressed(KeyboardKey::A))
        {
            camera.translateLocal(glm::vec3(-cameraSpeed * Time::deltaTime(), 0, 0));
        }
        if (inputManager.isKeyPressed(KeyboardKey::D))
        {
            camera.translateLocal(glm::vec3(cameraSpeed * Time::deltaTime(), 0, 0));
        }
        if (inputManager.isKeyPressed(KeyboardKey::W))
        {
            camera.translateLocal(glm::vec3(0, 0, cameraSpeed * Time::deltaTime()));
        }
        if (inputManager.isKeyPressed(KeyboardKey::S))
        {
            camera.translateLocal(glm::vec3(0, 0, -cameraSpeed * Time::deltaTime()));
        }

        camera.setTarget(glm::vec3(0, 0, 0));
        camera.update();

        shader.use();
        shader.setInt("m_Texture1", 0);
        shader.setInt("m_Texture2", 1);
        shader.setMat4("modelMatrix", mesh.transformMatrix);
        shader.setMat4("viewMatrix", camera.viewMatrix);
        shader.setMat4("projectionMatrix", camera.projectionMatrix);
        texture1.use(GL_TEXTURE0);
        texture2.use(GL_TEXTURE1);

        mesh.draw();

        performanceTracker.update(window.getWindow());
        window.update();
    }
    glfwTerminate();
    return 0;
}