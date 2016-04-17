#include "TextureManager.h"
#include "OpenGLError.h"
#include "Console.h"

#include <functional>

#include <SOIL\SOIL.h>
#include <GL\glew.h>


spehs::TextureManager* textureManager;
namespace spehs
{
	TextureManager::TextureManager()
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
		return textureDataMap.find(defaultTexture)->second;
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
		console::error("CubeMap data not found!");
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