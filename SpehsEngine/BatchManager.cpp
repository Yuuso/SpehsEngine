
#include "BatchManager.h"
#include "Exceptions.h"
#include "Primitive.h"
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
	
	static BatchManager* batchSectionCurrentBatch(nullptr);
	void BatchManager::beginSection()
	{
		//Record previously active batch manager and activate this
		previousSections.emplace(getActiveBatchManager());
		setActiveBatchManager(this);

		//Mark this batch as the currently running batch
		batchSectionCurrentBatch = this;
	}
	void BatchManager::endSection()
	{
		if (previousSections.empty())
		{
			spehs::exceptions::warning("A batch manager (" + name + ") called endSection()! All sections have already been ended!");
			return;
		}
		if (batchSectionCurrentBatch != this)
			spehs::exceptions::unexpectedError("A batch manager (" + batchSectionCurrentBatch->name + ") did not end its batch section! (BatchManager::endSection() was never called for a batch manager that began a section)");

		//Set currently active section pointer
		batchSectionCurrentBatch = previousSections.top();
		previousSections.pop();
		
		//Restore active batch manager to the one active when beginSection() was called
		setActiveBatchManager(batchSectionCurrentBatch);
	}

	BatchManager::BatchManager(Camera2D* _camera, const std::string _name) : name(_name)
	{
		camera2D = _camera;
	}
	BatchManager::~BatchManager()
	{
		//Delete batches
		clearBatches();

		int cleanUpPrimitives = primitives.size();
		//Delete Primitives
		for (unsigned i = 0; i < primitives.size(); i++)
		{
			if (primitives[i]->readyForDelete)
				cleanUpPrimitives--;
			delete primitives[i];
		}

		if (cleanUpPrimitives != 0)
			exceptions::warning("BatchManager cleaned up " + std::to_string(cleanUpPrimitives) + " primitives!");

		if (getActiveBatchManager() == this)
			resetActiveBatchManager();
	}


	Polygon* BatchManager::createPolygon(const int &_shapeID, const PlaneDepth &_planeDepth, const float &_width, const float &_height)
	{
		primitives.push_back(new Polygon(_shapeID, _planeDepth, _width, _height));
		return primitives.back()->getPolygonPtr();
	}
	Polygon* BatchManager::createPolygon(std::vector<Vertex> _vertexData, const PlaneDepth &_planeDepth, const float &_width, const float &_height)
	{
		primitives.push_back(new Polygon(_vertexData, _planeDepth, _width, _height));
		return primitives.back()->getPolygonPtr();
	}
	Polygon* BatchManager::createPolygon(std::vector<Vertex> _vertexData, const float &_width, const float &_height)
	{
		primitives.push_back(new Polygon(_vertexData, _width, _height));
		return primitives.back()->getPolygonPtr();
	}
	Polygon* BatchManager::createPolygon(std::vector<spehs::vec2> _cuspData, const PlaneDepth &_planeDepth, const float &_width, const float &_height)
	{
		primitives.push_back(new Polygon(_cuspData, _planeDepth, _width, _height));
		return primitives.back()->getPolygonPtr();
	}

	Line* BatchManager::createLine(const spehs::vec2 &_startPoint, const spehs::vec2 &_endPoint, const PlaneDepth &_planeDepth)
	{
		primitives.push_back(new Line(_startPoint, _endPoint, _planeDepth));
		return primitives.back()->getLinePtr();
	}
	Line* BatchManager::createLine(const PlaneDepth &_planeDepth)
	{
		primitives.push_back(new Line(_planeDepth));
		return primitives.back()->getLinePtr();
	}

	Point* BatchManager::createPoint(const PlaneDepth &_planeDepth)
	{
		primitives.push_back(new Point(_planeDepth));
		return primitives.back()->getPointPtr();
	}
	
	Text* BatchManager::createText(const PlaneDepth &_planeDepth)
	{
		texts.push_back(new Text(_planeDepth));
		return texts.back();
	}
	Text* BatchManager::createText(const std::string &_string, const PlaneDepth &_planeDepth)
	{
		texts.push_back(new Text(_string, _planeDepth));
		return texts.back();
	}



	void BatchManager::render()
	{
		bool batchFound = false;

	//BATCHING:

		//Batch primitives
		for (unsigned i = 0; i < primitives.size();)
		{
			//Check if user has requested for deletion
			if (primitives[i]->readyForDelete)
			{
				delete primitives[i];
				primitives[i] = primitives.back();
				primitives.pop_back();
			}
			//Check primitives rendering state
			else if (primitives[i]->renderState)
			{
				batchFound = false;
				//Update worldVertexArray
				primitives[i]->updateVertices();
				//Find a suitable batch for it
				for (unsigned j = 0; j < batches.size(); j++)
				{
					if (batches[j]->check(*(primitives[i])))
					{
						batches[j]->push(primitives[i]);
						batchFound = true;
						break;
					}
				}
				//If none found create a new one
				if (!batchFound)
				{
					batches.push_back(new PrimitiveBatch(primitives[i]->cameraMatrixState, primitives[i]->planeDepth, primitives[i]->blending, primitives[i]->shaderIndex,
						primitives[i]->textureDataID, primitives[i]->drawMode, primitives[i]->lineWidth));
					batches.back()->push(primitives[i]);
				}
				i++;
			}
			else
				i++;
		}
		//Batch texts
		for (unsigned i = 0; i < texts.size();)
		{
			//Check if user has requested for deletion
			if (texts[i]->readyForDelete)
			{
				delete texts[i];
				texts[i] = texts.back();
				texts.pop_back();
			}
			//Check texts rendering state
			else if (texts[i]->renderState)
			{
				batchFound = false;
				//Update worldVertexArray
				texts[i]->update();
				//Find a suitable batch for it
				for (unsigned j = 0; j < batches.size(); j++)
				{
					if (batches[j]->check(*(texts[i])))
					{
						batches[j]->push(texts[i]);
						batchFound = true;
						break;
					}
				}
				//If none found create a new one
				if (!batchFound)
				{
					batches.push_back(new TextBatch(texts[i]->cameraMatrixState, texts[i]->planeDepth, texts[i]->shaderIndex));
					batches.back()->push(texts[i]);
				}
				i++;
			}
			else
				i++;
		}


	//RENDERING:

		//Sort and Render batches
		std::sort(batches.begin(), batches.end(), [](Batch* first, Batch* second)
		{
			return first->getPriority() < second->getPriority();
		});
		for (unsigned i = 0; i < batches.size();)
		{
			if (!batches[i]->render(camera2D)) //If the batch is empty, delete it
			{
				delete batches[i];
				batches.erase(batches.begin() + i);
			}
			else
			{
				i++;
			}
		}
	}

	void BatchManager::clearBatches()
	{
		for (unsigned i = 0; i < batches.size(); i++)
		{
			delete batches[i];
		}
	}
}
