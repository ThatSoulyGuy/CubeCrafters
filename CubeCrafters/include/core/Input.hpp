#ifndef INPUT_HPP
#define INPUT_HPP

#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct KeyState
{
	bool currentState = false;
	bool lastState = false;
};

namespace Input
{
	GLFWwindow* window;
	glm::vec2 mousePosition;

	std::map<int, KeyState> keyStates;

	void Initialize(GLFWwindow* _window)
	{
		window = _window;
	}

	void UpdateInput()
	{
		for (auto& pair : keyStates)
		{
			int key = pair.first;
			pair.second.lastState = pair.second.currentState;
			pair.second.currentState = glfwGetKey(window, key) == GLFW_PRESS;
		}
	}

	bool GetKey(int key, bool state)
	{
		if (state)
			return keyStates[key].currentState;

		return !keyStates[key].currentState;
	}

	bool GetKeyJustPressed(int key)
	{
		return keyStates[key].currentState && !keyStates[key].lastState;
	}

	bool GetMouseButton(int button, bool state)
	{
		if (state)
			return glfwGetMouseButton(window, button) == GLFW_PRESS;

		return glfwGetMouseButton(window, button) == GLFW_RELEASE;
	}

	void SetCursorMode(bool value)
	{
		if (!value)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

#endif // !INPUT_HPP