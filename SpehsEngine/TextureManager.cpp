
#include "TextureManager.h"
#include "OpenGLError.h"
#include "Console.h"
#include "RNG.h"

#include <functional>
#include <algorithm>

#include <SOIL\SOIL.h>
#include <GL\glew.h>


spehs::TextureManager* textureManager;
namespace spehs
{
	TextureManager::TextureManager() : defaultSet(false)
	{
	}
	TextureManager::~TextureManager()
	{
		clearAllTextureData();
	}

	void TextureManager::setDefaultTexture(const std::string& _filepath)
	{
		toTexture(_filepath);
		defaultTexture = std::hash<std::string>()(_filepath);
		defaultSet = true;
	}


	TextureData* TextureManager::getTextureData(const std::string& _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		return toTexture(_texturePath);
	}
	TextureData* TextureManager::getTextureData(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		if (defaultSet)
			return textureDataMap.find(defaultTexture)->second;
		return nullptr;
	}
	size_t TextureManager::preloadTexture(const std::string& _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		if (textureDataMap.find(hash) == textureDataMap.end())
		{
			toTexture(_texturePath);
		}
		return hash;
	}


	TextureData* TextureManager::getCubeMapData(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz)
	{
		size_t hash = std::hash<std::string>()(_negx + _posx + _posy + _negy + _negz + _posz);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		return toCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
	}
	TextureData* TextureManager::getCubeMapData(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		console::error("CubeMap data not found! hash: " + std::to_string(_hash));
		return nullptr;
	}
	size_t TextureManager::preloadCubeMap(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz)
	{
		size_t hash = std::hash<std::string>()(_negx + _posx + _posy + _negy + _negz + _posz);
		if (textureDataMap.find(hash) == textureDataMap.end())
		{
			toCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
		}
		return hash;
	}


	TextureData* TextureManager::getNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const bool _turbulence)
	{
		size_t hash = std::hash<unsigned>()(_seed);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
			return it->second;

		return textureDataMap.find(preloadNoiseTexture(_width, _height, _seed, _factor, _turbulence))->second;
	}
	TextureData* TextureManager::getNoiseTexture(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
			return it->second;
		console::error("NoiseTexture data not found! hash: " + std::to_string(_hash));
		if (defaultSet)
			return textureDataMap.find(defaultTexture)->second;
		return nullptr;
	}
	size_t TextureManager::preloadNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const bool _turbulence)
	{
		size_t hash = std::hash<unsigned>()(_seed);
		if (textureDataMap.find(hash) != textureDataMap.end())
		{
			console::warning("Preloading already existing noise texture!");
			return hash;
		}

		struct COLOR
		{
			GLubyte r;
			GLubyte g;
			GLubyte b;
			GLubyte a;
		};
		
		RNG noiseRNG(_seed);
		std::vector<COLOR> noiseTextureData;
		noiseTextureData.reserve(_width*_height);
		double** noise = new double*[_width];

		//Generate noise
		for (unsigned x = 0; x < _width; x++)
		{
			noise[x] = new double[_height];
			for (unsigned y = 0; y < _height; y++)
			{
				noise[x][y] = noiseRNG.mtdrandom(0.0, 1.0);
			}
		}

		//Generate texture data from noise
		auto smoothNoise = [&](const double& _x, const double& _y)
		{
			//Get fractional parts
			double fractX = _x - int(_x);
			double fractY = _y - int(_y);

			//Wrap around
			int x1 = (int(_x) + _width) % _width;
			int y1 = (int(_y) + _height) % _height;

			//Neighbor values
			int x2 = (x1 + _width - 1) % _width;
			int y2 = (y1 + _height - 1) % _height;

			//Smooth noise with bilinear interpolation
			double value = 0.0;
			value += fractX * fractY * noise[y1][x1];
			value += (1 - fractX) * fractY * noise[y1][x2];
			value += fractX * (1 - fractY) * noise[y2][x1];
			value += (1 - fractX) * (1 - fractY) * noise[y2][x2];

			//Return value is the weighed average of 4 neighboring values
			return value;
		};
		auto turbulence = [&](const double& _x, const double& _y)
		{
			double value = 0.0, size = _factor;

			while (size >= 1.0)
			{
				value += smoothNoise(_x / size, _y / size) * size;
				size /= 2.0;
			}

			return (128.0 * value / _factor);
		};
		for (unsigned x = 0; x < _width; x++)
		{
			for (unsigned y = 0; y < _height; y++)
			{
				COLOR color;
				if (!_turbulence)
				{
					color.r = color.g = color.b = GLubyte(256 * smoothNoise(x / _factor, y / _factor));
					color.a = (color.r + color.g + color.b) / 3;
					noiseTextureData.push_back(color);
				}
				else
				{
					color.r = color.g = color.b = GLubyte(turbulence(x, y));
					color.a = (color.r + color.g + color.b) / 3;
					noiseTextureData.push_back(color);
				}
			}
		}

		//Create OpenGL Texture
		glEnable(GL_TEXTURE_2D);

		GLuint textureData;

		glGenTextures(1, &textureData);
		glBindTexture(GL_TEXTURE_2D, textureData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, noiseTextureData.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		for (unsigned i = 0; i < _width; i++)
			delete[] noise[i];
		delete[] noise;

		//Create TextureData
		TextureData* newTexData = new TextureData(textureData, _width, _height);
		textureDataMap.insert(std::pair<size_t, TextureData*>(hash, newTexData));

		checkOpenGLErrors(__FILE__, __LINE__);

		return hash;
	}


	void TextureManager::removeTextureData(const std::string& _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		glDeleteTextures(1, &it->second->textureDataID);
		textureDataMap.erase(hash);
	}
	void TextureManager::removeTextureData(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		glDeleteTextures(1, &it->second->textureDataID);
		textureDataMap.erase(_hash);
	}
	void TextureManager::clearAllTextureData()
	{
		for (auto &it : textureDataMap)
		{
			glDeleteTextures(1, &it.second->textureDataID);
		}
		textureDataMap.clear();
	}


	//Private:
	TextureData* TextureManager::toTexture(const std::string& _filepath)
	{
		glEnable(GL_TEXTURE_2D);

		GLuint textureData;
		int width, height;
		unsigned char* image;

		glGenTextures(1, &textureData);
		glBindTexture(GL_TEXTURE_2D, textureData);

		image = SOIL_load_image(_filepath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		//textureData = SOIL_load_OGL_texture(_filepath, SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Create TextureData
		size_t hash = std::hash<std::string>()(_filepath);
		TextureData* newTexData = new TextureData(textureData, width, height);
		textureDataMap.insert(std::pair<size_t, TextureData*>(hash, newTexData));

		checkOpenGLErrors(__FILE__, __LINE__);

		return newTexData;
	}
	TextureData* TextureManager::toCubeMap(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz)
	{
		GLuint cubemapData;

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glGenTextures(1, &cubemapData);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapData);
		glEnable(GL_BLEND);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//texture coordinate generation
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);

		cubemapData = SOIL_load_OGL_cubemap(_negx.c_str(), _posx.c_str(), _posy.c_str(), _negy.c_str(), _negz.c_str(), _posz.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
		
		glBindTexture(GL_TEXTURE_2D, 0);

		//Create TextureData
		size_t hash = std::hash<std::string>()(_negx + _posx + _posy + _negy + _negz + _posz);
		TextureData* newTexData = new TextureData(cubemapData, 0, 0);
		textureDataMap.insert(std::pair<size_t, TextureData*>(hash, newTexData));

		checkOpenGLErrors(__FILE__, __LINE__);

		return newTexData;
	}
}