
#include "Transform2D.h"
#include "GameObject.h"


namespace spehs
{
	Transform2D::Transform2D() : Component()
	{

	}
	Transform2D::Transform2D(GameObject* _owner) : Component(_owner)
	{
		
	}
	Transform2D::~Transform2D()
	{

	}


	glm::vec2 Transform2D::getPosition()
	{
		return position;
	}

	float Transform2D::getXPosition()
	{
		return position.x;
	}

	float Transform2D::getYPosition()
	{
		return position.y;
	}

	float Transform2D::getScale()
	{
		return scale;
	}

	float Transform2D::getRotation()
	{
		return rotation;
	}

	void Transform2D::setPosition(glm::vec2 _position)
	{
		position = _position;
	}

	void Transform2D::setXPosition(float _xpos)
	{
		position.x = _xpos;
	}

	void Transform2D::setYPosition(float _ypos)
	{
		position.y = _ypos;
	}

	void Transform2D::setRotation(float _rotation)
	{
		rotation = _rotation;
	}

	void Transform2D::setScale(float _scale)
	{
		scale = _scale;
	}
}
