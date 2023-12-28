#ifndef CAMERA_CONTROLS_H
#define CAMERA_CONTROLS_H

#include "Camera.hpp"

class CameraControls
{
	protected:
		Camera& camera;

	public:
		CameraControls(Camera& camera) : camera(camera)
		{

		}

		virtual void handleMouseMove(float xoffset, float yoffset)
		{

		}

		virtual void handleScroll(float xoffset, float yoffset)
		{

		}

		virtual void update()
		{

		}
};

#endif