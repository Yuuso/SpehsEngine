#pragma once

namespace se::gfx::impl
{
	class Batch;
	class LightBatch;
	class ModelInternal;
	class PrimitiveInternal;
	struct RenderContext;
	class ViewInternal;
}

namespace se::gfx
{
	class Light;
	class Model;
	class Primitive;

	class Scene
	{
	public:

		Scene();
		~Scene();

		Scene(const Scene& _other) = delete;
		Scene& operator=(const Scene& _other) = delete;

		Scene(Scene&& _other) = delete;
		Scene& operator=(Scene&& _other) = delete;


		void add(Primitive& _primitive);
		void remove(Primitive& _primitive);
		bool find(const Primitive& _primitive) const;

		void add(Model& _model);
		void remove(Model& _model);

		void add(Light& _light);
		void remove(Light& _light);

		void clear();

	private:

		friend class impl::ViewInternal;

		void render(impl::RenderContext& _renderContext);
		void preRender(const bool _renderState, const bool _forceAllUpdates);
		void postRender(const bool _renderState);
		void batch(impl::PrimitiveInternal& _primitive);

		std::vector<std::unique_ptr<impl::Batch>> batches;
		std::vector<std::unique_ptr<impl::PrimitiveInternal>> primitives;
		std::vector<std::unique_ptr<impl::ModelInternal>> models;
		std::unique_ptr<impl::LightBatch> lightBatch;

		bool readyToRender = false;
	};
}
