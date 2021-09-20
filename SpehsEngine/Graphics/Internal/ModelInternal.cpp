#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ModelInternal.h"


namespace se
{
	namespace graphics
	{
		ModelInternal::ModelInternal(Model& _model)
			: model(&_model)
		{
			modelDestroyedConnection = model->destroyedSignal.connect(boost::bind(&ModelInternal::modelDestroyed, this));
		}

		bool ModelInternal::operator==(const Model& _other) const
		{
			return &_other == model;
		}


		const bool ModelInternal::wasReloaded() const
		{
			return model->reloaded || wasAdded;
		}
		const bool ModelInternal::wasDestroyed() const
		{
			return model == nullptr;
		}
		void ModelInternal::foreachPrimitive(std::function<void(Primitive&)> _fn)
		{
			model->foreachPrimitive(_fn);
		}
		void ModelInternal::preRender()
		{
			model->updateAnimations();
		}
		void ModelInternal::postRender()
		{
			model->reloaded = false;
			wasAdded = false;
		}

		void ModelInternal::modelDestroyed()
		{
			model = nullptr;
		}
	}
}
