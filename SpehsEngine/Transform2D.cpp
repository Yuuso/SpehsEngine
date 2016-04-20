
#include "Transform2D.h"
#include "GameObject.h"
#include "Console.h"


namespace spehs
{
	Transform2D::Transform2D() : Component()
	{

	}
	Transform2D::Transform2D(GameObject& _owner) : Component(_owner)
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
#ifdef _DEBUG
		if (_position != _position)
			console::error("Position value corrupted!");
#endif
		position = _position;
	}

	void Transform2D::setXPosition(float _xpos)
	{
#ifdef _DEBUG
		if (_xpos != _xpos)
			console::error("Position value corrupted!");
#endif
		position.x = _xpos;
	}

	void Transform2D::setYPosition(float _ypos)
	{
#ifdef _DEBUG
		if (_ypos != _ypos)
			console::error("Position value corrupted!");
#endif
		position.y = _ypos;
	}

	void Transform2D::setRotation(float _rotation)
	{
#ifdef _DEBUG
		if (_rotation != _rotation)
			console::error("Rotation value corrupted!");
#endif
		rotation = _rotation;
	}

	void Transform2D::setScale(float _scale)
	{
#ifdef _DEBUG
		if (_scale != _scale)
			console::error("Scale value corrupted!");
#endif
		scale = _scale;
	}
}
