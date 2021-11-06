#pragma once

#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/GUI/GUIElement.h"
#include <memory>
#include <vector>


namespace se
{
	namespace gui
	{
		class GUIView
		{
		public:

			GUIView();
			~GUIView() = default;
			GUIView(const GUIView& _other) = delete;
			GUIView& operator=(const GUIView& _other) = delete;
			GUIView(GUIView&& _other) = delete;
			GUIView& operator=(GUIView&& _other) = delete;


			graphics::View& getView();

			void add(GUIElement& _element);
			void remove(GUIElement& _element);
			void clear();

		private:

			void update(glm::vec2 _renderSize);

			graphics::Camera camera;
			graphics::Scene scene;
			graphics::View view;

			boost::signals2::scoped_connection updateConnection;

			std::vector<GUIElement*> rootElements;
		};
	}
}
