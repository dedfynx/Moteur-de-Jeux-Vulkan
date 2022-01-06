#include "..\inc\Window.h"
#include <Application.h>

namespace dedOs {
    Window::Window(int w, int h, std::string n) : WIDTH{ w }, HEIGHT{ h }, windowName{ n }
    {
        initWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool Window::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Window Surface");
        }
    }

    void Window::frameBufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto hWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        hWindow->frameBufferResized = true;
        hWindow->WIDTH = width;
        hWindow->HEIGHT = height;
    }

    void Window::initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
    }
}