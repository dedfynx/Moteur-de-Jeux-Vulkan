#pragma once

#include <stdexcept>
#include <vector>
#include <memory>

#include "Pipeline.h"
#include "Device.h"
#include "GameObject.h"
#include "Camera.h"

namespace dedOs {
	class RenderSystem
	{

	public:
		RenderSystem(Device& device, VkRenderPass renderPass);
		~RenderSystem();

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera);
	private:
		//delete copy operation
		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		Device& hDevice;
		std::unique_ptr<Pipeline> hPipeline;
		VkPipelineLayout pLayout;
	};
}


