#pragma once

#include <unordered_map>
#include <GL\glew.h>


namespace SpehsEngine
{
	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();

		void initializeTextures();

		GLuint getTextureData(int _textureIndex);

	private:
		GLuint toTexture(const char* _filepath);
		std::unordered_map<unsigned int, GLuint> textureDataMap;
	};
}
extern SpehsEngine::TextureManager* textureManager;
