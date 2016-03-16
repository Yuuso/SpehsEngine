
#include "BatchManager.h"
#include "Primitive.h"
#include "Mesh.h"
#include "Batch.h"
#include "Polygon.h"
#include "Line.h"
#include "Point.h"

#include <algorithm>


namespace spehs
{
	BatchManager* activeBatchManager;
	void setActiveBatchManager(BatchManager* _active)
	{
		activeBatchManager = _active;
	}
	void resetActiveBatchManager()
	{
		activeBatchManager = nullptr;
	}
	BatchManager* getActiveBatchManager()
	{
		return activeBatchManager;
	}


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



	Mesh* BatchManager::createMesh()
	{
		//TODO
		return nullptr;
	}

	Polygon* BatchManager::createPolygon(const int &_shapeID, const PlaneDepth &_planeDepth, const float &_width, const float &_height)
	{
		primitives.push_back(new Polygon(_shapeID, _planeDepth, _width, _height));
		return primitives.back()->getPolygonPtr();
	}
	Polygon* BatchManager::createPolygon(Vertex* _vertexData, const int &_numVertices, const PlaneDepth &_planeDepth, const float &_width, const float &_height)
	{
		primitives.push_back(new Polygon(_vertexData, _numVertices, _planeDepth, _width, _height));
		return primitives.back()->getPolygonPtr();
	}
	Polygon* BatchManager::createPolygon(Vertex* _vertexData, const int &_numVertices, const float &_width, const float &_height)
	{
		primitives.push_back(new Polygon(_vertexData, _numVertices, _width, _height));
		return primitives.back()->getPolygonPtr();
	}

	Line* BatchManager::createLine(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth)
	{
		primitives.push_back(new Line(_startPoint, _endPoint, _planeDepth));
		return primitives.back()->getLinePtr();
	}
	Line* BatchManager::createLine(const glm::vec3 &_startPoint, const glm::vec3 &_endPoint)
	{
		primitives.push_back(new Line(_startPoint, _endPoint));
		return primitives.back()->getLinePtr();
	}

	Point* BatchManager::createPoint(const PlaneDepth &_planeDepth)
	{
		primitives.push_back(new Point(_planeDepth));
		return primitives.back()->getPointPtr();
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
					primitiveBatches.push_back(new PrimitiveBatch(this, primitives[i]->cameraMatrixState, primitives[i]->planeDepth, primitives[i]->shaderIndex,
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
		for (unsigned i = 0; i < primitiveBatches.size();i++)
		{
			if (!primitiveBatches[i]->render()) //If the batch is empty, delete it
			{
				delete primitiveBatches[i];
				primitiveBatches.erase(primitiveBatches.begin() + i);
			}
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
