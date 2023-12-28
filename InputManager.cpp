#include "InputManager.hpp"
#include "WindowManager.hpp"

InputManager::InputManager()
{
	lastMousePos = glm::vec2(0, 0);
	cameraControls = nullptr;
}

void InputManager::init(CameraControls* cameraControls)
{
	InputManager::getInstance().keysPressed.resize(static_cast<size_t>(KeyboardKey::KeysCount), false);
	InputManager::getInstance().cameraControls = cameraControls;
}

void InputManager::retrieveInputs()
{
	GLFWwindow* window = WindowManager::getInstance().getWindow();
	for (const std::pair<int, KeyboardKey> pair : keyMap)
	{
		if (pair.second == KeyboardKey::KeysCount) break;
		bool keyPressed = glfwGetKey(window, pair.first) == GLFW_PRESS;
		keysPressed.at(static_cast<size_t>(pair.second)) = keyPressed;
	}
}

void InputManager::handleMouseMovement(float xpos, float ypos)
{
	if (firstMouseInput)
	{
		firstMouseInput = false;
		lastMousePos.x = xpos;
		lastMousePos.y = ypos;
	}

	if (cameraControls == nullptr) return;

	float offsetX = lastMousePos.x - xpos;
	float offsetY = lastMousePos.y - ypos;

	lastMousePos.x = xpos;
	lastMousePos.y = ypos;

	cameraControls->handleMouseMove(offsetX, offsetY);
}

void InputManager::handleScroll(float xoffset, float yoffset)
{
	cameraControls->handleScroll(xoffset, yoffset);
}

bool InputManager::isKeyPressed(KeyboardKey key)
{
	return keysPressed.at(static_cast<size_t>(key));
}

void InputManager::update()
{
	retrieveInputs();
}