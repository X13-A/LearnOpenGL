#ifndef CREATIVE_CONTROLS_H
#define CREATIVE_CONTROLS_H

#include "CameraControls.hpp"
#include "InputManager.hpp"
#include "Time.hpp"

class CreativeControls : public CameraControls
{
    private:
        float moveSpeed;
        float rotateSpeed;
        float yaw;
        float pitch;

    public:
        CreativeControls(Camera& camera, float moveSpeed, float rotateSpeed) : CameraControls(camera), moveSpeed(moveSpeed), rotateSpeed(rotateSpeed)
        {
            yaw = camera.getYaw();
            pitch = camera.getPitch();
        }

        void handleMouseMove(float xoffset, float yoffset) override
        {
            yaw += xoffset * rotateSpeed;
            pitch += yoffset * rotateSpeed;

            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            glm::vec3 direction;
            direction.x = -(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

            camera.setForward(direction);
            camera.computeAxes();
            camera.computeViewMatrix();
        }

        void handleScroll(float xoffset, float yoffset)
        {
            float fov = camera.getFov() - yoffset;
            if (fov < 1.0f) fov = 1.0f;
            if (fov > 90.0f) fov = 90.0f;
            camera.setFov(fov);
            camera.computeProjectionMatrix();
        }

        void update() override
        {
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::Space))
            {
                camera.translateLocal(glm::vec3(0, moveSpeed * Time::deltaTime(), 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::LSHIFT))
            {
                camera.translateLocal(glm::vec3(0, -moveSpeed * Time::deltaTime(), 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::A))
            {
                camera.translateLocal(glm::vec3(-moveSpeed * Time::deltaTime(), 0, 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::D))
            {
                camera.translateLocal(glm::vec3(moveSpeed * Time::deltaTime(), 0, 0));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::W))
            {
                camera.translateLocal(glm::vec3(0, 0, moveSpeed * Time::deltaTime()));
            }
            if (InputManager::getInstance().isKeyPressed(KeyboardKey::S))
            {
                camera.translateLocal(glm::vec3(0, 0, -moveSpeed * Time::deltaTime()));
            }

            camera.computeAxes();
            camera.computeViewMatrix();
        }
};

#endif