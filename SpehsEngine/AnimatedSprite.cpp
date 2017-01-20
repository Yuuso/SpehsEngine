
#include "AnimatedSprite.h"

#include "Polygon.h"
#include "Time.h"


namespace spehs
{
	AnimatedSprite::AnimatedSprite()
	{

	}
	AnimatedSprite::AnimatedSprite(GameObject& _owner) : Sprite(_owner)
	{

	}
	AnimatedSprite::~AnimatedSprite()
	{

	}

	void AnimatedSprite::update()
	{
		speedCounter - time::getDeltaTimeAsSeconds();
		if (speedCounter <= 0.0f)
		{
			if (currentFrame < maxFrames)
			{
				currentFrame++;
			}
			else
			{
				currentFrame = 0;
			}
			speedCounter = animationSpeed;
		}

		//sprite;

		Sprite::update();
	}

	void AnimatedSprite::setAnimation(const glm::ivec2& _frameSize, const uint8_t _rows, const uint8_t _columns, const uint16_t _amountOfFrames, const uint16_t _startingFrame)
	{
		spriteFrameSize[0] = _frameSize.x;
		spriteFrameSize[1] = _frameSize.y;
		frameStructure[0] = _columns;
		frameStructure[1] = _rows;
		maxFrames = _amountOfFrames;
		currentFrame = _startingFrame;
	}
	void AnimatedSprite::setAnimationSpeed(const float _speed)
	{
		animationSpeed = _speed;
		speedCounter = _speed;
	}
}