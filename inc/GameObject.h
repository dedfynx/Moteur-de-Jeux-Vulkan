#pragma once

#include "Model.h"
//libs
#include <glm/gtc/matrix_transform.hpp>
//std
#include <memory>

namespace dedOs {
	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f,1.0f ,1.0f};
		glm::vec3 rotation{};

		//Fast mat4
		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct RigidBody2dComponent {
		glm::vec2 velocity;
		float mass{ 1.0f };
	};

	class GameObject {
	public:
		using uint = unsigned int;

		static GameObject createGameObject() {
			static uint currentId = 0;
			return GameObject{ currentId++ };
		}
		//delete copy operation
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		uint getId() { return id; }

		std::shared_ptr<Model> model{};
		glm::vec3 color;
		TransformComponent transform{};
		RigidBody2dComponent rigidBody2d{};
		
	protected:

	private:
		GameObject(uint objId) :id{ objId } {};
		uint id;
	};
}