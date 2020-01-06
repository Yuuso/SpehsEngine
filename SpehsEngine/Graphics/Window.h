#pragma once

#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Internal/ViewInstance.h"
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

			uint16_t getWidth() const;
			uint16_t getHeight() const;

		protected:

			friend class WindowInstance;

			std::vector<std::unique_ptr<ViewInstance>> views;

		private:

			uint16_t width = 800;
			uint16_t height = 600;
		};
	}
}
