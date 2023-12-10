#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Color.hpp"

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void handleWindowResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

class Window
{
private:
    unsigned int width;
    unsigned int height;
    GLFWwindow* window;

public:
    GLFWwindow* getWindow()
    {
        return window;
    }

    Window(unsigned int width, unsigned int height, const char* name)
	{
        this->width = width;
        this->height = height;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, name, NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, handleWindowResize);
	}

    void clear(Color color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void update()
    {
        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
};

#endif