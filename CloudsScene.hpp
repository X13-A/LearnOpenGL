#ifndef CLOUDS_SCENE_HPP
#define CLOUDS_SCENE_HPP

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
#include "CloudNoiseGenerator.hpp"

void control(glm::vec3* point, float moveSpeed)
{
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::Space))
    {
        *point += glm::vec3(0, moveSpeed * Time::deltaTime(), 0);
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::LSHIFT))
    {
        *point += glm::vec3(0, -moveSpeed * Time::deltaTime(), 0);
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::A))
    {
        *point += glm::vec3(-moveSpeed * Time::deltaTime(), 0, 0);
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::D))
    {
        *point += glm::vec3(moveSpeed * Time::deltaTime(), 0, 0);
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::W))
    {
        *point += glm::vec3(0, 0, moveSpeed * Time::deltaTime());
    }
    if (InputManager::getInstance().isKeyPressed(KeyboardKey::S))
    {
        *point += glm::vec3(0, 0, -moveSpeed * Time::deltaTime());
    }
}

class CloudsScene : Scene
{
private:
    // Models
	Model cloud;
    Model pointModel;

    // Rendering
	Camera camera;
	FrameBuffer frameBuffer;
    Color clearColor = Color(0.6, 0.6, 0.8, 1.0);

    // Shaders
    Shader* cloudShader;
    Shader* screenShader;
    Shader* lightShader;

    // Inputs
	CreativeControls* cameraControls;
    float cameraSpeed = 30;

    // Worley noise
    CloudNoiseGenerator generator;
    GLuint points_n_1 = 500;
    GLuint points_n_2 = 1000;
    GLuint points_n_3 = 200;
    GLuint worleyRes = 128;
    GLuint worleyTexture1;
    GLuint worleyTexture2;
    GLuint worleyTexture3;
    std::vector<glm::vec3> worleyPoints;

    // Clouds params
    float cloudBrightness = 1;
    float cloudDensity = 0.25;
    float sunlightAbsorption = 15;
    float cloudScale1 = 80;
    float cloudScale2 = 40;
    float cloudScale3 = 300;

    float cloudSpeed1 = 1;
    float cloudSpeed2 = 2;
    float cloudSpeed3 = 0.5;

    float global_noise = 2;

    float bottom_falloff = 0.5;
    float top_falloff = 0.5;

    int cloudSamples = 10;
    int cloudLightSamples = 4;
    glm::vec3 lightDir = glm::vec3(-90, 0, 0);

public:
	CloudsScene()
	{
		setup();
	}

	void setup() override
	{
        // Framebuffer
		frameBuffer.setup();

        // Shaders
        cloudShader = new Shader("shaders/cloud_vert.glsl", "shaders/cloud_frag.glsl");
        screenShader = new Shader("shaders/screen.vert", "shaders/screen.frag");
        lightShader = new Shader("shaders/lit.vert", "shaders/lit.frag");
        // Camera
		camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 100000.0f, glm::vec3(0, 0, 5));
		camera.translateGlobal(glm::vec3(-86.6055, -86.1902, 35.0335));
        camera.setForward(glm::vec3(0.491636, 0.830984, -0.260306));

        // Inputs
		cameraControls = new CreativeControls(camera, cameraSpeed, 0.1f);
		InputManager::init(cameraControls);

        // Worley
        worleyPoints = generator.CreateWorleyPoints(points_n_1);
        worleyPoints = generator.RepeatWorleyPoints(worleyPoints);
        worleyTexture1 = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
        
        worleyPoints = generator.CreateWorleyPoints(points_n_2);
        worleyPoints = generator.RepeatWorleyPoints(worleyPoints);
        worleyTexture2 = generator.ComputeWorleyTexture(worleyPoints, worleyRes);

        // Meteo
        worleyPoints = generator.CreateWorleyPoints(points_n_3);
        worleyPoints = generator.RepeatWorleyPoints(worleyPoints);
        worleyTexture3 = generator.ComputeWorleyTexture(worleyPoints, worleyRes);

        // Clouds
		cloud.loadModel("./models/Cube.obj");
		cloud.translate(glm::vec3(0.5000f, 0.5000f, 0.5000));
        cloud.scale(glm::vec3(100.000001, 10.000001, 100.000001));

        // Debug Models
        pointModel = Model("./models/Sphere.obj");
        pointModel.scale(glm::vec3(5, 5, 5));
	}

	void update() override
	{
        Time::update();
        InputManager::getInstance().update();
        cameraControls->update();

        if (InputManager::getInstance().isKeyPressed(KeyboardKey::G))
        {
            worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPoints(points_n_1));
            worleyTexture1 = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
        }
        else if (InputManager::getInstance().isKeyPressed(KeyboardKey::F))
        {
            worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPoints(points_n_2));
            worleyTexture2 = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
        }
        else if (InputManager::getInstance().isKeyPressed(KeyboardKey::H))
        {
            worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPoints(points_n_3));
            worleyTexture3 = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
        }
	}

    void lateUpdate() override
    {
        PerformanceTracker::getInstance().update();
        WindowManager::getInstance().update();
    }

    void drawWorleyPoints()
    {
        for (const glm::vec3& point : worleyPoints)
        {
            pointModel.setPosition(point);

            lightShader->use();
            lightShader->setVec4("lightColor", glm::vec4(0));
            lightShader->setMat4("viewMatrix", camera.viewMatrix);
            lightShader->setMat4("projectionMatrix", camera.projectionMatrix);
            lightShader->setVec3("lightPos", glm::vec3());
            lightShader->setVec3("viewPos", camera.getPosition());

            if (point.x >= 0 && point.x <= 1 && point.y >= 0 && point.y <= 1 && point.z >= 0 && point.z <= 1)
            {
                lightShader->setVec4("ambientColor", WHITE);
                pointModel.draw(*lightShader);
            }
            else if (point.y > 0 && point.y < 1)
            {
                lightShader->setVec4("ambientColor", RED);
                pointModel.draw(*lightShader);
            }
            else
            {
                lightShader->setVec4("ambientColor", RED);
                pointModel.draw(*lightShader);
            }
        }
    }

    void drawPoint(glm::vec3 pos)
    {
        pointModel.setPosition(pos);

        lightShader->use();
        lightShader->setVec4("lightColor", glm::vec4(0));
        lightShader->setMat4("viewMatrix", camera.viewMatrix);
        lightShader->setMat4("projectionMatrix", camera.projectionMatrix);
        lightShader->setVec3("lightPos", glm::vec3());
        lightShader->setVec3("viewPos", camera.getPosition());
        lightShader->setVec4("ambientColor", WHITE);
        pointModel.draw(*lightShader);
    }

	void draw() override
	{
        frameBuffer.use();
        frameBuffer.setPass(0);

        WindowManager::getInstance().clear(clearColor);
        glm::vec3 lightPos = glm::vec3(20, 50, 0);
        drawPoint(lightPos);

        // Write to frameBuffer's second pass
        frameBuffer.setPass(1);

        // Setup cloud shader
        cloudShader->use();
        cloudShader->setMat4("viewMatrix", camera.viewMatrix);
        cloudShader->setMat4("projectionMatrix", camera.projectionMatrix);
        cloudShader->setVec3("boundsMin", (cloud.getPosition() - cloud.getScale()));
        cloudShader->setVec3("boundsMax", (cloud.getPosition() + cloud.getScale()));
        cloudShader->setVec3("cameraPos", camera.getPosition());
        cloudShader->setVec3("cameraDir", camera.getForward());
        cloudShader->setVec3("lightDir", -lightPos);
        cloudShader->setVec3("lightPos", lightPos);
        cloudShader->setInt("numSteps", cloudSamples);
        cloudShader->setFloat("sunlightAbsorption", sunlightAbsorption);
        cloudShader->setInt("numLightSteps", cloudLightSamples);
        cloudShader->setFloat("global_density", cloudDensity);
        cloudShader->setFloat("global_brightness", cloudBrightness);
        cloudShader->setFloat("global_scale1", cloudScale1);
        cloudShader->setFloat("global_scale2", cloudScale2);
        cloudShader->setFloat("global_scale3", cloudScale3);

        cloudShader->setFloat("global_speed1", cloudSpeed1);
        cloudShader->setFloat("global_speed2", cloudSpeed2);
        cloudShader->setFloat("global_speed3", cloudSpeed3);

        cloudShader->setFloat("time", glfwGetTime());

        cloudShader->setFloat("bottom_falloff", bottom_falloff);
        cloudShader->setFloat("top_falloff", top_falloff);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.TCB);
        cloudShader->setInt("mainTex", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, worleyTexture1);
        cloudShader->setInt("noiseTex1", 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, worleyTexture2);
        cloudShader->setInt("noiseTex2", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_3D, worleyTexture3);
        cloudShader->setInt("noiseTex3", 3);


        cloud.draw(*cloudShader);
        frameBuffer.drawToWindow(*screenShader);
	}
};

#endif