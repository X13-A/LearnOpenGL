#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
	private:
		float ratio;
		float near;
		float far;
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;


	public:
		float fov;
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
			this->front = glm::vec3(0.0f, 0.0f, -1.0f);
			computeProjectionMatrix();
			computeViewMatrix();
		}

		void computeProjectionMatrix()
		{
			projectionMatrix = glm::perspective(glm::radians(fov), ratio, 0.1f, 100.0f);
		}

		void computeViewMatrix()
		{
			viewMatrix = glm::mat4(1.0f);
			viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
			//viewMatrix = glm::lookAt(position, position + front, up);
		}
};

#endif