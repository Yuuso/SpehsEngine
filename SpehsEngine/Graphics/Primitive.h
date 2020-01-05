#pragma once

#include "SpehsEngine/Graphics/Types.h"
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
			Primitive&						operator=(const Primitive& _other) = delete;

											Primitive(Primitive&& _other) = delete;
			Primitive&						operator=(Primitive&& _other) = delete;


			const std::string&				nameGet() const;
			const bool						renderStateGet() const;
			const Shader*					shaderGet() const;
			const std::vector<Vertex>&		verticesGet() const;
			const std::vector<IndexType>&	indicesGet() const;

			const RenderFlagsType			renderFlagsGet() const;
			const bool						renderFlagCheck(const RenderFlag _renderFlag) const;
			const RenderStyle				renderStyleGet() const;
			const RenderMode				renderModeGet() const;

			const glm::vec3&				positionGet() const;
			const glm::vec3&				localPositionGet() const;
			const glm::vec3&				scaleGet() const;
			const glm::quat&				rotationGet() const;

			void							nameSet(const std::string_view _name);
			void							renderStateSet(const bool _state);
			void							shaderSet(const Shader* _shader);
			void							verticesSet(const std::vector<Vertex>& _vertices);
			void							indicesSet(const std::vector<uint16_t>& _indices);

			void							renderFlagsSet(const RenderFlagsType _renderFlags);
			void							renderFlagEnable(const RenderFlag _renderFlag);
			void							renderFlagDisable(const RenderFlag _renderFlag);
			void							renderStyleSet(const RenderStyle _renderStyle);
			void							renderModeSet(const RenderMode _renderMode);

			void							positionSet(const glm::vec3& _position);
			void							localPositionSet(const glm::vec3& _position);
			void							scaleSet(const glm::vec3& _scale);
			void							rotationSet(const glm::quat& _rotation);

		protected:

			std::string						name					= "primitive";
			bool							renderState				= true;
			RenderFlagsType					renderFlags				= RenderFlag::BackFaceCulling
																	| RenderFlag::DepthTest;
			RenderStyle						renderStyle				= RenderStyle::Triangles;
			RenderMode						renderMode				= RenderMode::Dynamic;
			const Shader*					shader					= nullptr;
			std::vector<Vertex>				vertices;
			std::vector<IndexType>			indices;

			// Global space
			glm::vec3						position				= glm::vec3(0.0f);
			glm::vec3						scale					= glm::vec3(1.0f);
			glm::quat						rotation				= glm::vec3(0.0f);

			// Local space
			glm::vec3						localPosition			= glm::vec3(0.0f);

			friend class PrimitiveInstance;

			glm::vec3						initialLocalPosition	= glm::vec3(0.0f);
			glm::vec3						initialLocalScale		= glm::vec3(1.0f);
			glm::quat						initialLocalRotation	= glm::vec3(0.0f);

			UpdateFlagsType					updateFlags				= UpdateFlag::NothingChanged;
		};
	}
}
