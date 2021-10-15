#pragma once

#include "boost/signals2.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Internal/AnimatorInternal.h"
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

			Animator&						getAnimator();
			const Animator&					getAnimator() const;

			void							foreachPrimitive(std::function<void(Primitive&)> _fn);
			void							foreachPrimitive(std::function<void(Primitive&, std::string_view)> _fn);
			const Primitive*				getPrimitive(std::string_view _meshName) const;
			const Primitive*				getPrimitive(size_t _index) const;
			Primitive*						getPrimitive(std::string_view _meshName);
			Primitive*						getPrimitive(size_t _index);
			size_t							getNumPrimitives() const;


			const std::string&				getName() const;
			std::shared_ptr<Material>		getMaterial(const size_t _slot = 0) const;
			size_t							getNumMaterials() const;
			const Color&					getColor() const;
			RenderFlagsType					getRenderFlags() const;

			std::shared_ptr<VertexBuffer>	getInstances() const;
			void							setInstances(std::shared_ptr<VertexBuffer> _instances);

			const glm::vec3&				getPosition() const;
			const glm::quat&				getRotation() const;
			const glm::vec3&				getScale() const;

			void							setName(std::string_view _name);
			void							setRenderState(bool _state);
			void							setMaterial(std::shared_ptr<Material> _material, size_t _slot = 0);
			void							setColor(const Color& _color);

			void							setRenderFlags(RenderFlagsType _renderFlags);
			void							enableRenderFlags(RenderFlagsType _renderFlags);
			void							disableRenderFlags(RenderFlagsType _renderFlags);
			void							setPrimitiveType(PrimitiveType _primitiveType);
			void							setRenderMode(RenderMode _renderMode);

			void							setPosition(const glm::vec3& _position);
			void							setRotation(const glm::quat& _rotation);
			void							setScale(const glm::vec3& _scale);

		private:

			friend class ModelInternal;
			friend class ModelNode;
			friend class Mesh;

			void							updateAnimations();
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

			AnimatorInternal				animator;

			glm::vec3						position = glm::vec3(0.0f);
			glm::vec3						scale = glm::vec3(1.0f);
			glm::quat						rotation = glm::vec3(0.0f);

			std::shared_ptr<ModelData>		modelData;
			std::vector<std::shared_ptr<Material>> materials;
			std::shared_ptr<VertexBuffer>	instances;

			boost::signals2::signal<void(void)> destroyedSignal;
			boost::signals2::scoped_connection modelDataLoadedConnection;
		};
	}
}
