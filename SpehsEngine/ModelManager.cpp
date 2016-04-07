
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
	int checkArrayForElements(std::vector<spehs::Vertex3D>& _array, const spehs::Position& _position, const glm::vec2& _textureCoord, const glm::vec3& _normal)
	{
		for (unsigned i = 0; i < _array.size(); i++)
		{
			if (_array[i].position == _position && _array[i].uv == UV(_textureCoord.x, _textureCoord.y) && _array[i].normal == Position(_normal.x, _normal.y, _normal.z))
			{
				//Element found -> return position
				return i;
			}
		}
		//Element not found
		return -1;
	}

	ModelData::ModelData()
	{
	}
	void ModelData::loadFromData(std::vector<spehs::Vertex3D> &_vertexArray, std::vector<GLushort> &_elementArray)
	{
		int index;
		for (unsigned i = 0; i < vertexElements.size(); i++)
		{
			//Check if there is already a vertex with the wanted attributes (position, uv, normal)
			//if not -> create a new one
			//What ever is the position of that vertex in the array, add that to the element array
			if (textureCoordinates.size() == 0)
			{
				if (normals.size() == 0)
				{
					index = checkArrayForElements(_vertexArray, vertices[vertexElements[i]].position, glm::vec2(0.0f), glm::vec3(0.0f));
					if (index < 0)
					{
						_vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
							UV(),
							Position()));
						_elementArray.push_back(_vertexArray.size());
					}
					else
					{
						_elementArray.push_back(index);
					}
				}

				index = checkArrayForElements(_vertexArray, vertices[vertexElements[i]].position, glm::vec2(0.0f), normals[normalElements[i]]);
				if (index < 0)
				{
					_vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
						UV(),
						Position(normals[normalElements[i]].x, normals[normalElements[i]].y, normals[normalElements[i]].z)));
					_elementArray.push_back(_vertexArray.size());
				}
				else
				{
					_elementArray.push_back(index);
				}
			}
			else if (normals.size() == 0)
			{
				index = checkArrayForElements(_vertexArray, vertices[vertexElements[i]].position, textureCoordinates[textureElements[i]], glm::vec3(0.0f));
				if (index < 0)
				{
					_vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
						UV(textureCoordinates[textureElements[i]].x, textureCoordinates[textureElements[i]].y),
						Position()));
					_elementArray.push_back(_vertexArray.size());
				}
				else
				{
					_elementArray.push_back(index);
				}
			}
			else
			{
				index = checkArrayForElements(_vertexArray, vertices[vertexElements[i]].position, textureCoordinates[textureElements[i]], normals[normalElements[i]]);
				if (index < 0)
				{
					_vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
						UV(textureCoordinates[textureElements[i]].x, textureCoordinates[textureElements[i]].y),
						Position(normals[normalElements[i]].x, normals[normalElements[i]].y, normals[normalElements[i]].z)));
					_elementArray.push_back(_vertexArray.size());
				}
				else
				{
					_elementArray.push_back(index);
				}
			}
		}
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
		it->second->loadFromData(_mesh->vertexArray, _mesh->elementArray);
	}

	void ModelManager::loadOBJ(const size_t& _hash, spehs::Mesh* _mesh)
	{
		auto it = modelDataMap.find(_hash);
		if (it != modelDataMap.end())
		{
			it->second->loadFromData(_mesh->vertexArray, _mesh->elementArray);
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

		ModelData* data = new ModelData();
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
				data->vertices.push_back(spehs::Vertex3D(vertex));
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
				std::string subString = line.substr(2);
				GLushort v = 0, u = 0, n = 0;
				size_t pos;
				for (unsigned w = 0; w < 3; w++)
				{
					for (unsigned i = 0; i < 3; i++)
					{
						if (i < 2)
						{
							pos = subString.find('/');
							stringStream = std::istringstream(subString.substr(0, pos));
						}
						else
						{
							pos = subString.find(' ');
							stringStream = std::istringstream(subString);
						}

						switch (i)
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
						}

						subString = subString.substr(pos + 1);
					}
					data->vertexElements.push_back(v - 1);
					data->textureElements.push_back(u - 1);
					data->normalElements.push_back(n - 1);
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