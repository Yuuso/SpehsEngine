#pragma once


typedef unsigned int GLuint;

namespace se
{
	namespace rendering
	{
		struct TextureData
		{
			TextureData(const GLuint _dataID, const unsigned int _width, const unsigned int _height) : textureDataID(_dataID), width(_width), height(_height) {}
			TextureData(const GLuint _dataID, const unsigned int _width, const unsigned int _height, const unsigned int _seed) : textureDataID(_dataID), width(_width), height(_height), noiseSeed(_seed) {}
			unsigned int width = 0u, height = 0u;
			unsigned int noiseSeed = 0u;
			GLuint textureDataID = 0u;
		};
	}
}
