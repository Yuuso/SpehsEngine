

#include "TextureManager.h"
#include "OpenGLError.h"
#include "Exceptions.h"
#include "Console.h"
#include "RNG.h"
#include "ApplicationData.h"

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


spehs::TextureManager* textureManager;
namespace spehs
{
	TextureManager::TextureManager()
	{
		defaultTexture = getNoiseTexture(16, 16, DEFAULT_TEXTURE_SEED, 4);
	}
	TextureManager::~TextureManager()
	{
		clearAllTextureData();
	}

	void TextureManager::setDefaultTexture(const std::string& _filepath, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
	{
		TextureData* temp = toTexture(_filepath, minScaleFiltering, magScaleFiltering);
		if (temp)
			defaultTexture = temp;
		else
			exceptions::fatalError("Creating default texture failed!");
	}


	TextureData* TextureManager::getTextureData(const std::string& _texturePath, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		TextureData* temp = toTexture(_texturePath, minScaleFiltering, magScaleFiltering);
		if (temp)
			return temp;
		else
			return defaultTexture;
	}
	TextureData* TextureManager::getTextureData(const size_t& _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		console::warning("Texture not found, using default.");
		return defaultTexture;
	}
	size_t TextureManager::preloadTexture(const std::string& _texturePath, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		if (textureDataMap.find(hash) == textureDataMap.end())
		{
			TextureData* temp = toTexture(_texturePath, minScaleFiltering, magScaleFiltering);
			if (!temp)
				return std::hash<unsigned int>()(DEFAULT_TEXTURE_SEED);
		}
		else
		{
			console::warning("Trying to preload already existing texture!");
		}
		return hash;
	}


	TextureData* TextureManager::getNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
	{
		size_t hash = std::hash<unsigned>()(_seed);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
			return it->second;

		return textureDataMap.find(preloadNoiseTexture(_width, _height, _seed, _factor, minScaleFiltering, magScaleFiltering))->second;
	}
	size_t TextureManager::preloadNoiseTexture(const int& _width, const int& _height, const unsigned int& _seed, const int& _factor, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
	{
		if (_factor == 0)
		{
			console::error("Factor can't be 0! Using default texture. (TextureManager->preloadNoiseTexture)");
			return std::hash<unsigned int>()(DEFAULT_TEXTURE_SEED);
		}

		size_t hash = std::hash<unsigned>()(_seed);
		if (textureDataMap.find(hash) != textureDataMap.end())
		{
			console::warning("Tryng to preload already existing noisetexture!");
			return hash;
		}

		struct COLOR
		{
			GLubyte r;
			GLubyte g;
			GLubyte b;
			GLubyte a;
		};
		
		rng::PRNG32 noiseRNG(_seed);
		std::vector<COLOR> noiseTextureData;
		noiseTextureData.reserve(_width*_height);
		double** noise = new double*[_width];

		//Generate noise
		for (unsigned x = 0; x < _width; x++)
		{
			noise[x] = new double[_height];
			for (unsigned y = 0; y < _height; y++)
			{
				noise[x][y] = noiseRNG.drandom(0.0, 1.0);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, noiseTextureData.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) minScaleFiltering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) magScaleFiltering);
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
	TextureData* TextureManager::createTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
	{
		size_t hash = std::hash<std::string>()(_ID);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
			return it->second;

		return textureDataMap.find(preloadDataTexture(_ID, _uint8data, _width, _height, minScaleFiltering, magScaleFiltering))->second;
	}
	size_t TextureManager::preloadDataTexture(const std::string &_ID, const void* _uint8data, const int _width, const int _height, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
	{
		size_t hash = std::hash<std::string>()(_ID);
		if (textureDataMap.find(hash) != textureDataMap.end())
		{
			console::warning("Tryng to preload already existing datatexture!");
			return hash;
		}

		//Create OpenGL Texture
		glEnable(GL_TEXTURE_2D);

		GLuint textureData;

		glGenTextures(1, &textureData);
		glBindTexture(GL_TEXTURE_2D, textureData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _uint8data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) minScaleFiltering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) magScaleFiltering);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Create TextureData
		TextureData* newTexData = new TextureData(textureData, _width, _height);
		textureDataMap.insert(std::pair<size_t, TextureData*>(hash, newTexData));

		return hash;
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
			console::warning("Texture not found, cannot remove!");
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
	TextureData* TextureManager::toTexture(const std::string& _filepath, const TextureFiltering minScaleFiltering, const TextureFiltering magScaleFiltering)
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
			console::warning("Failed to load image file: " + _filepath);
			return nullptr;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) minScaleFiltering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) magScaleFiltering);
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

	void TextureManager::takeScreenShot()
	{
		takeScreenShot(applicationData->screenshotDirectory);
	}

	void TextureManager::takeScreenShot(std::string directory)
	{
		if (directory.size() > 0 && directory.back() != '/' && directory.back() != '\\')
			directory.push_back('/');

		std::time_t t = std::time(nullptr);
		std::tm tm;
		localtime_s(&tm, &t);

		//Determining file name/path
		char string[64];
		std::strftime(string, 64, "%H-%M-%S_%d-%m-%Y", &tm);
		std::string path = directory + "screenshot_" + string + ".png";

		int screenshot;
		screenshot = SOIL_save_screenshot(path.c_str(), SOIL_SAVE_TYPE_BMP, 0, 0, applicationData->getWindowWidth(), applicationData->getWindowHeight());
		console::log("Screenshot saved to \"" + path + "\"", glm::vec3(0.35f, 1.0f, 0.9f));
	}
}