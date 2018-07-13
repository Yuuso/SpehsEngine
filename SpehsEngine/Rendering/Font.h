#pragma once
#include <SpehsEngine/Core/Vector.h>
#include <map>

typedef unsigned int GLuint;
typedef char GLchar;

namespace se
{
	namespace rendering
	{
		struct Character
		{
			/* ID handle of the glyph texture */
			GLuint textureID;
			/* Size of glyph */
			se::ivec2 size;
			/* Offset from baseline to left/top of glyph */
			se::ivec2 bearing;
			/* Offset to advance to next glyph */
			GLuint advance;
		};

		class Font
		{
		public:
			Font();
			~Font();

			void* ftFace = nullptr;
			std::string fontPath;
			int fontSize;
			std::map<GLchar, Character> characters;
			int height = 0;
			int ascender = 0;
			int descender = 0;
			int maxAdvanceWidth = 0;
		};
	}
}