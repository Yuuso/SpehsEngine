
#pragma once

#include "Component.h"

#include <string>


namespace spehs
{
	class GameObject;
	class Polygon;

	class Sprite : public Component
	{
	public:
		Sprite();
		Sprite(GameObject& _owner);
		~Sprite();

		Polygon* sprite;
	};
}

