
#include "ModelManager.h"
#include "Console.h"
#include "Exceptions.h"	
#include "Mesh.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

#include <fstream>


spehs::ModelManager* modelManager;
namespace spehs
{
	int checkArrayForElements(const std::vector<spehs::Vertex3D>& _array, const spehs::Position& _position, const glm::vec2& _textureCoord, const glm::vec3& _normal)
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
		std::vector<spehs::Vertex3D> vertexArray;
		std::vector<GLushort> elementArray;
		for (unsigned i = 0; i < vertexElements.size(); i++)
		{
			//Check if there is already a vertex with the wanted attributes (position, uv, normal)
			//if not -> create a new one
			//What ever is the position of that vertex in the array, add that to the element array
			if (textureCoordinates.size() == 0)
			{
				if (normals.size() == 0)
				{
					index = checkArrayForElements(vertexArray, vertices[vertexElements[i]].position, glm::vec2(0.0f), glm::vec3(0.0f));
					if (index < 0)
					{
						vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
							UV(),
							Position()));
						elementArray.push_back(vertexArray.size() - 1);
					}
					else
					{
						elementArray.push_back(index);
					}
				}
				else
				{
					index = checkArrayForElements(vertexArray, vertices[vertexElements[i]].position, glm::vec2(0.0f), normals[normalElements[i]]);
					if (index < 0)
					{
						vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
							UV(),
							Position(normals[normalElements[i]].x, normals[normalElements[i]].y, normals[normalElements[i]].z)));
						elementArray.push_back(vertexArray.size() - 1);
					}
					else
					{
						elementArray.push_back(index);
					}
				}
			}
			else if (normals.size() == 0)
			{
				index = checkArrayForElements(vertexArray, vertices[vertexElements[i]].position, textureCoordinates[textureElements[i]], glm::vec3(0.0f));
				if (index < 0)
				{
					vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
						UV(textureCoordinates[textureElements[i]].x, textureCoordinates[textureElements[i]].y),
						Position()));
					elementArray.push_back(vertexArray.size() - 1);
				}
				else
				{
					elementArray.push_back(index);
				}
			}
			else
			{
				index = checkArrayForElements(vertexArray, vertices[vertexElements[i]].position, textureCoordinates[textureElements[i]], normals[normalElements[i]]);
				if (index < 0)
				{
					vertexArray.push_back(Vertex3D(vertices[vertexElements[i]].position,
						UV(textureCoordinates[textureElements[i]].x, textureCoordinates[textureElements[i]].y),
						Position(normals[normalElements[i]].x, normals[normalElements[i]].y, normals[normalElements[i]].z)));
					elementArray.push_back(vertexArray.size() - 1);
				}
				else
				{
					elementArray.push_back(index);
				}
			}
		}
		_vertexArray.clear();
		_elementArray.clear();
		_vertexArray = vertexArray;
		_elementArray = elementArray;
	}


	ModelManager::ModelManager()
	{

	}
	ModelManager::~ModelManager()
	{
		clearAllModelData();
	}


	void ModelManager::loadModel(const std::string& _filepath, spehs::Mesh* _mesh)
	{
		//Check for special cases
		if (_filepath == "Cube")
		{
			size_t hash = std::hash<std::string>()(_filepath);
			auto it = modelDataMap.find(hash);
			if (it == modelDataMap.end())
			{
				//Data not found ->
				//Load new data
				loadOBJFromStringStream(loadCube(), hash);
				it = modelDataMap.find(hash);
			}
			it->second->loadFromData(_mesh->vertexArray, _mesh->elementArray);
			return;
		}
		else if (_filepath == "InvertedCube")
		{
			size_t hash = std::hash<std::string>()(_filepath);
			auto it = modelDataMap.find(hash);
			if (it == modelDataMap.end())
			{
				//Data not found ->
				//Load new data
				loadOBJFromStringStream(loadInvertedCube(), hash);
				it = modelDataMap.find(hash);
			}
			it->second->loadFromData(_mesh->vertexArray, _mesh->elementArray);
			return;
		}

		//Check the file type of the given model file
		std::string fileEnding = "";
		auto it = _filepath.end() - 1;
		while (*it != '.')
		{
			fileEnding.push_back(*it);
			it--;
		}

		//Search for supported file types
		if (fileEnding == "jbo") //.obj files
		{
			loadOBJ(_filepath, _mesh);
			return;
		}
		else
		{
			exceptions::fatalError("The models file type is not supported!");
		}
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

		loadOBJFromFile(file, std::hash<std::string>()(_filepath));

		file.close();
	}

	void ModelManager::removeModelData(const std::string& _filepath)
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

	void ModelManager::removeModelData(const size_t& _hash)
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

	std::istringstream ModelManager::loadCube()
	{
		std::string file =
		{
			"v 1.000000 -1.000000 -1.000000\n"
			"v 1.000000 -1.000000 1.000000\n"
			"v -1.000000 -1.000000 1.000000\n"
			"v -1.000000 -1.000000 -1.000000\n"
			"v 1.000000 1.000000 -1.000000\n"
			"v 1.000000 1.000000 1.000000\n"
			"v -1.000000 1.000000 1.000000\n"
			"v -1.000000 1.000000 -1.000000\n"
			"vn 0.000000 1.000000 0.000000\n"
			"vn 0.000000 -1.000000 0.000000\n"
			"vn -1.000000 -0.000000 -0.000000\n"
			"vn 0.000000 -0.000000 -1.000000\n"
			"vn 1.000000 0.000000 0.000000\n"
			"vn -0.000000 -0.000000 1.000000\n"
			"f 4//1 3//1 2//1\n"
			"f 6//2 7//2 8//2\n"
			"f 2//3 6//3 5//3\n"
			"f 3//4 7//4 6//4\n"
			"f 3//5 4//5 8//5\n"
			"f 8//6 4//6 1//6\n"
			"f 1//1 4//1 2//1\n"
			"f 5//2 6//2 8//2\n"
			"f 1//3 2//3 5//3\n"
			"f 2//4 3//4 6//4\n"
			"f 7//5 3//5 8//5\n"
			"f 5//6 8//6 1//6\n"
		};
		return std::istringstream(file);
	}

	std::istringstream ModelManager::loadInvertedCube()
	{
		std::string file =
		{
			"v 1.000000 -1.000000 -1.000000\n"
			"v 1.000000 -1.000000 1.000000\n"
			"v -1.000000 -1.000000 1.000000\n"
			"v -1.000000 -1.000000 -1.000000\n"
			"v 1.000000 1.000000 -1.000000\n"
			"v 1.000000 1.000000 1.000000\n"
			"v -1.000000 1.000000 1.000000\n"
			"v -1.000000 1.000000 -1.000000\n"
			"vn 0.000000 1.000000 0.000000\n"
			"vn 0.000000 -1.000000 0.000000\n"
			"vn -1.000000 -0.000000 -0.000000\n"
			"vn 0.000000 -0.000000 -1.000000\n"
			"vn 1.000000 0.000000 0.000000\n"
			"vn -0.000000 -0.000000 1.000000\n"
			"f 4//1 3//1 2//1\n"
			"f 6//2 7//2 8//2\n"
			"f 2//3 6//3 5//3\n"
			"f 3//4 7//4 6//4\n"
			"f 3//5 4//5 8//5\n"
			"f 8//6 4//6 1//6\n"
			"f 1//1 4//1 2//1\n"
			"f 5//2 6//2 8//2\n"
			"f 1//3 2//3 5//3\n"
			"f 2//4 3//4 6//4\n"
			"f 7//5 3//5 8//5\n"
			"f 5//6 8//6 1//6\n"
		};
		return std::istringstream(file);
	}


	void ModelManager::loadOBJFromFile(std::ifstream& _data, const size_t& _hash)
	{
		ModelData* data = new ModelData();
		std::string line;
		std::istringstream stringStream;

		while (std::getline(_data, line))
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
		modelDataMap.insert(std::pair<size_t, ModelData*>(_hash, data));
	}
	void ModelManager::loadOBJFromStringStream(std::istringstream& _data, const size_t& _hash)
	{
		ModelData* data = new ModelData();
		std::string line;
		std::istringstream stringStream;

		while (std::getline(_data, line))
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
		modelDataMap.insert(std::pair<size_t, ModelData*>(_hash, data));
	}
}