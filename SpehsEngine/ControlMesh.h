
#pragma once

#include "Component.h"

#include <string>


namespace spehs
{
	class Mesh;

	class ControlMesh : public Component
	{
	public:
		ControlMesh();
		ControlMesh(Mesh* _mesh);
		ControlMesh(const std::string &_filepath);
		~ControlMesh();

		//Movement
		//Correction of axes
		//Get functions for axes

	private:
		Mesh* mesh;
	};

}
