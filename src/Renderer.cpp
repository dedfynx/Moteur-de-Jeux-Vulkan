#include "Renderer.h"

#include <array>
#include <stdexcept>

namespace dedOs {
	Renderer::Renderer(Window& window, Device& device) : hDevice{ device }, hWindow{ window }
	{
		recreateSwapChain();
		createCommandBuffer();
	}

	Renderer::~Renderer()
	{
		freeCommandBuffer();
	}


	void Renderer::createCommandBuffer()
	{
		cBuffer.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo aInfo{};
		aInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		aInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		aInfo.commandPool = hDevice.getCommandPool();
		aInfo.commandBufferCount = static_cast<uint32_t>(cBuffer.size());

		if (vkAllocateCommandBuffers(hDevice.device(), &aInfo, cBuffer.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate Command Buffer");
		}
	}

	void Renderer::freeCommandBuffer()
	{
		vkFreeCommandBuffers(
			hDevice.device(),
			hDevice.getCommandPool(),
			static_cast<uint32_t>(cBuffer.size()),
			cBuffer.data());
		cBuffer.clear();
	}

	void Renderer::recreateSwapChain()
	{
		auto extent = hWindow.getExtend();
		while (extent.width == 0 || extent.height == 0) {
			auto extent = hWindow.getExtend();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(hDevice.device());
		if (hSwapChain == nullptr) {
			hSwapChain = std::make_unique<SwapChain>(hDevice, extent);
		}
		else {
			std::shared_ptr<SwapChain> oldSwapChain = std::move(hSwapChain);
			hSwapChain = std::make_unique<SwapChain>(hDevice, extent, oldSwapChain);

			if (!oldSwapChain->cmpSwapFormat(*hSwapChain.get())) {
				throw std::runtime_error("SwapChain image or depth format has changed");
			}

		}
	}

	VkCommandBuffer Renderer::beginFrame() {
		assert(!isFrameStarted && "Cant begin frame, A frame is in progress already");

		auto result = hSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Fail to acquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo bInfo{};
		bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &bInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording Command Buffer");
		}

		return commandBuffer;
	}

	void Renderer::endFrame() {
		assert(isFrameStarted && "Cant end frame, A frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Fail to record command buffer");
		}

		auto result = hSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || hWindow.wasWindowResized()) {
			hWindow.resetWindowResizeFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Fail to submit Command Buffer");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;

	}

	void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cant beginSwapChainRenderPass, A frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cant beginSwapChainRenderPass on command buffer from a different frame");

		VkRenderPassBeginInfo rpInfo{};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.renderPass = hSwapChain->getRenderPass();
		rpInfo.framebuffer = hSwapChain->getFrameBuffer(currentImageIndex);
		rpInfo.renderArea.offset = { 0,0 };
		rpInfo.renderArea.extent = hSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f,0.01f,0.01f ,1.0f };
		clearValues[1].depthStencil = { 1.0f,0 };

		rpInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		rpInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(hSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(hSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{ {0,0},hSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cant endSwapChainRenderPass, A frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cant endSwapChainRenderPass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}