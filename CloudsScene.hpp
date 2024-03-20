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
#include "PostProcessLayer.hpp"

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
    Model rtxSphere;
    Model pointModel;

    // Rendering
	Camera camera;
	FrameBuffer frameBuffer;
    Color clearColor = Color(0, 0, 0, 1); // Color(162.0/255.0, 213.0/255.0, 230.0/255.0, 1.0);
    PostProcessLayer cloudsPostProcess;

    // Shaders
    Shader* cloudShader;
    Shader* screenShader;
    Shader* lightShader;

    // Inputs
	CreativeControls* cameraControls;
    float cameraSpeed = 15;

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
    float sunlightAbsorption = 0.15;
    float cloudScale1 = 80;
    float cloudScale2 = 40;
    float cloudScale3 = 300;

    glm::vec3 cloudSpeed1 = glm::vec3(6, -0, 0);
    glm::vec3 cloudSpeed2 = glm::vec3(3, -0, 0);
    glm::vec3 cloudSpeed3 = glm::vec3(1.5, -0, 0);
    glm::vec3 phaseParams = glm::vec3(0.5, 0.5, 1);

    float bottom_falloff = 0.5;
    float top_falloff = 0.5;

    int cloudSamples = 10;
    int cloudLightSamples = 3;
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
        cloudShader = new Shader("shaders/cloud_vert_fullscreen.glsl", "shaders/cloud_frag_fullscreen.glsl");
        screenShader = new Shader("shaders/screen.vert", "shaders/screen.frag");
        lightShader = new Shader("shaders/lit.vert", "shaders/lit.frag");
        
        // Camera
		camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 100000.0f, glm::vec3(0, 0, 5));
		//camera.translateGlobal(glm::vec3(-86.6055, -86.1902, 35.0335));
        camera.setPosition(glm::vec3(0));
        //camera.setForward(glm::vec3(0.491636, 0.830984, -0.260306));

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

        worleyPoints = generator.CreateWorleyPoints(points_n_3);
        worleyPoints = generator.RepeatWorleyPoints(worleyPoints);
        worleyTexture3 = generator.ComputeWorleyTexture(worleyPoints, worleyRes);

        // Clouds
        cloudsPostProcess.setup(cloudShader, &frameBuffer);
		cloud.loadModel("./models/Cube.obj");
		cloud.translate(glm::vec3(0.5000f, 0.5000f, 0.5000));
        cloud.scale(glm::vec3(150.000001, 15.000001, 150.000001));

        // Ray tracing plane
        rtxSphere.loadModel("./models/Sphere.obj");
        rtxSphere.scale(glm::vec3(0.5f));
        // Debug Models
        pointModel = Model("./models/Sphere.obj");
        pointModel.scale(glm::vec3(5, 5, 5));
	}

    glm::vec3 forwardVectorToEulerAngles(const glm::vec3& forward)
    {
        glm::vec3 angles;

        // Calculate pitch (rotation around x-axis)
        angles.x = glm::degrees(glm::atan(-forward.y, glm::sqrt(forward.x * forward.x + forward.z * forward.z)));

        // Calculate yaw (rotation around y-axis)
        angles.y = glm::degrees(glm::atan(forward.x, forward.z));

        // Roll is set to 0 as we're only interested in aligning with the forward vector

        return angles;
    }

	void update() override
	{
        Time::update();
        InputManager::getInstance().update();
        cameraControls->update();
        rtxSphere.setPosition(camera.getPosition());
        
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
        if (InputManager::getInstance().isKeyPressed(KeyboardKey::O))
        {
            phaseParams += glm::vec3(glm::vec2(0.5) * glm::vec2(Time::deltaTime()), 0);
            phaseParams = glm::clamp(phaseParams, glm::vec3(0), glm::vec3(1));
            std::cout << "Phase: " << phaseParams.r << ", " << phaseParams.g << ", " << phaseParams.b << std::endl;
        }
        if (InputManager::getInstance().isKeyPressed(KeyboardKey::P))
        {
            phaseParams -= glm::vec3(glm::vec2(0.5) * glm::vec2(Time::deltaTime()), 0);
            phaseParams = glm::clamp(phaseParams, glm::vec3(0), glm::vec3(1));
            std::cout << "Phase: " << phaseParams.r << ", " << phaseParams.g << ", " << phaseParams.b << std::endl;
        }
        if (InputManager::getInstance().isKeyPressed(KeyboardKey::K))
        {
            phaseParams.z -= 0.5 * Time::deltaTime();
            phaseParams = glm::clamp(phaseParams, glm::vec3(0), glm::vec3(1));
            std::cout << "Phase: " << phaseParams.r << ", " << phaseParams.g << ", " << phaseParams.b << std::endl;
        }
        if (InputManager::getInstance().isKeyPressed(KeyboardKey::L))
        {
            phaseParams.z += 0.5 * Time::deltaTime();
            phaseParams = glm::clamp(phaseParams, glm::vec3(0), glm::vec3(1));
            std::cout << "Phase: " << phaseParams.r << ", " << phaseParams.g << ", " << phaseParams.b << std::endl;
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

        WindowManager::getInstance().clear(clearColor);
        glm::vec3 lightPos = glm::vec3(5000, 10000, 0);
        drawPoint(lightPos);

        // Draw quad in front of camera
        lightShader->use();
        lightShader->setVec4("lightColor", glm::vec4(0));
        lightShader->setMat4("viewMatrix", camera.viewMatrix);
        lightShader->setMat4("projectionMatrix", camera.projectionMatrix);
        lightShader->setVec3("lightPos", glm::vec3());
        lightShader->setVec3("viewPos", camera.getPosition());
        lightShader->setVec4("ambientColor", RED);

        // Setup cloud shader
        cloudShader->use();

        // Ray tracing params
        cloudShader->setMat4("viewMatrix", camera.viewMatrix);
        cloudShader->setMat4("projectionMatrix", camera.projectionMatrix);

        // Texture params
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, worleyTexture1);
        cloudShader->setInt("noiseTex1", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, worleyTexture2);
        cloudShader->setInt("noiseTex2", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_3D, worleyTexture3);
        cloudShader->setInt("noiseTex3", 3);

        // Cloud params
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
        cloudShader->setVec3("global_speed1", cloudSpeed1);
        cloudShader->setVec3("global_speed2", cloudSpeed2);
        cloudShader->setVec3("global_speed3", cloudSpeed3);
        cloudShader->setFloat("time", glfwGetTime());
        cloudShader->setVec3("phaseParams", phaseParams);
        cloudShader->setFloat("bottom_falloff", bottom_falloff);
        cloudShader->setFloat("top_falloff", top_falloff);

        // Fullscreen params
        rtxSphere.draw(*cloudShader);

        //cloudsPostProcess.apply();
        frameBuffer.drawToWindow(*screenShader);
	}
};

#endif