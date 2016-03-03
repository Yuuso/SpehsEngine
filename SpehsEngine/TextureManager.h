#pragma once

#include <unordered_map>
#include <string>


typedef unsigned int GLuint;

namespace spehs
{
	struct TextureData
	{
		TextureData(GLuint _dataID, int _width, int _height) : textureDataID(_dataID), width(_width), height(_height){}
		int width, height;
		GLuint textureDataID;
	};

	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();

		void setDefaultTexture(std::string _filepath);

		TextureData* getTextureData(std::string _texturePath); //Get texture data from path
		TextureData* getTextureData(size_t _hash); //Get texture data from hash
		size_t preloadTexture(std::string _texturePath); //Loads texture and return hash

		void removeTextureData(std::string _texturePath);
		void removeTextureData(size_t _hash);
		void clearAllTextureData();

	private:
		TextureData* toTexture(std::string _filepath);

		size_t defaultTexture;
		std::unordered_map<size_t, TextureData*> textureDataMap;
	};
}
extern spehs::TextureManager* textureManager;
