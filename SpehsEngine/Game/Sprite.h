
#pragma once

#include "SpehsEngine/Game/Component.h"
#include "SpehsEngine/Rendering/Primitive.h"


namespace se
{
	namespace rendering
	{
		class BatchManager;
		class TextureManager;
		struct TextureData;
		class Polygon;
	}
	class GameObject;

	class Sprite : public Component
	{
	public:
		Sprite(rendering::BatchManager& batchManager);
		Sprite(rendering::BatchManager& batchManager, GameObject& _owner);
		~Sprite() override;

		void update(const time::Time deltaTime) override;

		virtual void setTexture(rendering::TextureData* _textureDataPtr);
		virtual void setTextureID(const unsigned int _textureID);
		void setSize(const float _x, const float _y);
		void setColor(const Color &_newColor);
		void setAlpha(const float _a);
		void setDepth(const PlaneDepth _depth);
		void setRenderState(const bool _state);
		void updateVertices();
		const rendering::Polygon* getSpritePolygon(){ return sprite; }

	protected:
		rendering::TextureManager &textureManager;
		rendering::Polygon* sprite;
	};
}

