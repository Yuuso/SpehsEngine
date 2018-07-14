#include "stdafx.h"
#include "SpehsEngine/Rendering/FontManager.h"
#include "SpehsEngine/Rendering/Font.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Core/Log.h"

#include <GL/glew.h>
#include <freetype/ft2build.h>
#include FT_FREETYPE_H


namespace se
{
	namespace rendering
	{
		FontManager::FontManager(Window& _window)
			: window(_window)
			, ftLibrary(nullptr)
		{
			ftLibrary = new FT_Library;

			const FT_Error error = FT_Init_FreeType((FT_Library*)ftLibrary);
			if (error)
			{
				log::error("Freetype library initialization failed!");
				FT_Done_FreeType(*(FT_Library*)ftLibrary);//Should this really be called here if the init failed?
				delete ftLibrary;
				ftLibrary = nullptr;
			}
		}

		FontManager::~FontManager()
		{
			//Uninitialize FreeType
			if (ftLibrary)
			{
				FT_Done_FreeType(*(FT_Library*)ftLibrary);
				delete (FT_Face*)ftLibrary;
				ftLibrary = nullptr;
			}

			//Inform (possible) memory leaks
			if (fonts.size() != 0)
			{
				log::info("Some Font objects were not deallocated! Remaining font count: " + std::to_string(fonts.size()));
				for (unsigned i = 0; i < fonts.size(); i++)
				{
					delete fonts[i];
				}
			}
		}

		Font* FontManager::getFont(const std::string &_fontPath, const int &_size)
		{
			if (!ftLibrary)
			{
				log::warning("FontManager wasn't successfully initialized, cannot load fonts!");
				return nullptr;
			}

			//Check for an already existing font
			for (unsigned int i = 0; i < fonts.size(); i++)
			{
				if (_fontPath == fonts[i]->fontPath)
				{
					if (_size == fonts[i]->fontSize)
					{
						return fonts[i];
					}
				}
			}

			//Create a new font
			Font* font = new Font();
			font->ftFace = new FT_Face;

			const FT_Error error = FT_New_Face(*(FT_Library*)ftLibrary, _fontPath.c_str(), 0, (FT_Face*)font->ftFace);
			if (error)
			{
				log::warning("Freetype error: Failed to load font '" + _fontPath + "', FT_Error: " + std::to_string(error));
				//Manually deallocate the ft face so that the font destructor won't call FT_Done_Face() with it.
				delete (FT_Face*)font->ftFace;
				font->ftFace = nullptr;
				delete font;
				return nullptr;
			}

			fonts.push_back(font);
			font->fontPath = _fontPath;
			font->fontSize = _size;

			FT_Face* ftFace = (FT_Face*)font->ftFace;
			FT_Set_Pixel_Sizes(*ftFace, 0, _size);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction

			for (GLubyte c = 32; c <= 126; c++)
			{
				//Load character glyph
				if (FT_Load_Char(*(FT_Face*)ftFace, c, FT_LOAD_RENDER))
				{
					log::warning("FreeType error: Failed to load Glyph");
				}
				else
				{
					//Generate texture
					GLuint texture;
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);
					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RED,
						(*(FT_Face*)ftFace)->glyph->bitmap.width,
						(*(FT_Face*)ftFace)->glyph->bitmap.rows,
						0,
						GL_RED,
						GL_UNSIGNED_BYTE,
						(*(FT_Face*)ftFace)->glyph->bitmap.buffer
					);

					//Set texture options
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					//Now store character for later use
					Character character =
					{
						texture,
						glm::ivec2((*(FT_Face*)ftFace)->glyph->bitmap.width, (*(FT_Face*)ftFace)->glyph->bitmap.rows),
						glm::ivec2((*(FT_Face*)ftFace)->glyph->bitmap_left, (*(FT_Face*)ftFace)->glyph->bitmap_top),
						(GLuint)(*(FT_Face*)ftFace)->glyph->advance.x
					};
					font->characters.insert(std::pair<GLchar, Character>(c, character));
				}
			}

			checkOpenGLErrors(__FILE__, __LINE__);

			font->height = (*(FT_Face*)font->ftFace)->size->metrics.height >> 6;
			font->ascender = (*(FT_Face*)font->ftFace)->ascender >> 6;
			font->descender = (*(FT_Face*)font->ftFace)->descender >> 6;
			font->maxAdvanceWidth = (*(FT_Face*)font->ftFace)->max_advance_width >> 6;

			return font;
		}
	}
}