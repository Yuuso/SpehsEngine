#pragma once

#include "SpehsEngine/Graphics/Vertex.h"
#include "SpehsEngine/Graphics/RenderFlags.h"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include <vector>
#include <string>


namespace se
{
	namespace graphics
	{
		class Shader;

		class Primitive
		{
		public:

			Primitive();
			~Primitive();

			Primitive(const Primitive& _other) = delete;
			Primitive& operator=(const Primitive& _other) = delete;

			Primitive(Primitive&& _other) = delete;
			Primitive& operator=(Primitive&& _other) = delete;

		//protected:

			friend class PrimitiveInstance;

			RenderFlagsType renderFlags;
			// TODO...

			std::string name;
			std::vector<Vertex> vertices;
			std::vector<uint16_t> indices;

			const Shader* shader;

			// Global space
			glm::vec3 position = glm::vec3(0.0f);
			glm::vec3 scale = glm::vec3(1.0f);
			glm::quat rotation = glm::vec3(0.0f);

			// Local space
			glm::vec3 localPosition = glm::vec3(0.0f);
			glm::vec3 localScale = glm::vec3(1.0f);
			glm::quat localRotation = glm::vec3(0.0f);

			// Primitive space
			/*const*/ glm::vec3 primitivePosition = glm::vec3(0.0f);
			/*const*/ glm::vec3 primitiveScale = glm::vec3(1.0f);
			/*const*/ glm::quat primitiveRotation = glm::vec3(0.0f);

		private:

		};
	}
}
