#include "stdafx.h"

#include "SpehsEngine/Rendering/ModelManager.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Batch3D.h"
#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Core/Exceptions.h"

#include <algorithm>


namespace se
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
		clearBatches();
		size_t cleanUpMeshes = meshes.size();
		for (size_t i = 0; i < meshes.size(); i++)
		{
			if (meshes[i]->readyForDelete)
				cleanUpMeshes--;
			delete meshes[i];
		}

		if (cleanUpMeshes != 0u)
			exceptions::warning("BatchManager cleaned up " + std::to_string(cleanUpMeshes) + " meshes!");
	}

	Mesh* BatchManager3D::createMesh()
	{
		meshes.push_back(new Mesh(*this));
		return meshes.back();
	}

	Mesh* BatchManager3D::createMesh(const std::string& _filepath)
	{
		meshes.push_back(new Mesh(*this, _filepath));
		return meshes.back();
	}

	void BatchManager3D::render()
	{
		static bool batchFound;

		// Batch Meshes
		for (size_t i = 0; i < meshes.size();)
		{
			// Check if user has requested for deletion
			if (meshes[i]->readyForDelete)
			{
				delete meshes[i];
				if (meshes[i] != meshes.back())
					meshes[i] = meshes.back();
				meshes.pop_back();
			}
			// Check meshes rendering state
			else if (meshes[i]->renderState)
			{
				batchFound = false;
				// Update worldVertexArray
				meshes[i]->updateVertices();
				// Find a suitable batch for it
				for (size_t j = 0; j < meshBatches.size(); j++)
				{
					if (meshBatches[j]->check(*(meshes[i])))
					{
						meshBatches[j]->push(meshes[i]);
						batchFound = true;
						j = meshBatches.size();
					}
				}
				// If none found create a new one
				if (!batchFound)
				{
					meshBatches.push_back(new MeshBatch(*this, meshes[i]->shaderIndex, meshes[i]->textureDataID, meshes[i]->depthTest, meshes[i]->blending, meshes[i]->backFaceCulling));
					// TODO don't do this
					if (!meshBatches.back()->check(*(meshes[i]))) // Increases size of new batch if needed, should never fail
						exceptions::fatalError("Cannot push mesh to mesh batch!");
					meshBatches.back()->push(meshes[i]);
				}
				i++;
			}
			else
				i++;
		}

		// Sort batches
		std::sort(meshBatches.begin(), meshBatches.end(), [](MeshBatch* first, MeshBatch* second) // written out for clarity
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

		// Render MeshBatches
		for (unsigned int i = 0; i < meshBatches.size(); i++)
		{
			if (!meshBatches[i]->render()) // If the batch is empty, delete it
			{
				delete meshBatches[i];
				meshBatches.erase(meshBatches.begin() + i);
			}
		}
	}

	void BatchManager3D::clearBatches()
	{
		for (unsigned i = 0; i < meshBatches.size(); i++)
		{
			delete meshBatches[i];
		}
	}
}