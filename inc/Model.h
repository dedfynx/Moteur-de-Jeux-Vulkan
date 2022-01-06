#pragma once

#include <Device.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace dedOs {

	class Model {
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex &other) const{
				return 
				position == other.position && 
				color == other.color &&
				normal == other.normal &&
				uv == other.uv;
			}
		};

		struct Data{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string &filePath);
		};

		Model(Device &device, const Model::Data &data);
		~Model();

		void bind(VkCommandBuffer cBuffer);
		void draw(VkCommandBuffer cBuffer);

		static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filePath);

	private:
		//delete copy operation
		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		void createVertexBuffer(const std::vector<Vertex> &vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);
		Device &hDevice;

		VkBuffer vBuffer;
		VkDeviceMemory vBufferMem;
		uint32_t vCount;

		bool hasIndexBuffer = false;
		VkBuffer iBuffer;
		VkDeviceMemory iBufferMem;
		uint32_t iCount;
	};
}