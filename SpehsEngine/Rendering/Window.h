#pragma once
#include <boost/signals2.hpp>
#include "SpehsEngine/Core/Color.h"

struct SDL_Window;
namespace se
{
	namespace rendering
	{
		class TextureManager;
		class FontManager;
		class GLContext;

		class Window
		{
		public:
			friend class GLContext;
		public:

			Window(const int pixelWidth, const int pixelHeight);
			~Window();

			void renderBegin();
			void renderEnd();

			void setClearColor(const Color& color);
			void setTitle(const char* title);
			void setSize(const int pixelWidth, const int pixelHeight);
			void setMinSize(const int minPixelWidth, const int minPixelHeight);
			void setMaxSize(const int maxPixelWidth, const int maxPixelHeight);
			void setFullscreen(const bool enabled);
			void setBorderless(const bool enabled);
			void setInputGrab(const bool enabled);
			void setMousePosition(const glm::ivec2& position);

			bool isValid() const;
			TextureManager* getTextureManager();
			const TextureManager* getTextureManager() const;
			FontManager* getFontManager();
			const FontManager* getFontManager() const;
			bool getInputGrab() const;
			SDL_Window* getSDLWindow() const { return sdlWindow; }
			GLContext* getGLContext() const { return glContext; }

			/* Returns window pixel width. */
			int getWidth() const;
			/* Returns window pixel height. */
			int getHeight() const;

			/* Fires when the size of the window is changed. */
			boost::signals2::signal<void(const int, const int)> sizeChangedSignal;

		private:
			SDL_Window* sdlWindow;
			GLContext* glContext;
			/* Each window must have its own texture manager. (OpenGL limitation) */
			TextureManager* textureManager;
			/* Each window must have its own font manager. (OpenGL limitation) */
			FontManager* fontManager;
			int width;
			int height;
		};
	}
}