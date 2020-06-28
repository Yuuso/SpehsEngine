#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include <vector>
#include <string>
#include <memory>


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
			virtual const std::shared_ptr<Shader>	getShader() const;
			virtual const std::shared_ptr<Texture>	findTexture(const std::string_view _uniformName) const;
			virtual const VertexBuffer&				getVertices() const;
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
			virtual void							setShader(std::shared_ptr<Shader> _shader);
			virtual void							setTexture(std::shared_ptr<Texture> _texture, const std::string_view _uniformName);
			virtual void							setVertices(const VertexBuffer& _vertices);
			virtual void							setIndices(const std::vector<uint16_t>& _indices);
			virtual void							setColor(const Color& _color); // TODO: How to deal with colors?

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
			std::shared_ptr<Shader>					shader					= nullptr;
			VertexBuffer							vertices;
			std::vector<IndexType>					indices;

			struct TextureInstance
			{
				TextureInstance(const std::string_view uniform, std::shared_ptr<Texture> tex)
					: uniformName(uniform), texture(tex) {}
				std::string uniformName;
				std::shared_ptr<Texture> texture;
			};
			std::vector<std::unique_ptr<TextureInstance>> textures;

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
			PrimitiveUpdateFlagsType				updateFlags				= 0;
			Color									primitiveColor			= { -1.0f, -1.0f, -1.0f, -1.0f };
		};
	}
}
