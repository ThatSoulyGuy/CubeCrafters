#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "core/Logger.hpp"
#include "core/Input.hpp"

namespace Window
{
	GLFWwindow* window = NULL;

	std::string title = "";
	glm::ivec2 size = {};
	glm::ivec2 position = {};

	glm::vec3 color = {};

	float deltaTime = 0.0f, currentFrame = 0.0f, lastFrame = 0.0f;

	void ResizeWindow(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void MouseMove(GLFWwindow* window, double xposIn, double yposIn)
	{
		Input::mousePosition.x = static_cast<float>(xposIn);
		Input::mousePosition.y = static_cast<float>(yposIn);
	}

	void Initalize()
	{
		Logger_FunctionStart;

		if (!glfwInit())
			Logger_ThrowError("nullptr", "Failed to initialize GLFW!", true);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 16);

		Logger_FunctionEnd;
	}

	void Center()
	{
		glfwGetWindowSize(window, &size.x, &size.y);

		const GLFWvidmode* video = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwSetWindowPos(window, (video->width - size.x) / 2, (video->height - size.y) / 2);
	}

	void GenerateWindow(const std::string& title, const glm::ivec2& size, const glm::vec3& color = glm::vec3{ 0.0f, 0.45f, 0.75f })
	{
		Logger_FunctionStart;

		window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
		Window::size = size;
		Window::color = color;

		if (!window)
			Logger_ThrowError("nullptr", "Failed to create GLFW window", true);

		glfwMakeContextCurrent(window);
		glfwSetCursorPosCallback(window, MouseMove);
		glfwSetFramebufferSizeCallback(window, ResizeWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			Logger_ThrowError("nullptr", "Failed to initialize GLAD", true);

		Center();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		Logger_FunctionEnd;
	}

	bool ShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void UpdateColors()
	{
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwGetWindowSize(window, &size.x, &size.y);
		glfwGetWindowPos(window, &position.x, &position.y);

		glClearColor(color.x, color.y, color.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void UpdateBuffers()
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void CleanUp()
	{
		Logger_FunctionStart;
		
		glfwDestroyWindow(window);
		glfwTerminate();

		Logger_FunctionEnd;
	}
}

#endif // !WINDOW_HPP