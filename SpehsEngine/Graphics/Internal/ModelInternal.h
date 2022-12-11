#pragma once

namespace se
{
	namespace graphics
	{
		class Model;
		class Primitive;

		class ModelInternal
		{
		public:

			ModelInternal(Model& _model);
			~ModelInternal() = default;

			ModelInternal(const ModelInternal& _other) = delete;
			ModelInternal& operator=(const ModelInternal& _other) = delete;

			ModelInternal(ModelInternal&& _other) = delete;
			ModelInternal& operator=(ModelInternal&& _other) = delete;

			bool operator==(const Model& _other) const;


			const bool wasReloaded() const;
			const bool wasDestroyed() const;
			void foreachPrimitive(std::function<void(Primitive&)> _fn);
			void preRender();
			void postRender();

		private:

			void modelDestroyed();

			boost::signals2::scoped_connection modelDestroyedConnection;

			Model* model = nullptr;
			bool wasAdded = true;
		};
	}
}
