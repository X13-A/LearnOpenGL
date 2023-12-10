#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.hpp"
#include "Texture.hpp"
#include "stb_image.cpp"
#include "Window.hpp"
#include "Color.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"

int main()
{
    Window window = Window(800, 600, "CustomEngine");
    #pragma region Vertices

    std::vector<Vertex> vertices =
    {
        Vertex(glm::vec3(0.5f,  0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
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

        shader.use();
        shader.setFloat("time", glfwGetTime());
        shader.setInt("m_Texture1", 0);
        shader.setInt("m_Texture2", 1);

        texture1.use(GL_TEXTURE0);
        texture2.use(GL_TEXTURE1);

        mesh.use();
        mesh.vertices[0].pos.x = glfwGetTime() / 10;
        mesh.vertices[3].pos.x = -glfwGetTime() / 10;
        mesh.updateBuffers();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        window.update();
    }
    glfwTerminate();
    return 0;
}