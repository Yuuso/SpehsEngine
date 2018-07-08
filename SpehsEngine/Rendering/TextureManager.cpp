#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Rendering/GLContext.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Rendering/TextureManager.h"

#include <functional>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <time.h>
#include <sstream>
#include <string>

#include <SOIL\SOIL.h>
#include <GL\glew.h>


#define DEFAULT_TEXTURE_SEED 1965819

const spehs::TextureParameter spehs::TextureParameter::defaultParameters = spehs::TextureParameter();


namespace spehs
{
	TextureManager::TextureManager(Window& _window)
		: window(_window)
	{
		SPEHS_ASSERT(window.isValid());
		defaultTexture = getNoiseTexture(4, 4, DEFAULT_TEXTURE_SEED, 4);
	}

	TextureManager::~TextureManager()
	{
		clearAllTextureData();
	}

	void TextureManager::setDefaultTexture(const std::string& _filepath, const TextureParameter& _parameters)
	{
		TextureData* temp = toTexture(_filepath, _parameters);
		if (temp)
			defaultTexture = temp;
		else
			exceptions::fatalError("Creating default texture failed!");
	}

	void TextureManager::setDefaultTexture(const std::string& _filepath)
	{
		setDefaultTexture(_filepath, TextureParameter::defaultParameters);
	}

	bool TextureManager::isTexture(const std::string& _ID)
	{
		size_t hash = std::hash<std::string>()(_ID);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return true;
		}
		return false;
	}
	bool TextureManager::isTexture(const size_t _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return true;
		}
		return false;
	}

	TextureData* TextureManager::getTextureData(const std::string& _texturePath, const TextureParameter& _parameters)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		TextureData* temp = toTexture(_texturePath, _parameters);
		if (temp)
			return temp;
		else
			return defaultTexture;
	}

	TextureData* TextureManager::getTextureData(const std::string& _texturePath)
	{
		return getTextureData(_texturePath, TextureParameter::defaultParameters);
	}

	TextureData* TextureManager::getTextureData(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		exceptions::warning("Texture not found, using default.");
		return defaultTexture;
	}

	size_t TextureManager::preloadTexture(const std::string& _texturePath, const TextureParameter& _parameters)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		if (textureDataMap.find(hash) == textureDataMap.end())
		{
			TextureData* temp = toTexture(_texturePath, _parameters);
			if (!temp)
				return std::hash<unsigned int>()(DEFAULT_TEXTURE_SEED);
		}
		else
		{
			exceptions::warning("Trying to preload already existing texture!");
		}
		return hash;
	}

	size_t TextureManager::preloadTexture(const std::string& _texturePath)
	{
		return preloadTexture(_texturePath, TextureParameter::defaultParameters);
	}

	TextureData* TextureManager::getCubeMapData(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz, const TextureParameter& _parameters)
	{
		size_t hash = std::hash<std::string>()(_negx + _posx + _posy + _negy + _negz + _posz);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		return toCubeMap(_negx, _posx, _negy, _posy, _negz, _posz, _parameters);
	}
	TextureData* TextureManager::getCubeMapData(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		exceptions::warning("CubeMap data not found! hash: " + std::to_string(_hash));
		//TODO: Default Cube Map
		return nullptr;
	}
	size_t TextureManager::preloadCubeMap(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz, const TextureParameter& _parameters)
	{
		size_t hash = std::hash<std::string>()(_negx + _posx + _posy + _negy + _negz + _posz);
		if (textureDataMap.find(hash) == textureDataMap.end())
		{
			toCubeMap(_negx, _posx, _negy, _posy, _negz, _posz, _parameters);
		}
		else
		{
			exceptions::warning("Trying to preload already existing cubemap!");
		}
		return hash;
	}

	TextureData* TextureManager::getNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const TextureParameter& _parameters)
	{
		size_t hash = std::hash<unsigned>()(_seed);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
			return it->second;

		return textureDataMap.find(preloadNoiseTexture(_width, _height, _seed, _factor, _parameters))->second;
	}

	TextureData* TextureManager::getNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor)
	{
		return getNoiseTexture(_width, _height, _seed, _factor, TextureParameter::defaultParameters);
	}

	size_t TextureManager::preloadNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const TextureParameter& _parameters)
	{
		if (_factor == 0)
		{
			exceptions::unexpectedError("Factor can't be 0! Using default texture. (TextureManager->preloadNoiseTexture)");
			return std::hash<unsigned int>()(DEFAULT_TEXTURE_SEED);
		}

		size_t hash = std::hash<unsigned>()(_seed);
		if (textureDataMap.find(hash) != textureDataMap.end())
		{
			exceptions::warning("Tryng to preload already existing noisetexture!");
			return hash;
		}

		struct COLOR
		{
			GLubyte r;
			GLubyte g;
			GLubyte b;
			GLubyte a;
		};

		rng::PRNG<uint32_t> noiseRNG(_seed);
		std::vector<COLOR> noiseTextureData;
		noiseTextureData.reserve(_width*_height);
		double** noise = new double*[_width];

		//Generate noise
		for (unsigned x = 0; x < _width; x++)
		{
			noise[x] = new double[_height];
			for (unsigned y = 0; y < _height; y++)
			{
				//noise[x][y] = noiseRNG.unit<double>();
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
				color.r = color.g = color.b = GLubyte(turbulence(x, y));
				color.a = (color.r + color.g + color.b) / 3;
				noiseTextureData.push_back(color);
			}
		}

		//Create OpenGL Texture
		glEnable(GL_TEXTURE_2D);

		GLuint textureData;

		glGenTextures(1, &textureData);
		glBindTexture(GL_TEXTURE_2D, textureData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) _parameters.xWrapping);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) _parameters.yWrapping);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, (GLint) _parameters.dataType, noiseTextureData.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) _parameters.minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) _parameters.magFilter);
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

	size_t TextureManager::preloadNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor)
	{
		return preloadNoiseTexture(_width, _height, _seed, _factor, TextureParameter::defaultParameters);
	}

	TextureData* TextureManager::createTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureParameter& _parameters)
	{
		size_t hash = std::hash<std::string>()(_ID);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
			return it->second;

		return textureDataMap.find(preloadDataTexture(_ID, _uint8data, _width, _height, _parameters))->second;
	}

	TextureData* TextureManager::createTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height)
	{
		return createTexture(_ID, _uint8data, _width, _height, TextureParameter::defaultParameters);
	}

	size_t TextureManager::preloadDataTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureParameter& _parameters)
	{
		size_t hash = std::hash<std::string>()(_ID);
		if (textureDataMap.find(hash) != textureDataMap.end())
		{
			exceptions::warning("Tryng to preload already existing datatexture!");
			return hash;
		}

		//Create OpenGL Texture
		glEnable(GL_TEXTURE_2D);

		GLuint textureData;

		glGenTextures(1, &textureData);
		glBindTexture(GL_TEXTURE_2D, textureData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) _parameters.xWrapping);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) _parameters.yWrapping);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, (GLint) _parameters.dataType, _uint8data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) _parameters.minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) _parameters.magFilter);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Create TextureData
		TextureData* newTexData = new TextureData(textureData, _width, _height);
		textureDataMap.insert(std::pair<size_t, TextureData*>(hash, newTexData));

		return hash;
	}

	size_t TextureManager::preloadDataTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height)
	{
		return preloadDataTexture(_ID, _uint8data, _width, _height, TextureParameter::defaultParameters);
	}

	void TextureManager::removeTextureData(const std::string& _texturePath)
	{
		removeTextureData(std::hash<std::string>()(_texturePath));
	}

	void TextureManager::removeTextureData(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			glDeleteTextures(1, &it->second->textureDataID);
			textureDataMap.erase(_hash);
		}
		else
		{
			exceptions::warning("Texture not found, cannot remove!");
		}
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
	TextureData* TextureManager::toTexture(const std::string& _filepath, const TextureParameter& _parameters)
	{
		glEnable(GL_TEXTURE_2D);

		GLuint textureData;
		int width, height;
		unsigned char* image;

		glGenTextures(1, &textureData);
		glBindTexture(GL_TEXTURE_2D, textureData);

		image = SOIL_load_image(_filepath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		//textureData = SOIL_load_OGL_texture(_filepath, SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

		if (!image)
		{
			exceptions::warning("Failed to load image file: " + _filepath);
			return nullptr;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,		(GLint) _parameters.xWrapping);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,		(GLint) _parameters.yWrapping);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, (GLint) _parameters.dataType, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	(GLint) _parameters.minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,	(GLint) _parameters.magFilter);
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
	TextureData* TextureManager::toCubeMap(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz, const TextureParameter& _parameters)
	{
		GLuint cubemapData;

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glGenTextures(1, &cubemapData);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapData);
		glEnable(GL_BLEND);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (GLint) _parameters.magFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (GLint) _parameters.minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, (GLint) _parameters.xWrapping);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, (GLint) _parameters.yWrapping);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (GLint) _parameters.rWrapping);

		//texture coordinate generation
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);

		// right, left, top, bottom, back, front
		cubemapData = SOIL_load_OGL_cubemap(_negx.c_str(), _posx.c_str(), _posy.c_str(), _negy.c_str(), _negz.c_str(), _posz.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

		glBindTexture(GL_TEXTURE_2D, 0);

		// Create TextureData
		size_t hash = std::hash<std::string>()(_negx + _posx + _posy + _negy + _negz + _posz);
		TextureData* newTexData = new TextureData(cubemapData, 0, 0);
		textureDataMap.insert(std::pair<size_t, TextureData*>(hash, newTexData));

		checkOpenGLErrors(__FILE__, __LINE__);

		return newTexData;
	}

	void TextureManager::takeScreenshot()
	{
		takeScreenshot("screenshots/");
	}

	void TextureManager::takeScreenshot(std::string directory)
	{
		if (directory.size() > 0 && directory.back() != '/' && directory.back() != '\\')
			directory.push_back('/');

		std::time_t t = std::time(nullptr);
		std::tm tm;
		localtime_s(&tm, &t);

		//Determining file name/path
		char string[64];
		std::strftime(string, 64, "%Y-%m-%d_%H-%M-%S", &tm);
		std::string path = directory + "screenshot_" + string + ".png";

		int screenshot;
		screenshot = SOIL_save_screenshot(path.c_str(), SOIL_SAVE_TYPE_BMP, 0, 0, window.getWidth(), window.getHeight());
		//console::log("Screenshot saved to \"" + path + "\"", spehs::Color(90, 255, 230));
	}
}