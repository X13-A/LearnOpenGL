#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Shader.hpp"
#include "Texture.hpp"
#include "stb_image.cpp"
#include "Window.hpp"
#include "Color.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "performanceTracker.hpp"

/// <summary>
/// Use NVDIA GPU
/// </summary>
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
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
        Vertex(glm::vec4(0.5f,  0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
    };

    std::vector<unsigned int> indices = 
    {
        0, 1, 3,
        1, 2, 3 
    };
    
    #pragma endregion
    Mesh mesh = Mesh(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, indices);
    Shader shader("shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    Texture texture1 = Texture("textures/fractal.jpg");
    Texture texture2 = Texture("textures/container.jpg");

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        window.clear(Color(0.6, 0.6, 0.8, 1.0));

        mesh.setRotation(glm::vec3(0, 0, sin(glfwGetTime()) * 180));
        mesh.setPosition(glm::vec3(glm::sin(glfwGetTime()), glm::cos(glfwGetTime()), 0.0f));

        shader.use();
        shader.setFloat("time", glfwGetTime());
        shader.setInt("m_Texture1", 0);
        shader.setInt("m_Texture2", 1);
        shader.setMat4("transform", mesh.transform);
        texture1.use(GL_TEXTURE0);
        texture2.use(GL_TEXTURE1);


        mesh.draw();

        performanceTracker.update(window.getWindow());
        window.update();
    }
    glfwTerminate();
    return 0;
}