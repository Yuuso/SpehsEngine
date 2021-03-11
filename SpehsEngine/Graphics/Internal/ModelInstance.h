#pragma once

#include "boost/signals2/connection.hpp"
#include "SpehsEngine/Graphics/Model.h"

namespace se
{
	namespace graphics
	{
		class ModelInstance
		{
		public:

			ModelInstance(Model& _model);
			~ModelInstance() = default;

			ModelInstance(const ModelInstance& _other) = delete;
			ModelInstance& operator=(const ModelInstance& _other) = delete;

			ModelInstance(ModelInstance&& _other) = delete;
			ModelInstance& operator=(ModelInstance&& _other) = delete;

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
