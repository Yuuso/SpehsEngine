
#include "BatchRenderer.h"
#include "Primitive.h"
#include "Mesh.h"
#include "Batch.h"


namespace SpehsEngine
{
	BatchRenderer* batchRenderer;
	BatchRenderer::BatchRenderer(Camera2D* _camera)
	{
		camera = _camera;
	}
	BatchRenderer::~BatchRenderer()
	{
		for (unsigned i = 0; i < batches.size(); i++)
		{
			delete batches[i];
		}
	}


	void BatchRenderer::render()
	{
		for (unsigned i = 0; i < batches.size(); i++)
		{
			batches[i]->render();
		}
	}

	Batch* BatchRenderer::addBatchObject(BatchObject* _primitive)
	{
		//Find a fitting batch for object and add it there
		//or create a new one
		return nullptr;
	}
}
