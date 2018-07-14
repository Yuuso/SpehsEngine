
#pragma once

#include "SpehsEngine/Game/Sprite.h"
#include "SpehsEngine/Core/Vector.h"

#include <stdint.h>


namespace se
{
	namespace rendering
	{
		struct TextureData;
	}
	class GameObject;

	class AnimatedSprite : public Sprite
	{
	public:
		AnimatedSprite(rendering::BatchManager& batchManager);
		AnimatedSprite(rendering::BatchManager& batchManager, GameObject& _owner);
		~AnimatedSprite() override;

		void update(const time::Time deltaTime) override;

		void setTexture(rendering::TextureData* _textureDataPtr) override;
		void setTextureID(const unsigned int _textureID) override;

		void setAnimation(const se::ivec2& _frameSize, const uint8_t _rows, const uint8_t _columns, const uint16_t _amountOfFrames, const uint16_t _startingFrame = 0);
		void setAnimationSpeed(const time::Time& _speed);

	private:
		rendering::TextureData* textureData;

		uint16_t spriteFrameSize[2];
		uint8_t frameStructure[2];
		uint16_t maxFrames;
		uint16_t currentFrame;
		time::Time animationSpeed = time::fromSeconds(1.0f);
		time::Time speedCounter = animationSpeed;
	};
}

