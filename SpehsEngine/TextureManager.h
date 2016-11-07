#pragma once

#include <unordered_map>
#include <string>
#include <stdint.h>


typedef unsigned int GLuint;

namespace spehs
{
	enum TextureFiltering
	{
		Nearest = 0x2600,
		Linear = 0x2601,
		NearestMipMapNearest = 0x2700,
		NearestMipMapLinear = 0x2702,
		LinearMipMapNearest = 0x2701,
		LinearMipMapLinear = 0x2703
	};

	struct TextureData
	{
		TextureData(const GLuint& _dataID, const int& _width, const int& _height) : textureDataID(_dataID), width(_width), height(_height){}
		TextureData(const GLuint& _dataID, const int& _width, const int& _height, const unsigned int& _seed) : textureDataID(_dataID), width(_width), height(_height), noiseSeed(_seed){}
		int width, height;
		unsigned int noiseSeed;
		GLuint textureDataID;
	};

	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();

		void setDefaultTexture(const std::string& _filepath, const TextureFiltering downScaleFiltering = spehs::LinearMipMapLinear, const TextureFiltering upScaleFiltering = spehs::Linear);

		TextureData* getTextureData(const std::string& _texturePath, const TextureFiltering downScaleFiltering = spehs::LinearMipMapLinear, const TextureFiltering upScaleFiltering = spehs::Linear); //Get texture data from path
		TextureData* getTextureData(const size_t& _hash); //Get texture data from hash
		size_t preloadTexture(const std::string& _texturePath, const TextureFiltering downScaleFiltering = spehs::LinearMipMapLinear, const TextureFiltering upScaleFiltering = spehs::Linear); //Loads texture and returns hash

		TextureData* getNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const bool _turbulence, const TextureFiltering downScaleFiltering = spehs::LinearMipMapLinear, const TextureFiltering upScaleFiltering = spehs::Linear);
		size_t preloadNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const bool _turbulence, const TextureFiltering downScaleFiltering = spehs::LinearMipMapLinear, const TextureFiltering upScaleFiltering = spehs::Linear);

		TextureData* createTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureFiltering downScaleFiltering = spehs::LinearMipMapLinear, const TextureFiltering upScaleFiltering = spehs::Linear);
		size_t preloadDataTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureFiltering downScaleFiltering = spehs::LinearMipMapLinear, const TextureFiltering upScaleFiltering = spehs::Linear);

		void removeTextureData(const std::string& _texturePath);
		void removeTextureData(const size_t& _hash);
		void clearAllTextureData();

	private:
		TextureData* toTexture(const std::string& _filepath, const TextureFiltering downScaleFiltering, const TextureFiltering upScaleFiltering);

		TextureData* defaultTexture;
		std::unordered_map<size_t, TextureData*> textureDataMap;
	};
}
extern spehs::TextureManager* textureManager;
