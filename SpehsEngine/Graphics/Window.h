#pragma once

#include "boost/signals2.hpp"
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


			void add(View& _view);
			void remove(View& _view);

			uint16_t getWidth() const;
			uint16_t getHeight() const;

		private:

			friend class WindowInstance;

			std::vector<std::unique_ptr<ViewInstance>> views;

			uint16_t width = 1600;
			uint16_t height = 900;

			boost::signals2::signal<void(void)> destroyedSignal;
		};
	}
}
