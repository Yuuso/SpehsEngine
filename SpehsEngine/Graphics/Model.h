#pragma once

namespace se::gfx::impl
{
	class AnimatorInternal;
	class Mesh;
	class ModelInternal;
	class ModelNode;
}

namespace se::gfx
{
	class Animator;
	class Material;
	class ModelAsset;
	class Primitive;
	class VertexBuffer;

	class Model
	{
	public:

												Model();
												~Model();

												Model(const Model& _other) = delete;
		Model&									operator=(const Model& _other) = delete;

												Model(Model&& _other) = delete;
		Model&									operator=(Model&& _other) = delete;


		void									loadModelAsset(std::shared_ptr<ModelAsset> _modelAsset);
		void									reloadModeAsset();

		Animator&								getAnimator();
		const Animator&							getAnimator() const;

		void									foreachPrimitive(std::function<void(Primitive&)> _fn);
		void									foreachPrimitive(std::function<void(Primitive&, std::string_view)> _fn);
		const Primitive*						getPrimitive(std::string_view _meshName) const;
		const Primitive*						getPrimitive(size_t _index) const;
		Primitive*								getPrimitive(std::string_view _meshName);
		Primitive*								getPrimitive(size_t _index);
		size_t									getNumPrimitives() const;


		const std::string&						getName() const;
		bool									getRenderState() const;
		std::shared_ptr<const Material>			getMaterial(size_t _slot = 0) const;
		std::shared_ptr<Material>				getMaterial(size_t _slot = 0);
		size_t									getNumMaterials() const;
		const Color&							getColor() const;
		RenderFlagsType							getRenderFlags() const;
		std::shared_ptr<const VertexBuffer>		getInstances() const;
		std::shared_ptr<VertexBuffer>			getInstances();
		PrimitiveType							getPrimitiveType() const;
		RenderMode								getRenderMode() const;
		const glm::vec3&						getPosition() const;
		const glm::quat&						getRotation() const;
		const glm::vec3&						getScale() const;

		void									setName(std::string_view _name);
		void									setRenderState(bool _state);
		void									setMaterial(std::shared_ptr<Material> _material, size_t _slot = 0);
		void									setColor(const Color& _color);
		void									setRenderFlags(RenderFlagsType _renderFlags);
		void									enableRenderFlags(RenderFlagsType _renderFlags);
		void									disableRenderFlags(RenderFlagsType _renderFlags);
		void									setPrimitiveType(PrimitiveType _primitiveType);
		void									setRenderMode(RenderMode _renderMode);
		void									setInstances(std::shared_ptr<VertexBuffer> _instances);
		void									setPosition(const glm::vec3& _position);
		void									setRotation(const glm::quat& _rotation);
		void									setScale(const glm::vec3& _scale);

	private:

		friend class impl::ModelInternal;
		friend class impl::ModelNode;
		friend class impl::Mesh;

		void									updateAnimations();
		void									postReload();

		std::unique_ptr<impl::ModelNode>		rootNode;
		glm::mat4								globalInverseTransform;

		std::string								name;
		bool									reloaded = false;
		size_t									numMaterialSlots = 0;

		bool									renderState = true;
		RenderFlagsType							renderFlags = defaultRenderFlags | RenderFlag::CullBackFace;
		PrimitiveType							primitiveType = PrimitiveType::Undefined; // Model loading might set primitive types
		RenderMode								renderMode = defaultRenderMode;
		Color									color;

		std::unique_ptr<impl::AnimatorInternal>	animator;

		glm::vec3								position = glm::vec3(0.0f);
		glm::vec3								scale = glm::vec3(1.0f);
		glm::quat								rotation = glm::vec3(0.0f);

		std::shared_ptr<ModelAsset>				modelAsset;
		std::vector<std::shared_ptr<Material>>	materials;
		std::shared_ptr<VertexBuffer>			instances;

		Signal<void(void)>						destroyedSignal;
		ScopedConnection						modelAssetLoadedConnection;
	};
}
