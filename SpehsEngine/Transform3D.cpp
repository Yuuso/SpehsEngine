
#include "Transform3D.h"


namespace spehs
{
	Transform3D::Transform3D()
	{
	}
	Transform3D::~Transform3D()
	{
	}


	glm::vec3 Transform3D::getPosition()
	{
		return position;
	}
	float Transform3D::getXPosition()
	{
		return position.x;
	}
	float Transform3D::getYPosition()
	{
		return position.y;
	}
	float Transform3D::getZPosition()
	{
		return position.z;
	}
	glm::vec3 Transform3D::getRotation()
	{
		return rotation;
	}
	float Transform3D::getYaw()
	{
		return rotation.x;
	}
	float Transform3D::getPitch()
	{
		return rotation.y;
	}
	float Transform3D::getRoll()
	{
		return rotation.z;
	}
	float Transform3D::getScale()
	{
		return scale;
	}

	void Transform3D::setPosition(const glm::vec3& _position)
	{
		position = _position;
	}
	void Transform3D::setXPosition(const float _xpos)
	{
		position.x = _xpos;
	}
	void Transform3D::setYPosition(const float _ypos)
	{
		position.y = _ypos;
	}
	void Transform3D::setZPosition(const float _zpos)
	{
		position.z = _zpos;
	}
	void Transform3D::setRotation(const glm::vec3& _rotation)
	{
		rotation = _rotation;
	}
	void Transform3D::setPitch(const float _pitch)
	{
		rotation.x = _pitch;
	}
	void Transform3D::setYaw(const float _yaw)
	{
		rotation.y = _yaw;
	}
	void Transform3D::setRoll(const float _roll)
	{
		rotation.z = _roll;
	}
	void Transform3D::setScale(const float _scale)
	{
		scale = _scale;
	}
}
