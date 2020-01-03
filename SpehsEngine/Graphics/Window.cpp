#include "stdafx.h"
#include "SpehsEngine/Graphics/Window.h"


namespace se
{
	namespace graphics
	{
		Window::Window()
		{
		}
		Window::~Window()
		{
		}

		void Window::add(const View& _view)
		{
			auto it = std::find_if(views.begin(),
								   views.end(),
								   [&_view](const std::unique_ptr<ViewInstance>& view)
								   {
									   return *view.get() == _view;
								   });
			if (it != views.end())
			{
				se::log::error("View already found in window!");
				return;
			}
			views.emplace_back(std::make_unique<ViewInstance>(_view));
		}
		void Window::remove(const View& _view)
		{
			auto it = std::find_if(views.begin(),
								   views.end(),
								   [&_view](const std::unique_ptr<ViewInstance>& view)
								   {
									   return *view.get() == _view;
								   });
			if (it == views.end())
			{
				se::log::error("View not found!");
				return;
			}
			views.erase(it);
		}
	}
}
