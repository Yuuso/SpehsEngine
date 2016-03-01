
#include "Batch.h"
#include "BatchObject.h"
#include "Console.h"

#include <GL/glew.h>


namespace SpehsEngine
{
	Batch::Batch()
	{
		shaderIndex = -1;
		textureDataID = 0;
		drawMode = 0;
	}
	Batch::~Batch()
	{
		if (batchObjects.size() != 0)
		{
			console->fatalError("Trying to delete batch that is not empty!");
		}
	}


	void Batch::render()
	{
		for (unsigned i = 0; i < batchObjects.size();)
		{
			//Check if object wants to be deleted
			if (batchObjects[i]->readyForDelete)
			{
				delete batchObjects[i];
				std::swap(batchObjects.begin() + i, batchObjects.end());
				batchObjects.pop_back();
			}
			else
			{
				//Render stuff
				if (batchObjects[i]->renderState)
				{
					//
				}
				i++;
			}
		}
	}

	void Batch::push(BatchObject* _batchObject)
	{
		batchObjects.push_back(_batchObject);
	}

	void Batch::remove(BatchObject* _batchObject)
	{
		for (unsigned i = 0; i < batchObjects.size(); i++)
		{
			if (batchObjects[i] == _batchObject)
			{
				std::swap(batchObjects.begin() + i, batchObjects.end());
				batchObjects.pop_back();
				_batchObject->batch = nullptr;
				return;
			}
		}
		console->error("Given BatchObject is not found in Batch and so cannot be removed!");
	}

	void Batch::updateIndices()
	{
		//
	}
}