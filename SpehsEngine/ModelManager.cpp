
#include "ModelManager.h"
#include "Console.h"
#include "Mesh.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

#include <fstream>
#include <sstream>


spehs::ModelManager* modelManager;
namespace spehs
{
	ModelData::ModelData()
	{
	}
	void ModelData::loadFromData(std::vector<spehs::Vertex> _vertexArray, std::vector<GLushort> &_elementArray, std::vector<glm::vec3> &_normalArray)
	{
		//TODO: insert data into mesh
		//Figure out indexing stuff
	}


	ModelManager::ModelManager()
	{

	}
	ModelManager::~ModelManager()
	{
		clearAllModelData();
	}

	void ModelManager::loadOBJ(const std::string& _filepath, spehs::Mesh* _mesh)
	{
		//Try to find from data map
		size_t hash = std::hash<std::string>()(_filepath);
		auto it = modelDataMap.find(hash);
		if (it == modelDataMap.end())
		{
			//Data not found ->
			//Load new data
			preloadOBJ(_filepath);
			it = modelDataMap.find(hash);
		}
		it->second->loadFromData(_mesh->vertexArray, _mesh->elementArray, _mesh->normalArray);
	}

	void ModelManager::loadOBJ(const size_t& _hash, spehs::Mesh* _mesh)
	{
		auto it = modelDataMap.find(_hash);
		if (it != modelDataMap.end())
		{
			it->second->loadFromData(_mesh->vertexArray, _mesh->elementArray, _mesh->normalArray);
			return;
		}
		else
		{
			console::error("Couldn't find model data: " + std::to_string(_hash));
			return;
		}
	}

	void ModelManager::preloadOBJ(const std::string& _filepath)
	{
		//Load new data
		std::ifstream file(_filepath, std::ios::in);
		if (!file.is_open())
		{
			console::error("Failed to open OBJ file: " + _filepath);
			return;
		}

		ModelData*  data = new ModelData();
		std::string line;
		std::istringstream stringStream;
		while (std::getline(file, line))
		{
			//Vertices
			if (line.substr(0, 2) == "v ")
			{
				stringStream = std::istringstream(line.substr(2));
				spehs::Position vertex;
				stringStream >> vertex.x;
				stringStream >> vertex.y;
				stringStream >> vertex.z;
				data->vertices.push_back(spehs::Vertex(vertex));
			}
			//Normals
			else if (line.substr(0, 3) == "vn ")
			{
				stringStream = std::istringstream(line.substr(3));
				glm::vec3 normal;
				stringStream >> normal.x;
				stringStream >> normal.y;
				stringStream >> normal.z;
				data->normals.push_back(normal);
			}
			//Texture Coordinates
			else if (line.substr(0, 3) == "vt ")
			{
				stringStream = std::istringstream(line.substr(3));
				glm::vec2 uv;
				stringStream >> uv.x;
				stringStream >> uv.y;
				data->textureCoordinates.push_back(uv);
			}
			//Elements
			else if (line.substr(0, 2) == "f ")
			{
				stringStream = std::istringstream(line.substr(2));
				GLushort v, u, n, index = 0;
				for (unsigned i = 0; i < 3; i++)
				{
					while (stringStream.peek() != ' ')
					{
						switch (stringStream.peek())
						{
						case '/':
							index++;
							break;
						default:
							switch (index)
							{
							case 0:
								stringStream >> v;
								break;
							case 1:
								stringStream >> u;
								break;
							case 2:
								stringStream >> n;
								break;
							default:
								break;
							}
							break;
						}
					}
					data->vertexElements.push_back(v);
					data->textureElements.push_back(u);
					data->normalElements.push_back(n);
					index = 0;
				}
			}
		}
		size_t hash = std::hash<std::string>()(_filepath);
		modelDataMap.insert(std::pair<size_t, ModelData*>(hash, data));
	}

	void ModelManager::removeModelData(std::string _filepath)
	{
		size_t hash = std::hash<std::string>()(_filepath);
		auto it = modelDataMap.find(hash);
		if (it == modelDataMap.end())
		{
			console::error("Couldn't find model data: " + _filepath);
			return;
		}
		delete it->second;
		modelDataMap.erase(hash);
	}

	void ModelManager::removeModelData(size_t _hash)
	{
		auto it = modelDataMap.find(_hash);
		if (it == modelDataMap.end())
		{
			console::error("Couldn't find model data: " + std::to_string(_hash));
			return;
		}
		delete it->second;
		modelDataMap.erase(_hash);
	}

	void ModelManager::clearAllModelData()
	{
		for (auto &it : modelDataMap)
		{
			delete it.second;
		}
		modelDataMap.clear();
	}
}