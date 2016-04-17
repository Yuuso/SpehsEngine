#pragma once

#include <unordered_map>
#include <string>


typedef unsigned int GLuint;

namespace spehs
{
	struct TextureData
	{
		TextureData(const GLuint& _dataID, const int& _width, const int& _height) : textureDataID(_dataID), width(_width), height(_height){}
		int width, height;
		GLuint textureDataID;
	};

	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();

		void setDefaultTexture(const std::string& _filepath);

		TextureData* getTextureData(const std::string& _texturePath); //Get texture data from path
		TextureData* getTextureData(const size_t& _hash); //Get texture data from hash
		size_t preloadTexture(const std::string& _texturePath); //Loads texture and returns hash

		TextureData* getCubeMapData(const std::string& _negx, const std::string& _posx,
									const std::string& _negy, const std::string& _posy,
									const std::string& _negz, const std::string& _posz); //Get CubeMap data from path
		TextureData* getCubeMapData(const size_t& _hash); //Get CubeMap data from hash
		size_t preloadCubeMap(const std::string& _negx, const std::string& _posx,
								const std::string& _negy, const std::string& _posy,
								const std::string& _negz, const std::string& _posz); //Loads CubeMap and returns hash

		void removeTextureData(const std::string& _texturePath);
		void removeTextureData(const size_t& _hash);
		void clearAllTextureData();

	private:
		TextureData* toTexture(const std::string& _filepath);
		TextureData* toCubeMap(const std::string& _negx, const std::string& _posx, 
								const std::string& _negy, const std::string& _posy, 
								const std::string& _negz, const std::string& _posz);

		size_t defaultTexture;
		std::unordered_map<size_t, TextureData*> textureDataMap;
	};
}
extern spehs::TextureManager* textureManager;
