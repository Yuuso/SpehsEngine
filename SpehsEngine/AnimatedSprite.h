
#pragma once

#include "Sprite.h"

#include <glm/vec2.hpp>
#include <stdint.h>


namespace spehs
{
	class GameObject;
	class TextureData;
	class AnimatedSprite : public Sprite
	{
	public:
		AnimatedSprite();
		AnimatedSprite(GameObject& _owner);
		~AnimatedSprite() override;

		void update() override;

		void setTexture(TextureData* _textureDataPtr) override;
		void setTextureID(const unsigned int _textureID) override;

		void setAnimation(const glm::ivec2& _frameSize, const uint8_t _rows, const uint8_t _columns, const uint16_t _amountOfFrames, const uint16_t _startingFrame = 0);
		void setAnimationSpeed(const float _speed);

	private:
		TextureData* textureData;

		uint16_t spriteFrameSize[2];
		uint8_t frameStructure[2];
		uint16_t maxFrames;
		uint16_t currentFrame;
		float animationSpeed = 1.0f;
		float speedCounter = animationSpeed;
	};
}

