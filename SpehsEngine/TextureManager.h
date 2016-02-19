#pragma once

#include <unordered_map>
#include <string>


typedef unsigned int GLuint;

namespace SpehsEngine
{
	struct TextureSizeData
	{
		TextureSizeData(int _width, int _height) : width(_width), height(_height){}
		int width, height;
	};

	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();

		void setDefaultTexture(std::string _filepath);

		GLuint getTextureData(std::string _texturePath); //Get texture data from path
		GLuint getTextureData(size_t _hash); //Get texture data from hash
		size_t preloadTexture(std::string _texturePath); //Loads texture and return hash

		TextureSizeData* getSizeData(std::string _texturePath);

		void removeTextureData(std::string _texturePath);
		void removeTextureData(size_t _hash);
		void clearAllTextureData();

	private:
		GLuint toTexture(std::string _filepath);

		GLuint defaultTexture;
		std::unordered_map<size_t, GLuint> textureDataMap;
		std::unordered_map<size_t, TextureSizeData*> textureSizeDataMap;
	};
}
extern SpehsEngine::TextureManager* textureManager;
