
#pragma once

#include "SpehsEngine/Game/Component.h"
#include "SpehsEngine/Core/Vector.h"


namespace spehs
{
	class GameObject;

	class Transform2D : public Component
	{
	public:
		Transform2D();
		Transform2D(GameObject& _owner);
		~Transform2D();

		//Getters
		spehs::vec2 getPosition();
		float getXPosition();
		float getYPosition();
		float getRotation();
		float getScale();

		spehs::vec2 getLastSetPosition();
		float getXLastSetPosition();
		float getYLastSetPosition();
		float getLastSetRotation();


		//Setters
		void setPosition(spehs::vec2 _position);
		void setXPosition(float _xpos);
		void setYPosition(float _ypos);
		void setRotation(float _rotation);
		void setScale(float _scale);

	private:
		spehs::vec2 position = spehs::vec2(0.0f, 0.0f);
		float rotation = 0.0f;
		float scale = 1.0f;

		spehs::vec2 lastSetPosition;
		float lastSetRotation;
	};
}
