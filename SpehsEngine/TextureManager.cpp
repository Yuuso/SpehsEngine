#include "TextureManager.h"
#include "Exceptions.h"

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
		defaultTexture = toTexture(_filepath);
	}


	GLuint TextureManager::getTextureData(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		GLuint data = toTexture(_texturePath);
		textureDataMap.insert(std::pair<size_t, GLuint>(hash, data));
		return data;
	}
	GLuint TextureManager::getTextureData(size_t _hash)
	{
		auto it = textureDataMap.find(_hash);
		if (it != textureDataMap.end())
		{
			return it->second;
		}
		return defaultTexture;
	}
	size_t TextureManager::preloadTexture(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		if (textureDataMap.find(hash) == textureDataMap.end())
		{
			textureDataMap.insert(std::pair<size_t, GLuint>(hash, toTexture(_texturePath)));
		}
		return hash;
	}


	TextureSizeData* TextureManager::getSizeData(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureSizeDataMap.find(hash);
		if (it != textureSizeDataMap.end())
		{
			return it->second;			
		}
		unexpectedError("Texture doesn't exists!");
		return nullptr;
	}


	void TextureManager::removeTextureData(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		auto it = textureDataMap.find(hash);
		glDeleteTextures(1, &it->second);
		auto it2 = textureSizeDataMap.find(hash);
		delete it2->second;
		textureDataMap.erase(hash);
		textureSizeDataMap.erase(hash);
	}
	void TextureManager::removeTextureData(size_t _hash)
	{
		auto it = textureDataMap.find(_hash);
		glDeleteTextures(1, &it->second);
		auto it2 = textureSizeDataMap.find(_hash);
		delete it2->second;
		textureDataMap.erase(_hash);
		textureSizeDataMap.erase(_hash);
	}
	void TextureManager::clearAllTextureData()
	{
		for (auto &it : textureDataMap)
		{
			glDeleteTextures(1, &it.second);
		}
		for (auto &it : textureSizeDataMap)
		{
			delete it.second;
		}
		textureDataMap.clear();
		textureSizeDataMap.clear();
	}


	//Private:
	GLuint TextureManager::toTexture(std::string _filepath)
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
		textureSizeDataMap.insert(std::pair<size_t, TextureSizeData*>(hash, new TextureSizeData(width, height)));

		//GLenum error = glGetError();
		//if (error != GL_NO_ERROR)
		//{
		//	fatalError("Error loading a texture: " + _filepath);
		//}

		return textureData;
	}
}