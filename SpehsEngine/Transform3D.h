
#pragma once

#include "Component.h"

#include <glm/vec3.hpp>


namespace spehs
{
	class Transform3D : public Component
	{
	public:
		Transform3D();
		~Transform3D();

		//Getters
		glm::vec3 getPosition();
		float getXPosition();
		float getYPosition();
		float getZPosition();
		glm::vec3 getRotation();
		float getYaw();
		float getPitch();
		float getRoll();
		float getScale();


		//Setters
		void setPosition(const glm::vec3& _position);
		void setXPosition(const float _xpos);
		void setYPosition(const float _ypos);
		void setZPosition(const float _zpos);
		void setRotation(const glm::vec3& _rotation);
		void setYaw(const float _yaw);
		void setPitch(const float _pitch);
		void setRoll(const float _roll);
		void setScale(const float _scale);

	private:
		glm::vec3 position;
		glm::vec3 rotation;
		float scale;
	};
}

