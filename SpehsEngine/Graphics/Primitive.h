#pragma once

#include "boost/signals2.hpp"
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


			const std::string&				getName() const;
			const bool						getRenderState() const;
			const Shader*					getShader() const;
			const std::vector<Vertex>&		getVertices() const;
			const std::vector<IndexType>&	getIndices() const;

			const RenderFlagsType			getRenderFlags() const;
			const bool						checkRenderFlag(const RenderFlag _renderFlag) const;
			const PrimitiveType				getPrimitiveType() const;
			const RenderMode				getRenderMode() const;

			const glm::vec3&				getPosition() const;
			const glm::vec3&				getLocalPosition() const;
			const glm::vec3&				getScale() const;
			const glm::quat&				getRotation() const;

			void							setName(const std::string_view _name);
			void							setRenderState(const bool _state);
			void							toggleRenderState();
			void							setShader(const Shader* _shader);
			void							setVertices(const std::vector<Vertex>& _vertices);
			void							setIndices(const std::vector<uint16_t>& _indices);

			void							setRenderFlags(const RenderFlagsType _renderFlags);
			void							enableRenderFlag(const RenderFlag _renderFlag);
			void							disableRenderFlag(const RenderFlag _renderFlag);
			void							setPrimitiveType(const PrimitiveType _primitiveType);
			void							setRenderMode(const RenderMode _renderMode);

			void							setPosition(const glm::vec3& _position);
			void							setLocalPosition(const glm::vec3& _position);
			void							setScale(const glm::vec3& _scale);
			void							setLocalScale(const glm::vec3& _scale);
			void							setRotation(const glm::quat& _rotation);
			void							setLocalRotation(const glm::quat& _rotation);

		protected:

			std::string						name					= "primitive";
			bool							renderState				= true;
			RenderFlagsType					renderFlags				= RenderFlag::DepthTest;
			PrimitiveType					primitiveType			= PrimitiveType::Triangles;
			RenderMode						renderMode				= RenderMode::Dynamic;
			const Shader*					shader					= nullptr; // TODO: default shader
			std::vector<Vertex>				vertices;
			std::vector<IndexType>			indices;

			// Global space
			glm::vec3						position				= glm::vec3(0.0f);
			glm::vec3						scale					= glm::vec3(1.0f);
			glm::quat						rotation				= glm::vec3(0.0f);

			// Local space
			glm::vec3						localPosition			= glm::vec3(0.0f);
			glm::vec3						localScale				= glm::vec3(1.0f);
			glm::quat						localRotation			= glm::vec3(0.0f);

			glm::vec3						initialLocalPosition	= glm::vec3(0.0f);
			glm::vec3						initialLocalScale		= glm::vec3(1.0f);
			glm::quat						initialLocalRotation	= glm::vec3(0.0f);

		private:

			friend class PrimitiveInstance;

			UpdateFlagsType					updateFlags				= 0/*UpdateFlag::NothingChanged*/;

			boost::signals2::signal<void(void)> destroyedSignal;
		};
	}
}
