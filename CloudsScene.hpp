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

    // Worley noise
    CloudNoiseGenerator generator;
    GLuint points_n = 3;
    GLuint worleyRes = 128;
    GLuint worleyTexture;
    std::vector<glm::vec3> worleyPoints;

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
        cloudShader = new Shader("shaders/cloud.vert", "shaders/cloud.frag");
        screenShader = new Shader("shaders/screen.vert", "shaders/screen.frag");
        lightShader = new Shader("shaders/lit.vert", "shaders/lit.frag");
        // Camera
		camera = Camera(45, WindowManager::getInstance().getWidth() / WindowManager::getInstance().getHeight(), 0.1f, 100000.0f, glm::vec3(0, 0, 5));
		camera.translateGlobal(glm::vec3(0.0f, 0.0f, 3.0f));

        // Inputs
		cameraControls = new CreativeControls(camera, 3.0f, 0.1f);
		InputManager::init(cameraControls);

        // Worley
        worleyPoints = generator.CreateWorleyPoints(points_n);
        worleyPoints = generator.RepeatWorleyPoints(worleyPoints);
        worleyTexture = generator.ComputeWorleyTexture(worleyPoints, worleyRes);

        // Clouds
		cloud.loadModel("./models/Cube.obj");
		cloud.translate(glm::vec3(0.5000f, 0.5000f, 0.5000));
        cloud.scale(glm::vec3(1000.1, 4, 1000.1));

        // Debug Models
        pointModel = Model("./models/Sphere.obj");
        pointModel.scale(glm::vec3(0.05, 0.05, 0.05));
	}

	void update() override
	{
        Time::update();
        InputManager::getInstance().update();
        cameraControls->update();

        if (InputManager::getInstance().isKeyPressed(KeyboardKey::G))
        {
            worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPointsGrid(10));
            worleyTexture = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
        }
        else if (InputManager::getInstance().isKeyPressed(KeyboardKey::F))
        {
            worleyPoints = generator.RepeatWorleyPoints(generator.CreateWorleyPoints(points_n));
            worleyTexture = generator.ComputeWorleyTexture(worleyPoints, worleyRes);
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

	void draw() override
	{
        frameBuffer.use();
        frameBuffer.setPass(0);

        WindowManager::getInstance().clear(clearColor);

        // Write to frameBuffer's second pass
        frameBuffer.setPass(1);

        // Setup cloud shader
        cloudShader->use();
        cloudShader->setMat4("viewMatrix", camera.viewMatrix);
        cloudShader->setMat4("projectionMatrix", camera.projectionMatrix);
        cloudShader->setVec3("boundsMin", (cloud.getPosition() - cloud.getScale()));
        cloudShader->setVec3("boundsMax", (cloud.getPosition() + cloud.getScale()));
        cloudShader->setVec3("cameraPos", camera.getPosition());
        cloudShader->setInt("numSteps", 50);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.TCB);
        cloudShader->setInt("mainTex", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, worleyTexture);
        cloudShader->setInt("noiseTex", 1);
        cloud.draw(*cloudShader);

        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Use base render target (window)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.TCB_secondPass); // Draw second pass texture
        frameBuffer.draw(*screenShader);
        glEnable(GL_DEPTH_TEST);
	}
};

#endif