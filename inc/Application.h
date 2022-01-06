#pragma once

#include <stdexcept>
#include <vector>
#include <memory>

#include "Window.h"
#include "Device.h"
#include "Renderer.h"
#include "RenderSystem.h"
#include "GameObject.h"
#include "Camera.h"
#include "KeyboardController.h"
#include "DImgui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace dedOs {
	class Application
	{

	public:
		//Taille de base
		const int WIDTH = 800;
		const int HEIGHT = 600;

		Application();
		~Application();
		void run();
	private:
		//delete copy operation
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void loadGameObjects();

		
		Window hWindow{ WIDTH,HEIGHT,"Moteur Jeux" };
		Device hDevice{ hWindow };
		Renderer hRenderer{ hWindow, hDevice };
		std::vector<GameObject> vGameObjects;

		


	};
}


