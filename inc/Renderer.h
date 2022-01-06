#pragma once

#include <vector>
#include <memory>
#include <cassert>

#include "Window.h"
#include "Device.h"
#include "SwapChain.h"

namespace dedOs {
	class Renderer {
	public:
		Renderer( Window& window, Device& device);
		~Renderer();
		

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		bool isFrameInProgress() const {
			return isFrameStarted;
		};

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Need a frame in progress to get command Buffer");
			return cBuffer[currentFrameIndex];
		};

		VkRenderPass getSwapChainRenderPass() const {
			return hSwapChain->getRenderPass();
		};

		uint32_t getImageCount() const { return hSwapChain->imageCount(); }

		float getAspectRatio() const{return hSwapChain->extentAspectRatio();}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}
		
	private:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		void createCommandBuffer();
		void freeCommandBuffer();
		void recreateSwapChain();

		Window& hWindow;
		Device& hDevice;
			
		std::unique_ptr<SwapChain> hSwapChain;
		std::vector<VkCommandBuffer> cBuffer;
		uint32_t currentImageIndex{ 0 };
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}