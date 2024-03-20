#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Camera
{
	private:
		float ratio;
		float near;
		float far;
		glm::vec3 position;
		
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;
		float fov;

	public:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		Camera(float fov = 45, float ratio = 8/6, float near = 0.1f, float far = 100.0f, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
		{
			this->fov = fov;
			this->ratio = ratio;
			this->near = near;
			this->far = far;
			this->position = position;
			this->up = up;
			this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
			computeProjectionMatrix();
			computeViewMatrix();
			computeAxes();
		}

		/// <summary>
		/// Sets the target at which the camera should look at
		/// </summary>
		void setTarget(glm::vec3 target)
		{
			setForward(target - position);
		}

		/// <summary>
		/// Directly sets the forward vector of the camera
		/// </summary>
		void setForward(glm::vec3 forward)
		{
			this->forward = glm::normalize(forward);
		}

		const glm::vec3& getForward() const
		{
			return forward;
		}

		const float& getFov() const
		{
			return fov;
		}

		void setFov(float fov)
		{
			this->fov = fov;
		}

		void setRatio(float ratio)
		{
			this->ratio = ratio;
		}

		/// <summary>
		/// Computes the right and up axes depending on the forward axis
		/// </summary>
		void computeAxes()
		{
			right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
			up = glm::cross(right, forward);
		}

		/// <summary>
		/// Computes the projection matrix
		/// </summary>
		void computeProjectionMatrix()
		{
			projectionMatrix = glm::perspective(glm::radians(fov), ratio, near, far);
		}

		/// <summary>
		/// Conputes the view matrix depending on the camera direction
		/// </summary>
		void computeViewMatrix()
		{
			viewMatrix = glm::lookAt(position, position + forward, up);
		}

		const glm::vec3& getPosition() const
		{
			return this->position;
		}

		/// <summary>
		/// Updates the position of the camera
		/// </summary>
		void setPosition(glm::vec3 position)
		{
			this->position = position;
		}

		/// <summary>
		/// Translate the camera in world space
		/// </summary>
		void translateGlobal(glm::vec3 offset)
		{
			position += offset;
		}

		/// <summary>
		/// Translates the camera in local space
		/// </summary>
		void translateLocal(glm::vec3 offset)
		{
			position += right * offset.x;
			position += up * offset.y;
			position += forward * offset.z;
		}

		float getYaw() const
		{
			float yaw = atan2(forward.z, forward.x);
			return yaw;
		}

		float getPitch() const
		{
			float pitch = asin(forward.y);
			return pitch;
		}

		float getNear() const
		{
			return near;
		}

		float getFar() const
		{
			return far;
		}

		void printValues()
		{
			std::cout << "Pos: " << getPosition().x << ", " << getPosition().y << ", " << getPosition().z << std::endl;
			std::cout << "Rot: " << getForward().x << ", " << getForward().y << ", " << getForward().z << std::endl;
		}
};

#endif