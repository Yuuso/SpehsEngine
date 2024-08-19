#pragma once

#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Window.h"
#include <memory>
#include <vector>

namespace se
{
	namespace graphics
	{
		class ShaderManager;
		class TextureManager;
		class FontManager;
	}
}


namespace se
{
	namespace gui
	{
		class GUIElement;
		class GUIMaterialManager;

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
			void setLayerMaskStyle(LayerMaskStyle _style);

			void add(GUIElement& _element);
			void remove(GUIElement& _element);
			void clear();
			const std::vector<GUIElement*>& getRootElements() const;

			[[nodiscard]] boost::signals2::scoped_connection connectToRootElementAddedSignal(const std::function<void(GUIElement&)>& callback);

		private:

			void update(glm::vec2 _renderSize);
			bool mouseButtonCallback(const input::MouseButtonEvent& _event);

			graphics::Camera camera;
			graphics::Scene scene;

			boost::signals2::scoped_connection updateConnection;
			boost::signals2::scoped_connection mouseButtonConnection;
			boost::signals2::signal<void(GUIElement&)> rootElementAddedSignal;
			graphics::View view;

			std::unique_ptr<GUIMaterialManager> materialManager;
			bool inputHandlingEnabled = true;
			LayerMaskStyle layerMaskStyle = LayerMaskStyle::Incrementing;

			std::vector<GUIElement*> rootElements;
		};
	}
}
