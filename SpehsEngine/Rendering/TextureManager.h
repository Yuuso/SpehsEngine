
#pragma once

#include <unordered_map>
#include <string>
#include <stdint.h>


typedef unsigned int GLuint;

namespace spehs
{
	class Window;

	enum class TextureFiltering : int
	{
		Nearest = 0x2600,
		Linear = 0x2601,
		NearestMipMapNearest = 0x2700,
		LinearMipMapNearest = 0x2701,
		NearestMipMapLinear = 0x2702,
		LinearMipMapLinear = 0x2703
	};

	enum class TextureWrapping : int
	{
		Repeat = 0x2901,
		MirroredRepeat = 0x8370,
		ClampToEdge = 0x812F,
		ClampToBorder = 0x812D,
		MirrorClampToEdge = 0x8743,
	};

	enum class PixelDataType : int
	{
		UnsignedByte = 0x1401,
		Byte = 0x1400,
		UnsignedShort = 0x1403,
		Short = 0x1402,
		UnsignedInt = 0x1405,
		Int = 0x1404,
		Float = 0x1406,
		//...
	};

	class TextureParameter
	{
		//Class containing parameters for textures
		//Can be expanded to include more in the future if needed
	public:
		static const spehs::TextureParameter defaultParameters;

		TextureParameter(){}
		TextureParameter(const TextureFiltering _minFilter, const TextureFiltering _magFilter)
			: minFilter(_minFilter), magFilter(_magFilter){}
		TextureParameter(const TextureFiltering _minFilter, const TextureFiltering _magFilter, const TextureWrapping _xWrapping, const TextureWrapping _yWrapping)
			: minFilter(_minFilter), magFilter(_magFilter), xWrapping(_xWrapping), yWrapping(_yWrapping){}
		TextureParameter(const TextureFiltering _minFilter, const TextureFiltering _magFilter, const TextureWrapping _xWrapping, const TextureWrapping _yWrapping, const PixelDataType _dataType)
			: minFilter(_minFilter), magFilter(_magFilter), xWrapping(_xWrapping), yWrapping(_yWrapping), dataType(_dataType){}
		/*
		mag: Nearest or Linear
		min: Nearest, Linear, NearestMipMapNearest, LinearMipMapNearest, NearestMipMapLinear or LinearMipMapLinear
		*/
		TextureFiltering minFilter = TextureFiltering::NearestMipMapLinear;
		TextureFiltering magFilter = TextureFiltering::Linear;
		TextureWrapping xWrapping = TextureWrapping::Repeat;
		TextureWrapping yWrapping = TextureWrapping::Repeat;
		TextureWrapping rWrapping = TextureWrapping::Repeat;
		PixelDataType dataType = PixelDataType::UnsignedByte;
	};

	struct TextureData
	{
		TextureData(const GLuint _dataID, const unsigned int _width, const unsigned int _height) : textureDataID(_dataID), width(_width), height(_height) {}
		TextureData(const GLuint _dataID, const unsigned int _width, const unsigned int _height, const unsigned int _seed) : textureDataID(_dataID), width(_width), height(_height), noiseSeed(_seed) {}
		unsigned int width, height;
		unsigned int noiseSeed;
		GLuint textureDataID;
	};

	class TextureManager
	{
	public:

		void setDefaultTexture(const std::string& _filepath, const TextureParameter& _parameters);
		void setDefaultTexture(const std::string& _filepath);

		bool isTexture(const std::string& _ID);
		bool isTexture(const size_t _hash);

		TextureData* getTextureData(const std::string& _texturePath, const TextureParameter& _parameters); // Get texture data from path
		TextureData* getTextureData(const std::string& _texturePath);
		TextureData* getTextureData(const size_t _hash); // Get texture data from hash
		size_t preloadTexture(const std::string& _texturePath, const TextureParameter& _parameters); // Loads texture and returns hash
		size_t preloadTexture(const std::string& _texturePath); // Loads texture and returns hash

		TextureData* getCubeMapData(const std::string& _negx, const std::string& _posx, // Get CubeMap data from path
									const std::string& _negy, const std::string& _posy,
									const std::string& _negz, const std::string& _posz, const TextureParameter& _parameters = TextureParameter::defaultParameters);
		TextureData* getCubeMapData(const size_t _hash); // Get CubeMap data from hash
		size_t preloadCubeMap(const std::string& _negx, const std::string& _posx, // Loads CubeMap and returns hash
								const std::string& _negy, const std::string& _posy,
								const std::string& _negz, const std::string& _posz, const TextureParameter& _parameters = TextureParameter::defaultParameters);

		TextureData* getNoiseTexture(const unsigned int _width, const unsigned int _height, const unsigned int _seed, const int _factor, const TextureParameter& _parameters);
		TextureData* getNoiseTexture(const unsigned int _width, const unsigned int _height, const unsigned int _seed, const int _factor);
		size_t preloadNoiseTexture(const unsigned int _width, const unsigned int _height, const unsigned int _seed, const int _factor, const TextureParameter& _parameters);
		size_t preloadNoiseTexture(const unsigned int _width, const unsigned int _height, const unsigned int _seed, const int _factor);

		TextureData* createTexture(const std::string& _ID, const void* _uint8data, const unsigned int _width, const unsigned int _height, const TextureParameter& _parameters);
		TextureData* createTexture(const std::string& _ID, const void* _uint8data, const unsigned int _width, const unsigned int _height);
		size_t preloadDataTexture(const std::string& _ID, const void* _uint8data, const unsigned int _width, const unsigned int _height, const TextureParameter& _parameters);
		size_t preloadDataTexture(const std::string& _ID, const void* _uint8data, const unsigned int _width, const unsigned int _height);

		void removeTextureData(const std::string& _texturePath);
		void removeTextureData(const size_t _hash);
		void clearAllTextureData();

		void takeScreenshot(std::string directory);// Takes a screenshot and writes it to the specified directory
		void takeScreenshot();// Takes screenshot and writes it to the default screenshot directory, specified in the application data

		Window& window;

	private:
		friend class Window;
		TextureManager(Window& window);
		~TextureManager();

		TextureData* toTexture(const std::string& _filepath, const TextureParameter& _parameters);
		TextureData* toCubeMap(const std::string& _negx, const std::string& _posx,
								const std::string& _negy, const std::string& _posy,
								const std::string& _negz, const std::string& _posz, const TextureParameter& _parameters);

		TextureData* defaultTexture;
		std::unordered_map<size_t, TextureData*> textureDataMap;
	};
}