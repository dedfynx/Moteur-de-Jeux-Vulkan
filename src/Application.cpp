#include "Application.h"

#include <array>
#include <stdexcept>
#include <iostream>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace dedOs
{

	Application::Application()
	{
		loadGameObjects();
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		DImgui hImgui{
			hWindow,
			hDevice,
			hRenderer.getSwapChainRenderPass(),
			hRenderer.getImageCount()};

		Camera camera{};
		//camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		RenderSystem renderSystem{hDevice, hRenderer.getSwapChainRenderPass()};

		auto currentTime = std::chrono::high_resolution_clock::now();

		auto viewerObject = GameObject::createGameObject();
		KeyboardController cameraController{};

		while (!hWindow.shouldClose())
		{
			glfwPollEvents();

			//Gestion Delta
			auto newTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//Gestion Camera
			cameraController.moveInPlaneXZ(hWindow.getGLFWwindow(), deltaTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = hRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			float f = 0.5f;
			//Draw Frame
			if (auto commandBuffer = hRenderer.beginFrame())
			{
				//Tell imgui that we're starting a new frame
				hImgui.newFrame();

				//update systems

				// example code telling imgui what windows to render, and their contents
				//hImgui.runExample();
				hImgui.runModelWindow(f);
				/*
				for(auto& gameObj : vGameObjects){
					gameObj.transform.scale = glm::vec3{f};
				}
				*/
				//render system
				hRenderer.beginSwapChainRenderPass(commandBuffer);
				//renderSystem.renderGameObjects(commandBuffer,canvas.getCanvas());
				renderSystem.renderGameObjects(commandBuffer, vGameObjects, camera);

				// as last step in render pass, record the imgui draw commands
				hImgui.render(commandBuffer);
				/*Marche mais spam de Validation Error
				if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
				}
				*/

				hRenderer.endSwapChainRenderPass(commandBuffer);
				hRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(hDevice.device());
	}

	void Application::loadGameObjects()
	{
		std::shared_ptr<Model> model = Model::createModelFromFile(hDevice, "../models/flat_vase.obj");
		auto flat = GameObject::createGameObject();
		flat.model = model;
		flat.transform.translation = {-.5f, .5f, 2.5f};
		flat.transform.scale = glm::vec3{3.f};
		vGameObjects.push_back(std::move(flat));

		model = Model::createModelFromFile(hDevice, "../models/smooth_vase.obj");
		auto smooth = GameObject::createGameObject();
		smooth.model = model;
		smooth.transform.translation = {.5f, .5f, 2.5f};
		smooth.transform.scale = glm::vec3{3.f};

		vGameObjects.push_back(std::move(smooth));
	}

}
