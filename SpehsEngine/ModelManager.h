#pragma once

#include "Vertex.h"

#include <glm/vec3.hpp>

#include <string>
#include <unordered_map>


typedef unsigned short GLushort;

namespace SpehsEngine
{
	struct ModelData
	{

	};

	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		void loadOBJ(const std::string &_filepath, Vertex* vertexArray, std::vector<GLushort> &_elementArray, glm::vec3* _normalArray);

	private:
		std::unordered_map<size_t, ModelData*> modelDataMap;
	};
	extern ModelManager* modelManager;
}