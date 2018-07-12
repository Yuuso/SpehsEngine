#include "stdafx.h"
#include "SpehsEngine/Game/AnimatedSprite.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Time.h"



namespace se
{
	AnimatedSprite::AnimatedSprite(BatchManager& _batchManager)
		: Sprite(_batchManager)
	{

	}

	AnimatedSprite::AnimatedSprite(BatchManager& _batchManager, GameObject& _owner)
		: Sprite(_batchManager, _owner)
	{

	}

	AnimatedSprite::~AnimatedSprite()
	{

	}

	void AnimatedSprite::update(const time::Time deltaTime)
	{
		speedCounter -= deltaTime;
		if (speedCounter <= time::Time::zero)
		{
			if (currentFrame < maxFrames - 1)
			{
				currentFrame++;
			}
			else
			{
				currentFrame = 0;
			}
			speedCounter = animationSpeed;
		}
		
		se::vec2 currentFramePosition(currentFrame, 0);
		while (currentFramePosition.x >= frameStructure[0])
		{
			currentFramePosition.x -= frameStructure[0];
			currentFramePosition.y++;
		}
		
		sprite->worldVertexArray[0].uv.u = float(spriteFrameSize[0]) / float(textureData->width) * currentFramePosition.x;
		sprite->worldVertexArray[0].uv.v = float(spriteFrameSize[1]) / float(textureData->height) * currentFramePosition.y;
		sprite->worldVertexArray[1].uv.u = sprite->worldVertexArray[0].uv.u + float(spriteFrameSize[0]) / float(textureData->width);
		sprite->worldVertexArray[1].uv.v = sprite->worldVertexArray[0].uv.v;
		sprite->worldVertexArray[2].uv.u = sprite->worldVertexArray[0].uv.u + float(spriteFrameSize[0]) / float(textureData->width);
		sprite->worldVertexArray[2].uv.v = sprite->worldVertexArray[0].uv.v + float(spriteFrameSize[1]) / float(textureData->height);
		sprite->worldVertexArray[3].uv.u = sprite->worldVertexArray[0].uv.u;
		sprite->worldVertexArray[3].uv.v = sprite->worldVertexArray[0].uv.v + float(spriteFrameSize[1]) / float(textureData->height);

		Sprite::update(deltaTime);
	}

	void AnimatedSprite::setTexture(TextureData* _textureDataPtr)
	{
		textureData = _textureDataPtr;
		Sprite::setTexture(_textureDataPtr);
	}

	void AnimatedSprite::setTextureID(const unsigned int _textureID)
	{
		textureData = textureManager.getTextureData(_textureID);
		Sprite::setTextureID(_textureID);
	}

	void AnimatedSprite::setAnimation(const se::ivec2& _frameSize, const uint8_t _rows, const uint8_t _columns, const uint16_t _amountOfFrames, const uint16_t _startingFrame)
	{
		spriteFrameSize[0] = _frameSize.x;
		spriteFrameSize[1] = _frameSize.y;
		frameStructure[0] = _columns;
		frameStructure[1] = _rows;
		maxFrames = _amountOfFrames;
		currentFrame = _startingFrame;
	}

	void AnimatedSprite::setAnimationSpeed(const time::Time& _speed)
	{
		animationSpeed = _speed;
		speedCounter = _speed;
	}
}