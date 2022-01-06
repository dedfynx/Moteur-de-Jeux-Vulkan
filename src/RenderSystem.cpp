#include "RenderSystem.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace dedOs {
	struct SimplePushConstantData {
		glm::mat4 transform{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
		//alignas(16) glm::vec3 color;

	};

	RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass) : hDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderPass);

	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(hDevice.device(), pLayout, nullptr);
	}

	void RenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pLayoutInfo{};
		pLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pLayoutInfo.setLayoutCount = 0;
		pLayoutInfo.pSetLayouts = nullptr;
		pLayoutInfo.pushConstantRangeCount = 1;
		pLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(hDevice.device(), &pLayoutInfo, nullptr, &pLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Pipeline Layout");
		}
	}

	void RenderSystem::createPipeline(VkRenderPass renderPass)
	{

		assert(pLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo  pConfInfo{};
		Pipeline::defaultPCI(pConfInfo);
		pConfInfo.renderPass = renderPass;
		pConfInfo.pipelineLayout = pLayout;
		hPipeline = std::make_unique<Pipeline>(
			hDevice,
			"../shaders/vshader.spv",
			"../shaders/fshader.spv",
			pConfInfo);
	}

	void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera)
	{
		hPipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& obj : gameObjects) {
			SimplePushConstantData push{};
			//push.color = obj.color;
			auto modelMatrix = obj.transform.mat4();
			push.transform = projectionView * modelMatrix;
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				commandBuffer,
				pLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

}

