#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Graphics/IndexBuffer.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "SpehsEngine/Graphics/Material.h"
#include "glm/mat4x4.hpp"
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
			virtual std::shared_ptr<Material>		getMaterial() const;
			virtual std::shared_ptr<VertexBuffer>	getVertices() const;
			virtual std::shared_ptr<IndexBuffer>	getIndices() const;

			virtual const RenderFlagsType			getRenderFlags() const;
			virtual const bool						checkRenderFlag(const RenderFlag _renderFlag) const;
			virtual const PrimitiveType				getPrimitiveType() const;
			virtual const RenderMode				getRenderMode() const;

			virtual const glm::vec3&				getPosition() const;
			virtual const glm::vec3&				getScale() const;
			virtual const glm::quat&				getRotation() const;

			virtual void							setName(const std::string_view _name);
			virtual void							setRenderState(const bool _state);
			virtual void							toggleRenderState();
			virtual void							setMaterial(std::shared_ptr<Material> _material);
			virtual void							setVertices(std::shared_ptr<VertexBuffer> _vertices);
			virtual void							setIndices(std::shared_ptr<IndexBuffer> _indices);
			virtual void							setColor(const Color& _color, const size_t _colorIndex = 0);

			virtual void							setRenderFlags(const RenderFlagsType _renderFlags);
			virtual void							enableRenderFlag(const RenderFlag _renderFlag);
			virtual void							disableRenderFlag(const RenderFlag _renderFlag);
			virtual void							setPrimitiveType(const PrimitiveType _primitiveType);
			virtual void							setRenderMode(const RenderMode _renderMode);

			virtual void							setPosition(const glm::vec3& _position);
			virtual void							setScale(const glm::vec3& _scale);
			virtual void							setRotation(const glm::quat& _rotation);

		protected:

			friend class PrimitiveInstance;
			friend class Model;

			virtual void							update();
			virtual const glm::mat4&				getTransformMatrix() const; // ? needed, expose only either getters or values
			virtual const glm::mat4&				getNormalMatrix() const;
			virtual void							updateMatrices();

			PrimitiveUpdateFlagsType				updateFlags = 0;
			glm::mat4								transformMatrix;
			glm::mat4								normalMatrix;

		private:

			void									invalidatePrimitiveColors();
			bool									getVerticesChanged();
			bool									getIndicesChanged();

			std::string								name					= "primitive";
			bool									renderState				= true;
			RenderFlagsType							renderFlags				= RenderFlag::DepthTest;
			PrimitiveType							primitiveType			= PrimitiveType::Triangles;
			RenderMode								renderMode				= RenderMode::Dynamic;
			std::shared_ptr<Material>				material				= nullptr;
			std::shared_ptr<VertexBuffer>			vertices;
			std::shared_ptr<IndexBuffer>			indices;

			glm::vec3								position				= glm::vec3(0.0f);
			glm::vec3								scale					= glm::vec3(1.0f);
			glm::quat								rotation				= glm::vec3(0.0f);

			boost::signals2::signal<void(void)>		destroyedSignal;

			static constexpr size_t					MAX_PRIMITIVE_COLORS	= 4;
			Color									primitiveColor[MAX_PRIMITIVE_COLORS];
		};
	}
}
