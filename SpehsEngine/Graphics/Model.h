#pragma once

#include "boost/signals2.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Internal/Animation.h"
#include "SpehsEngine/Graphics/Internal/Mesh.h"
#include "SpehsEngine/Graphics/Internal/ModelNode.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/ModelData.h"
#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/Types.h"
#include <memory>
#include <string>


namespace se
{
	namespace graphics
	{
		class Model
		{
		public:

											Model();
											~Model();

											Model(const Model& _other) = delete;
			Model&							operator=(const Model& _other) = delete;

											Model(Model&& _other) = delete;
			Model&							operator=(Model&& _other) = delete;


			void							loadModelData(std::shared_ptr<ModelData> _modelData);
			void							reloadModeData();

			void							startAnimation(const std::string_view _name);
			void							stopAnimation();

			void							foreachPrimitive(std::function<void(Primitive&)> _fn);
			const Primitive*				getPrimitive(const std::string_view _meshName) const;
			const Primitive*				getPrimitive(const size_t _index) const;
			Primitive*						getPrimitive(const std::string_view _meshName);
			Primitive*						getPrimitive(const size_t _index);
			const size_t					getNumPrimitives() const;


			const std::string&				getName() const;
			std::shared_ptr<Material>		getMaterial(const size_t _slot = 0) const;
			const size_t					getNumMaterials() const;

			const glm::vec3&				getPosition() const;
			const glm::quat&				getRotation() const;
			const glm::vec3&				getScale() const;

			void							setName(const std::string_view _name);
			void							setRenderState(const bool _state);
			void							setMaterial(std::shared_ptr<Material> _material, const size_t _slot = 0);
			void							setColor(const Color& _color);

			void							setRenderFlags(const RenderFlagsType _renderFlags);
			void							enableRenderFlags(const RenderFlagsType _renderFlags);
			void							disableRenderFlags(const RenderFlagsType _renderFlags);
			void							setPrimitiveType(const PrimitiveType _primitiveType);
			void							setRenderMode(const RenderMode _renderMode);

			void							setPosition(const glm::vec3& _position);
			void							setRotation(const glm::quat& _rotation);
			void							setScale(const glm::vec3& _scale);

		private:

			friend class ModelInstance;
			friend class ModelNode;
			friend class Mesh;

			void							updateAnimation();
			void							postReload();

			ModelNode						rootNode;
			glm::mat4						globalInverseTransform;

			std::string						name;
			bool							reloaded = false;
			size_t							numMaterialSlots = 0;

			bool							renderState = true;
			RenderFlagsType					renderFlags = defaultRenderFlags | RenderFlag::CullBackFace;
			PrimitiveType					primitiveType = PrimitiveType::Undefined; // Model loading might set primitive types
			RenderMode						renderMode = defaultRenderMode;
			Color							color;

			std::string						activeAnimationName;
			const Animation*				activeAnimation = nullptr;

			glm::vec3						position = glm::vec3(0.0f);
			glm::vec3						scale = glm::vec3(1.0f);
			glm::quat						rotation = glm::vec3(0.0f);

			std::shared_ptr<ModelData>		modelData;
			std::vector<std::shared_ptr<Material>> materials;
			std::shared_ptr<std::vector<Animation>> animations;

			boost::signals2::signal<void(void)> destroyedSignal;
			boost::signals2::scoped_connection modelDataLoadedConnection;
		};
	}
}
