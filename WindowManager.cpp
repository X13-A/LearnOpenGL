#include "Commons.hpp"
#include "WindowManager.hpp"

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    InputManager::getInstance().handleMouseMovement(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    InputManager::getInstance().handleScroll(xoffset, yoffset);
}

GLFWwindow* WindowManager::getWindow()
{
    return window;
}

unsigned int WindowManager::getWidth() const
{
    return width;
}

unsigned int WindowManager::getHeight() const
{
    return height;
}

void WindowManager::init(unsigned int width, unsigned int height, const char* name)
{
    WindowManager& instance = getInstance();
    instance.width = width;
    instance.height = height;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    instance.window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (instance.window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(instance.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, width, height);
    glfwSetInputMode(instance.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(instance.window, windowResizeCallback);
    glfwSetCursorPosCallback(instance.window, mouseCallback);
    glfwSetScrollCallback(instance.window, scrollCallback);
}

void WindowManager::setTitle(const char* title)
{
    glfwSetWindowTitle(WindowManager::getInstance().getWindow(), title);
}

void WindowManager::clear(Color color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WindowManager::checkForEvents()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        close();
    }
}

void WindowManager::close()
{
    glfwSetWindowShouldClose(window, true);
}

void WindowManager::update()
{
    checkForEvents();
    glfwSwapBuffers(window);
    glfwPollEvents();
}