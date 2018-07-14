#include "stdafx.h"

#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Rendering/Batch.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/Line.h"
#include "SpehsEngine/Rendering/Point.h"
#include "SpehsEngine/Rendering/Window.h"

#include <algorithm>


namespace se
{
	namespace rendering
	{
		BatchManager::BatchManager(Window& _window, ShaderManager& _shaderManager, Camera2D& _camera, const std::string& _name)
			: shaderManager(_shaderManager)
			, textureManager(*_window.getTextureManager())
			, fontManager(*_window.getFontManager())
			, window(_window)
			, camera2D(_camera)
			, name(_name)
		{

		}

		BatchManager::~BatchManager()
		{
			//Delete batches
			clearBatches();

			size_t cleanUpPrimitives = primitives.size();
			//Delete Primitives
			for (size_t i = 0; i < primitives.size(); i++)
			{
				if (primitives[i]->readyForDelete)
					cleanUpPrimitives--;
				delete primitives[i];
			}

			if (cleanUpPrimitives != 0)
				log::warning("BatchManager cleaned up " + std::to_string(cleanUpPrimitives) + " primitives!");
		}

		Polygon* BatchManager::createPolygon(const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height)
		{
			primitives.push_back(new Polygon(*this, _shapeID, _planeDepth, _width, _height));
			return primitives.back()->getPolygonPtr();
		}
		Polygon* BatchManager::createPolygon(std::vector<Vertex> _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height)
		{
			primitives.push_back(new Polygon(*this, _vertexData, _planeDepth, _width, _height));
			return primitives.back()->getPolygonPtr();
		}
		Polygon* BatchManager::createPolygon(std::vector<Vertex> _vertexData, const float _width, const float _height)
		{
			primitives.push_back(new Polygon(*this, _vertexData, _width, _height));
			return primitives.back()->getPolygonPtr();
		}
		Polygon* BatchManager::createPolygon(std::vector<glm::vec2> _cuspData, const PlaneDepth _planeDepth, const float _width, const float _height)
		{
			primitives.push_back(new Polygon(*this, _cuspData, _planeDepth, _width, _height));
			return primitives.back()->getPolygonPtr();
		}

		Line* BatchManager::createLine(const glm::vec2& _startPoint, const glm::vec2& _endPoint, const PlaneDepth _planeDepth)
		{
			primitives.push_back(new Line(*this, _startPoint, _endPoint, _planeDepth));
			return primitives.back()->getLinePtr();
		}
		Line* BatchManager::createLine(const PlaneDepth _planeDepth)
		{
			primitives.push_back(new Line(*this, _planeDepth));
			return primitives.back()->getLinePtr();
		}

		Point* BatchManager::createPoint(const PlaneDepth _planeDepth)
		{
			primitives.push_back(new Point(*this, _planeDepth));
			return primitives.back()->getPointPtr();
		}

		Text* BatchManager::createText(const PlaneDepth _planeDepth)
		{
			texts.push_back(new Text(*this, _planeDepth));
			return texts.back();
		}
		Text* BatchManager::createText(const std::string& _string, const PlaneDepth _planeDepth)
		{
			texts.push_back(new Text(*this, _string, _planeDepth));
			return texts.back();
		}



		void BatchManager::render(BatchRenderResults* results)
		{
			bool batchFound = false;

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
							batches[j]->push(*primitives[i]);
							batchFound = true;
							break;
						}
					}
					//If none found create a new one
					if (!batchFound)
					{
						batches.push_back(new PrimitiveBatch(*this, primitives[i]->cameraMatrixState, primitives[i]->planeDepth, primitives[i]->blending, primitives[i]->shaderIndex,
							primitives[i]->textureDataID, primitives[i]->drawMode, primitives[i]->lineWidth));
						batches.back()->push(*primitives[i]);
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
							batches[j]->push(*texts[i]);
							batchFound = true;
							break;
						}
					}
					//If none found create a new one
					if (!batchFound)
					{
						batches.push_back(new TextBatch(*this, texts[i]->cameraMatrixState, texts[i]->planeDepth, texts[i]->shaderIndex));
						batches.back()->push(*texts[i]);
					}
					i++;
				}
				else
					i++;
			}

			//Sort batches
			std::sort(batches.begin(), batches.end(), [](Batch* first, Batch* second)
			{
				return first->getPriority() < second->getPriority();
			});

			//Render batches
			for (unsigned i = 0; i < batches.size();)
			{
				if (!batches[i]->render(results)) //If the batch is empty, delete it
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
}