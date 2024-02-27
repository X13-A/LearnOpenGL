#include "Commons.hpp"

#include "CloudsScene.hpp"
#include "Scene.hpp"


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
    glEnable(GL_DEPTH_TEST);
}

int main()
{
    init();
    CloudsScene cloudsScene = CloudsScene();

    while (!glfwWindowShouldClose(WindowManager::getInstance().getWindow()))
    {
        cloudsScene.update();
        cloudsScene.draw();
        cloudsScene.lateUpdate();
    }
    glfwTerminate();
    return 0;
}