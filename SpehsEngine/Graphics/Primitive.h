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
			virtual									~Primitive();

													Primitive(const Primitive& _other) = delete;
			Primitive&								operator=(const Primitive& _other) = delete;

													Primitive(Primitive&& _other) = delete;
			Primitive&								operator=(Primitive&& _other) = delete;


			virtual const std::string&				getName() const;
			virtual const bool						getRenderState() const;
			virtual const Shader*					getShader() const;
			virtual const std::vector<Vertex>&		getVertices() const;
			virtual const std::vector<IndexType>&	getIndices() const;

			virtual const RenderFlagsType			getRenderFlags() const;
			virtual const bool						checkRenderFlag(const RenderFlag _renderFlag) const;
			virtual const PrimitiveType				getPrimitiveType() const;
			virtual const RenderMode				getRenderMode() const;

			virtual const glm::vec3&				getPosition() const;
			virtual const glm::vec3&				getLocalPosition() const;
			virtual const glm::vec3&				getScale() const;
			virtual const glm::vec3&				getLocalScale() const;
			virtual const glm::quat&				getRotation() const;
			virtual const glm::quat&				getLocalRotation() const;

			virtual void							setName(const std::string_view _name);
			virtual void							setRenderState(const bool _state);
			virtual void							toggleRenderState();
			virtual void							setShader(const Shader* _shader);
			virtual void							setVertices(const std::vector<Vertex>& _vertices);
			virtual void							setIndices(const std::vector<uint16_t>& _indices);

			virtual void							setRenderFlags(const RenderFlagsType _renderFlags);
			virtual void							enableRenderFlag(const RenderFlag _renderFlag);
			virtual void							disableRenderFlag(const RenderFlag _renderFlag);
			virtual void							setPrimitiveType(const PrimitiveType _primitiveType);
			virtual void							setRenderMode(const RenderMode _renderMode);

			virtual void							setPosition(const glm::vec3& _position);
			virtual void							setLocalPosition(const glm::vec3& _position);
			virtual void							setScale(const glm::vec3& _scale);
			virtual void							setLocalScale(const glm::vec3& _scale);
			virtual void							setRotation(const glm::quat& _rotation);
			virtual void							setLocalRotation(const glm::quat& _rotation);

		private:

			std::string								name					= "primitive";
			bool									renderState				= true;
			RenderFlagsType							renderFlags				= RenderFlag::DepthTest;
			PrimitiveType							primitiveType			= PrimitiveType::Triangles;
			RenderMode								renderMode				= RenderMode::Dynamic;
			const Shader*							shader					= nullptr; // TODO: default shader
			std::vector<Vertex>						vertices;
			std::vector<IndexType>					indices;

			// Global space
			glm::vec3								position				= glm::vec3(0.0f);
			glm::vec3								scale					= glm::vec3(1.0f);
			glm::quat								rotation				= glm::vec3(0.0f);

			// Local space
			glm::vec3								localPosition			= glm::vec3(0.0f);
			glm::vec3								localScale				= glm::vec3(1.0f);
			glm::quat								localRotation			= glm::vec3(0.0f);

			friend class PrimitiveInstance;
			boost::signals2::signal<void(void)>		destroyedSignal;
			PrimitiveUpdateFlagsType				updateFlags = 0;
		};
	}
}
