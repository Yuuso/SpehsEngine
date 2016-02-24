#include "TextureManager.h"
#include "Exceptions.h"
#include "OpenGLError.h"

#include <functional>

#include <SOIL\SOIL.h>
#include <GL\glew.h>


SpehsEngine::TextureManager* textureManager;
namespace SpehsEngine
{
	TextureManager::TextureManager()
	{
	}
	TextureManager::~TextureManager()
	{
		clearAllTextureData();
	}

	void TextureManager::setDefaultTexture(std::string _filepath)
	{
		toTexture(_filepath);
		defaultTexture = std::hash<std::string>()(_filepath);
	}


	TextureData* TextureManager::getTextureData(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		return toTexture(_texturePath);
	}
	TextureData* TextureManager::getTextureData(size_t _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		return textureDataMap.find(defaultTexture)->second;
	}
	size_t TextureManager::preloadTexture(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		if (textureDataMap.find(hash) == textureDataMap.end())
		{
			toTexture(_texturePath);
		}
		return hash;
	}


	void TextureManager::removeTextureData(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		glDeleteTextures(1, &it->second->textureDataID);
		textureDataMap.erase(hash);
	}
	void TextureManager::removeTextureData(size_t _hash)
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
	TextureData* TextureManager::toTexture(std::string _filepath)
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

		//Create TextureSizeData
		size_t hash = std::hash<std::string>()(_filepath);
		TextureData* newTexData = new TextureData(textureData, width, height);
		textureDataMap.insert(std::pair<size_t, TextureData*>(hash, newTexData));

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif

		return newTexData;
	}
}