#pragma once

#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/GUI/GUIElement.h"
#include "SpehsEngine/GUI/Internal/GUIMaterialManager.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include <memory>
#include <vector>


namespace se
{
	namespace gui
	{
		class GUIView
		{
		public:

			GUIView(graphics::ShaderManager& _shaderManager, graphics::TextureManager& _textureManager,
					graphics::FontManager& _fontManager, input::EventSignaler& _eventSignaler, int _inputPriority);
			~GUIView() = default;
			GUIView(const GUIView& _other) = delete;
			GUIView& operator=(const GUIView& _other) = delete;
			GUIView(GUIView&& _other) = delete;
			GUIView& operator=(GUIView&& _other) = delete;


			graphics::View& getView();
			void setInputHandlingEnabled(bool _value);
			bool getInputhandlingEnabled() const;

			void add(GUIElement& _element);
			void remove(GUIElement& _element);
			void clear();

		private:

			void update(glm::vec2 _renderSize);
			bool mouseButtonCallback(const input::MouseButtonEvent& _event);

			graphics::Camera camera;
			graphics::Scene scene;

			boost::signals2::scoped_connection updateConnection;
			boost::signals2::scoped_connection mouseButtonConnection;
			graphics::View view;

			GUIMaterialManager materialManager;
			bool inputHandlingEnabled = true;

			std::vector<GUIElement*> rootElements;
		};
	}
}
