
#pragma once

#include "Component.h"

#include <glm/vec2.hpp>


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
		glm::vec2 getPosition();
		float getXPosition();
		float getYPosition();
		float getRotation();
		float getScale();

		glm::vec2 getLastSetPosition();
		float getXLastSetPosition();
		float getYLastSetPosition();
		float getLastSetRotation();


		//Setters
		void setPosition(glm::vec2 _position);
		void setXPosition(float _xpos);
		void setYPosition(float _ypos);
		void setRotation(float _rotation);
		void setScale(float _scale);

	private:
		glm::vec2 position = glm::vec2(0.0f);
		float rotation = 0.0f;
		float scale = 1.0f;

		glm::vec2 lastSetPosition;
		float lastSetRotation;
	};
}
