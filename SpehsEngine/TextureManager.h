#pragma once

#include <unordered_map>
#include <string>
#include <stdint.h>


typedef unsigned int GLuint;

namespace spehs
{
	/*
	mag: Nearest or Linear
	min: Nearest, Linear, NearestMipMapNearest, LinearMipMapNearest, NearestMipMapLinear or LinearMipMapLinear
	*/
	enum class TextureFiltering : int
	{
		Nearest = 0x2600,
		Linear = 0x2601,
		NearestMipMapNearest = 0x2700,
		LinearMipMapNearest = 0x2701,
		NearestMipMapLinear = 0x2702,
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

		void setDefaultTexture(const std::string& _filepath, const TextureFiltering minScaleFiltering = spehs::TextureFiltering::LinearMipMapLinear, const TextureFiltering magScaleFiltering = spehs::TextureFiltering::Linear);

		TextureData* getTextureData(const std::string& _texturePath, const TextureFiltering minScaleFiltering = spehs::TextureFiltering::LinearMipMapLinear, const TextureFiltering magScaleFiltering = spehs::TextureFiltering::Linear); //Get texture data from path
		TextureData* getTextureData(const size_t& _hash); //Get texture data from hash
		size_t preloadTexture(const std::string& _texturePath, const TextureFiltering minScaleFiltering = spehs::TextureFiltering::LinearMipMapLinear, const TextureFiltering magScaleFiltering = spehs::TextureFiltering::Linear); //Loads texture and returns hash

		TextureData* getNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const TextureFiltering minScaleFiltering = spehs::TextureFiltering::LinearMipMapLinear, const TextureFiltering magScaleFiltering = spehs::TextureFiltering::Linear);
		size_t preloadNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const TextureFiltering minScaleFiltering = spehs::TextureFiltering::LinearMipMapLinear, const TextureFiltering magScaleFiltering = spehs::TextureFiltering::Linear);

		TextureData* createTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureFiltering minScaleFiltering = spehs::TextureFiltering::LinearMipMapLinear, const TextureFiltering magScaleFiltering = spehs::TextureFiltering::Linear);
		size_t preloadDataTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureFiltering minScaleFiltering = spehs::TextureFiltering::LinearMipMapLinear, const TextureFiltering magScaleFiltering = spehs::TextureFiltering::Linear);

		void removeTextureData(const std::string& _texturePath);
		void removeTextureData(const size_t& _hash);
		void clearAllTextureData();

	private:
		TextureData* toTexture(const std::string& _filepath, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering);

		TextureData* defaultTexture;
		std::unordered_map<size_t, TextureData*> textureDataMap;
	};
}
extern spehs::TextureManager* textureManager;
