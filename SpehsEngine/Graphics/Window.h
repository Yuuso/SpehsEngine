#pragma once

#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/ViewInstance.h"
#include "SpehsEngine/Graphics/Camera.h"
#include <vector>
#include <memory>



namespace se
{
	namespace graphics
	{
		class Window
		{
		public:

			Window();
			~Window();

			Window(const Window& _other) = delete;
			Window& operator=(const Window& _other) = delete;

			Window(Window&& _other) = delete;
			Window& operator=(Window&& _other) = delete;


			void add(const View& _view);
			void remove(const View& _view);

			uint16_t widthGet() const  { return width;  };
			uint16_t heightGet() const { return height; };

		protected:

			friend class WindowInstance;

			std::vector<std::unique_ptr<ViewInstance>> views;

		private:

			uint16_t width = 600;
			uint16_t height = 600;
		};
	}
}
