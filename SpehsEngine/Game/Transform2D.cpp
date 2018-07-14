#include "stdafx.h"

#include "SpehsEngine/Game/Transform2D.h"
#include "SpehsEngine/Game/GameObject.h"


namespace se
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


	se::vec2 Transform2D::getPosition()
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

	float Transform2D::getRotation()
	{
		return rotation;
	}

	float Transform2D::getScale()
	{
		return scale;
	}
	
	se::vec2 Transform2D::getLastSetPosition()
	{
		return lastSetPosition;
	}

	float Transform2D::getXLastSetPosition()
	{
		return lastSetPosition.x;
	}

	float Transform2D::getYLastSetPosition()
	{
		return lastSetPosition.y;
	}

	float Transform2D::getLastSetRotation()
	{
		return lastSetRotation;
	}


	void Transform2D::setPosition(se::vec2 _position)
	{
#ifdef _DEBUG
		if (_position != _position)
			log::error("Position value corrupted!");
#endif
		lastSetPosition = position;
		position = _position;
	}

	void Transform2D::setXPosition(float _xpos)
	{
#ifdef _DEBUG
		if (_xpos != _xpos)
			log::error("Position value corrupted!");
#endif
		lastSetPosition.x = position.x;
		position.x = _xpos;
	}

	void Transform2D::setYPosition(float _ypos)
	{
#ifdef _DEBUG
		if (_ypos != _ypos)
			log::error("Position value corrupted!");
#endif
		lastSetPosition.y = position.y;
		position.y = _ypos;
	}

	void Transform2D::setRotation(float _rotation)
	{
#ifdef _DEBUG
		if (_rotation != _rotation)
			log::error("Rotation value corrupted!");
#endif
		lastSetRotation = rotation;
		rotation = _rotation;
	}

	void Transform2D::setScale(float _scale)
	{
#ifdef _DEBUG
		if (_scale != _scale)
			log::error("Scale value corrupted!");
#endif
		scale = _scale;
	}
}
