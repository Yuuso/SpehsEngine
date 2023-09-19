#pragma once

#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/GUI/GUIElement.h"
#include "SpehsEngine/GUI/Internal/GUIMaterialManager.h"
#include "SpehsEngine/GUI/Internal/StencilMaskManager.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include <memory>
#include <vector>


namespace se
{
	namespace legacygui
	{
		class GUIView
		{
		public:

			GUIView(gfx::ShaderManager& _shaderManager, gfx::TextureManager& _textureManager,
					gfx::FontManager& _fontManager, input::EventSignaler& _eventSignaler, int _inputPriority);
			~GUIView() = default;
			GUIView(const GUIView& _other) = delete;
			GUIView& operator=(const GUIView& _other) = delete;
			GUIView(GUIView&& _other) = delete;
			GUIView& operator=(GUIView&& _other) = delete;


			gfx::View& getView();
			void setInputHandlingEnabled(bool _value);
			bool getInputhandlingEnabled() const;
			void setLayerMaskStyle(gui::LayerMaskStyle _style);

			void add(GUIElement& _element);
			void remove(GUIElement& _element);
			void clear();
			const std::vector<GUIElement*>& getRootElements() const;

			[[nodiscard]] boost::signals2::scoped_connection connectToRootElementAddedSignal(const std::function<void(GUIElement&)>& callback);

		private:

			void update(glm::vec2 _renderSize);
			bool mouseButtonCallback(const input::MouseButtonEvent& _event);

			gfx::Camera camera;
			gfx::Scene scene;

			boost::signals2::scoped_connection updateConnection;
			boost::signals2::scoped_connection mouseButtonConnection;
			boost::signals2::signal<void(GUIElement&)> rootElementAddedSignal;
			gfx::View view;

			GUIMaterialManager materialManager;
			bool inputHandlingEnabled = true;
			gui::LayerMaskStyle layerMaskStyle = gui::LayerMaskStyle::Incrementing;

			std::vector<GUIElement*> rootElements;
		};
	}
}
