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
		if (textureDataMap.find(hash) != textureDataMap.end())
		{
			return textureDataMap[hash];
		}
		GLuint data = toTexture(_texturePath);
		textureDataMap.insert(std::pair<size_t, GLuint>(hash, data));
		return data;
	}
	GLuint TextureManager::getTextureData(size_t _hash)
	{
		if (textureDataMap.find(_hash) != textureDataMap.end())
		{
			return textureDataMap[_hash];
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


	void TextureManager::removeTextureData(std::string _texturePath)
	{
		size_t hash = std::hash<std::string>()(_texturePath);
		glDeleteTextures(1, &(textureDataMap[hash]));
		textureDataMap.erase(hash);
	}
	void TextureManager::removeTextureData(size_t _hash)
	{
		glDeleteTextures(1, &(textureDataMap[_hash]));
		textureDataMap.erase(_hash);
	}
	void TextureManager::clearAllTextureData()
	{
		for (unsigned int i = 0; i < textureDataMap.size(); i++)
		{
			glDeleteTextures(1, &(textureDataMap[i]));
		}
		textureDataMap.clear();
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

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			fatalError("Error loading a texture: " + _filepath);
		}

		return textureData;
	}
}