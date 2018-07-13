#include "stdafx.h"
#include "SpehsEngine/Rendering/Font.h"
#include "SpehsEngine/Core/Log.h"
#include <freetype/ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>

namespace se
{
	namespace rendering
	{
		Font::Font()
		{

		}

		Font::~Font()
		{
			if (ftFace)
			{
				const FT_Error error = FT_Done_Face(*(FT_Face*)ftFace);
				if (error)
				{
					log::error("Freetype error: Failed to unload font " + fontPath + " FT_Error: " + std::to_string(error));
				}
				delete (FT_Face*)ftFace;

				for (unsigned i = 0; i < characters.size(); i++)
				{
					glDeleteTextures(1, &characters[i].textureID);
				}
			}
		}
	}
}