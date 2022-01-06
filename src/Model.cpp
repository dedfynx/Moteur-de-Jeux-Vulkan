#include "..\inc\Model.h"
#include "Utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std
{
	template <>
	struct hash<dedOs::Model::Vertex>
	{
		size_t operator()(dedOs::Model::Vertex const &vertex) const
		{
			size_t seed = 0;
			dedOs::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}
namespace dedOs
{
	Model::Model(Device &device, const Model::Data &data) : hDevice{device}
	{
		createVertexBuffer(data.vertices);
		createIndexBuffer(data.indices);
	}

	Model::~Model()
	{
		vkDestroyBuffer(hDevice.device(), vBuffer, nullptr);
		vkFreeMemory(hDevice.device(), vBufferMem, nullptr);

		if (hasIndexBuffer)
		{
			vkDestroyBuffer(hDevice.device(), iBuffer, nullptr);
			vkFreeMemory(hDevice.device(), iBufferMem, nullptr);
		}
	}

	void Model::bind(VkCommandBuffer cBuffer)
	{
		VkBuffer buffers[] = {vBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(cBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(cBuffer, iBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Model::draw(VkCommandBuffer cBuffer)
	{
		if (hasIndexBuffer)
		{
			vkCmdDrawIndexed(cBuffer, iCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(cBuffer, vCount, 1, 0, 0);
		}
	}

	void Model::createVertexBuffer(const std::vector<Vertex> &vertices)
	{
		vCount = static_cast<uint32_t>(vertices.size());
		assert(vCount >= 3 && "Vertex Count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;

		hDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMem);

		//Link GPU buffer and  Cpu data
		void *data;
		vkMapMemory(hDevice.device(), stagingBufferMem, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(hDevice.device(), stagingBufferMem);

		hDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vBuffer,
			vBufferMem);

		hDevice.copyBuffer(stagingBuffer, vBuffer, bufferSize);
		vkDestroyBuffer(hDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(hDevice.device(), stagingBufferMem, nullptr);
	}

	void Model::createIndexBuffer(const std::vector<uint32_t> &indices)
	{
		iCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = iCount > 0;

		if (!hasIndexBuffer)
		{
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * iCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;

		hDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMem);

		//Link GPU buffer and  Cpu data
		void *data;
		vkMapMemory(hDevice.device(), stagingBufferMem, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(hDevice.device(), stagingBufferMem);

		hDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			iBuffer,
			iBufferMem);

		hDevice.copyBuffer(stagingBuffer, iBuffer, bufferSize);
		vkDestroyBuffer(hDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(hDevice.device(), stagingBufferMem, nullptr);
	}

	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescription(1);
		bindingDescription[0].binding = 0;
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDescription[0].stride = sizeof(Vertex);

		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescription{};

		attributeDescription.push_back({0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, position)});
		attributeDescription.push_back({1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, color)});
		attributeDescription.push_back({2,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, normal)});
		attributeDescription.push_back({3,0,VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex, uv)});

		return attributeDescription;
	}

	std::unique_ptr<Model> Model::createModelFromFile(Device &device, const std::string &filePath)
	{
		Data data{};
		data.loadModel(filePath);
		return std::make_unique<Model>(device, data);
	}

	void Model::Data::loadModel(const std::string &filePath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
		{
			std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto &shape : shapes)
		{
			for (const auto &index : shape.mesh.indices)
			{
				Vertex vertex{};
				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					//Gestion Couleurs si present dans le fichier
					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}
				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

}
