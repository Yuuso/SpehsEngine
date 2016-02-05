#include "TextureManager.h"
#include "Exceptions.h"

#include <SOIL\SOIL.h>


SpehsEngine::TextureManager* textureManager;
namespace SpehsEngine
{
	TextureManager::TextureManager()
	{}
	TextureManager::~TextureManager()
	{
		for (unsigned int i = 0; i < textureDataMap.size(); i++)
		{
			glDeleteTextures(1, &(textureDataMap[i]));
		}
		textureDataMap.clear();
	}

	void TextureManager::initializeTextures()
	{
		//HULL TEXTURES: 1 - 9999


		//COMPONENT TEXTURES: 10000 - 19999


		//BACKGROUND TEXTURES: 20000 - 29999


		//PARTICES EFFECT TEXTURES: 30000 - 39999
		textureDataMap[30000] = toTexture("Textures/blue_star.png");

		//MENU/GUI/BUTTON TEXTURES: 40000 - 49999


		//SOMETHING ELSE?
		textureDataMap[50000] = toTexture("Textures/test_texture.png");

	}


	GLuint TextureManager::getTextureData(int _textureIndex)
	{
		return textureDataMap[_textureIndex];
	}


	GLuint TextureManager::toTexture(const char* _filepath)
	{
		glEnable(GL_TEXTURE_2D);

		GLuint textureData;
		int width, height;
		unsigned char* image;

		glGenTextures(1, &textureData);
		glBindTexture(GL_TEXTURE_2D, textureData);

		image = SOIL_load_image(_filepath, &width, &height, 0, SOIL_LOAD_RGBA);
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
			fatalError("Error loading a texture!");
		}

		return textureData;
	}
}