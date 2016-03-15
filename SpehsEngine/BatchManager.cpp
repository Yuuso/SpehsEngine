
#include "BatchManager.h"
#include "Primitive.h"
#include "Mesh.h"
#include "Batch.h"

#include <algorithm>


namespace spehs
{
	BatchManager::BatchManager(Camera2D* _camera)
	{
		camera2D = _camera;
		camera3D = nullptr;
	}
	BatchManager::BatchManager(Camera3D* _camera)
	{
		camera2D = nullptr;
		camera3D = _camera;
	}
	BatchManager::~BatchManager()
	{
		//Delete batches
		clearBatches();

		//Delete Primitives
		for (unsigned i = 0; i < primitives.size(); i++)
		{
			delete primitives[i];
		}

		//Delete Meshes
		for (unsigned i = 0; i < meshes.size(); i++)
		{
			delete meshes[i];
		}
	}


	void BatchManager::render()
	{
		static bool batchFound;

	//BATCHING:

		//Batch primitives
		for (unsigned i = 0; i < primitives.size();)
		{
			//Check if user has requested for deletion
			if (primitives[i]->readyForDelete)
			{
				delete primitives[i];
				std::swap(primitives.begin() + i, primitives.end());
				primitives.pop_back();
			}
			//Check primitives rendering state
			else if (primitives[i]->renderState)
			{
				batchFound = false;
				//Update worldVertexArray
				primitives[i]->updateVertices();
				//Find a suitable batch for it
				for (unsigned j = 0; j < primitiveBatches.size(); j++)
				{
					if (*(primitiveBatches[j]) == *(primitives[i]))
					{
						primitiveBatches[j]->push(primitives[i]);
						batchFound = true;
					}
				}
				//If none found create a new one
				if (!batchFound)
				{
					primitiveBatches.push_back(new PrimitiveBatch(primitives[i]->planeDepth, primitives[i]->shaderIndex,
						primitives[i]->textureDataID, primitives[i]->drawMode, primitives[i]->lineWidth));
					primitiveBatches.back()->push(primitives[i]);
				}
				i++;
			}
			else
				i++;
		}

		//Batch Meshes
		///TODO

	//RENDERING:

		//Render MeshBatches
		for (unsigned int i = 0; i < meshBatches.size(); i++)
		{
			meshBatches[i]->render();
		}

		//Sort and Render PrimitiveBatches
		std::sort(primitiveBatches.begin(), primitiveBatches.end(), [](PrimitiveBatch* first, PrimitiveBatch* second)
		{
			return first->getPriority() < second->getPriority();
		});
		for (unsigned i = 0; i < primitiveBatches.size();)
		{
			if (!primitiveBatches[i]->render()) //If the batch is empty, delete it
			{
				delete primitiveBatches[i];
				std::swap(primitiveBatches.begin() + i, primitiveBatches.end());
				primitiveBatches.pop_back();
			}
			else
				i++;
		}
	}

	void BatchManager::clearBatches()
	{
		for (unsigned i = 0; i < primitiveBatches.size(); i++)
		{
			delete primitiveBatches[i];
		}
		for (unsigned i = 0; i < meshBatches.size(); i++)
		{
			delete meshBatches[i];
		}
	}
}
