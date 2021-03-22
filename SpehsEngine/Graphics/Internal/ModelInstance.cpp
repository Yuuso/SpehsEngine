#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ModelInstance.h"


namespace se
{
	namespace graphics
	{
		ModelInstance::ModelInstance(Model& _model)
			: model(&_model)
		{
			modelDestroyedConnection = model->destroyedSignal.connect(boost::bind(&ModelInstance::modelDestroyed, this));
		}

		bool ModelInstance::operator==(const Model& _other) const
		{
			return &_other == model;
		}


		const bool ModelInstance::wasReloaded() const
		{
			return model->reloaded || wasAdded;
		}
		const bool ModelInstance::wasDestroyed() const
		{
			return model == nullptr;
		}
		void ModelInstance::foreachPrimitive(std::function<void(Primitive&)> _fn)
		{
			model->foreachPrimitive(_fn);
		}
		void ModelInstance::preRender()
		{
			model->updateAnimations();
		}
		void ModelInstance::postRender()
		{
			model->reloaded = false;
			wasAdded = false;
		}

		void ModelInstance::modelDestroyed()
		{
			model = nullptr;
		}
	}
}
