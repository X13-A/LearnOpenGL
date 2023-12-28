#ifndef ORBIT_CONTROLS_H
#define ORBIT_CONTROLS_H

#include "CameraControls.hpp"
#include "InputManager.hpp"
#include "Time.hpp"

class OrbitControls : public CameraControls
{
    private:
        float cameraSpeed;
        glm::vec3 target;

	public:
        OrbitControls(Camera& camera, glm::vec3 target, float cameraSpeed) : CameraControls(camera), target(target), cameraSpeed(cameraSpeed)
        {

        }

		void update() override
		{
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::Space))
            {
                camera.translateLocal(glm::vec3(0, cameraSpeed * Time::deltaTime(), 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::LSHIFT))
            {
                camera.translateLocal(glm::vec3(0, -cameraSpeed * Time::deltaTime(), 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::A))
            {
                camera.translateLocal(glm::vec3(-cameraSpeed * Time::deltaTime(), 0, 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::D))
            {
                camera.translateLocal(glm::vec3(cameraSpeed * Time::deltaTime(), 0, 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::W))
            {
                camera.translateLocal(glm::vec3(0, 0, cameraSpeed * Time::deltaTime()));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::S))
            {
                camera.translateLocal(glm::vec3(0, 0, -cameraSpeed * Time::deltaTime()));
            }

            camera.setTarget(target);
            camera.computeAxes();
            camera.computeViewMatrix();
        }
};

#endif