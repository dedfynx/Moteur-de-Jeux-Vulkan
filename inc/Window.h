#pragma once

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"
#include <vulkan/vulkan.h>
#include <string>
namespace dedOs {
	class Window {
	public:
		Window(int w, int h, std::string n);
		~Window();

		bool shouldClose();
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		VkExtent2D getExtend() {
			return { static_cast<uint32_t>(WIDTH) , static_cast<uint32_t>(HEIGHT) };
		}

		bool wasWindowResized() { return frameBufferResized; };
		void resetWindowResizeFlag() { frameBufferResized = false; };

		GLFWwindow *getGLFWwindow() const {return window;};

	private:
		//delete copy operation
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		GLFWwindow* window;
		int WIDTH;
		int HEIGHT;
		bool frameBufferResized = false;
		std::string windowName;


	};
}