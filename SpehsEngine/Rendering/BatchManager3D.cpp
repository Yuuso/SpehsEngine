
#include "stdafx.h"

#include "SpehsEngine/Rendering/ModelManager.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Batch3D.h"
#include "SpehsEngine/Rendering/Model.h"
#include "SpehsEngine/Rendering/Plane3D.h"
#include "SpehsEngine/Rendering/Line3D.h"
#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/Window.h"

#include <algorithm>


namespace se
{
	namespace rendering
	{
		BatchManager3D::BatchManager3D(Window& _window, ModelManager& _modelManager, ShaderManager& _shaderManager, Camera3D& _camera, const std::string& _name)
			: window(_window)
			, modelManager(_modelManager)
			, textureManager(*_window.getTextureManager())
			, shaderManager(_shaderManager)
			, camera3D(_camera)
			, name(_name)
		{

		}

		BatchManager3D::~BatchManager3D()
		{
			for (unsigned i = 0; i < batches.size(); i++)
				delete batches[i];
		}

		void BatchManager3D::add(Model& _model)
		{
			se_assert(!_model.batchManager);
			for (size_t i = 0; i < _model.meshes.size(); i++)
				addMesh(*_model.meshes[i]);
			_model.batchManager = this;
		}
		void BatchManager3D::remove(Model& _model)
		{
			se_assert(_model.batchManager);
			for (size_t i = 0; i < _model.meshes.size(); i++)
				removeMesh(*_model.meshes[i]);
			_model.batchManager = nullptr;
		}

		void BatchManager3D::add(Plane3D& _plane)
		{
			se_assert(!_plane.batchManager);
			addMesh(*_plane.mesh);
			_plane.batchManager = this;
		}
		void BatchManager3D::remove(Plane3D& _plane)
		{
			se_assert(_plane.batchManager);
			removeMesh(*_plane.mesh);
			_plane.batchManager = nullptr;
		}

		void BatchManager3D::add(Line3D& _line)
		{
			se_assert(!_line.batchManager);
			addMesh(*_line.mesh);
			_line.batchManager = this;
		}
		void BatchManager3D::remove(Line3D& _line)
		{
			se_assert(_line.batchManager);
			removeMesh(*_line.mesh);
			_line.batchManager = nullptr;
		}

		void BatchManager3D::addMesh(Mesh& _mesh)
		{
			_mesh.batchManager = this;
			meshes.push_back(MeshObject(_mesh));
		}
		void BatchManager3D::removeMesh(Mesh& _mesh)
		{
			for (size_t i = 0; i < meshes.size(); i++)
			{
				if (&_mesh == meshes[i].mesh)
				{
					if (meshes[i].batch != nullptr)
					{
						unbatch(meshes[i]);
					}
					_mesh.batchManager = nullptr;
					meshes[i] = meshes.back();
					meshes.pop_back();
					return;
				}
			}
			log::error("BatchManager3D::removeMesh: Mesh not found!");
		}

		void BatchManager3D::unbatch(MeshObject& _object)
		{
			_object.batch->remove(_object.indexInBatch, _object.sizeInBatch);
			MeshBatch* comp = _object.batch;
			_object.batch = nullptr;
			for (size_t i = 0; i < meshes.size(); i++)
			{
				if (meshes[i].batch != nullptr && meshes[i].batch == comp && meshes[i].indexInBatch.first >= _object.indexInBatch.first)
				{
					meshes[i].indexInBatch.first -= _object.sizeInBatch.first;
					meshes[i].indexInBatch.second -= _object.sizeInBatch.second;
				}
			}
		}
		void BatchManager3D::batch(MeshObject& _object, MeshBatch* _batch)
		{
			_object.indexInBatch = _batch->push(*_object.mesh);
			_object.sizeInBatch.first = _object.mesh->vertexArray.size();
			_object.sizeInBatch.second = _object.mesh->elementArray.size();
			_object.batch = _batch;
		}
		void BatchManager3D::updateMeshVertices(MeshObject& _object)
		{
			if (_object.mesh->needUpdate)
			{
				_object.batch->updateVertices(_object.indexInBatch.first, *_object.mesh);
				_object.mesh->needUpdate = false;
			}
		}

		void BatchManager3D::render()
		{
			bool batchFound;

			// Update
			for (size_t i = 0; i < meshes.size(); i++)
			{
				if (meshes[i].batch != nullptr)
				{
					if (!meshes[i].mesh->renderState || meshes[i].mesh->vertexArray.size() == 0)
						unbatch(meshes[i]);
					else
						updateMeshVertices(meshes[i]);
				}
				else if (meshes[i].mesh->renderState && meshes[i].mesh->vertexArray.size() != 0)
				{
					batchFound = false;
					for (size_t j = 0; j < batches.size(); j++)
					{
						if (batches[j]->checkCompatibility(*(meshes[i].mesh)))
						{
							batch(meshes[i], batches[j]);
							batchFound = true;
							j = batches.size();
						}
					}
					if (!batchFound)
					{
						batches.push_back(new MeshBatch(*this, meshes[i].mesh->shaderIndex, meshes[i].mesh->textureDataID, meshes[i].mesh->depthTest,
																	meshes[i].mesh->blending, meshes[i].mesh->backFaceCulling, meshes[i].mesh->drawMode));
						batch(meshes[i], batches.back());
					}
				}
			}

			// Sort
			std::sort(batches.begin(), batches.end(), [](MeshBatch* first, MeshBatch* second) // written out for clarity
			{
				if (first->getDepthTest())
				{
					if (second->getDepthTest())
					{
						if (first->getBlending())
						{
							return false;
						}
						else
						{
							if (second->getBlending())
							{
								return true;
							}
							else
							{
								return false;
							}
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					if (second->getDepthTest())
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			});

			// Render
			for (unsigned int i = 0; i < batches.size(); i++)
			{
				if (!batches[i]->render()) // If the batch is empty, delete it
				{
					delete batches[i];
					batches.erase(batches.begin() + i); // ???
				}
			}
		}
	}
}