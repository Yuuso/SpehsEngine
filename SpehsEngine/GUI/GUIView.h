#pragma once

#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/View.h"
#include <memory>
#include <vector>

namespace se
{
	class AssetManager;
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

			SE_NO_COPY_OR_MOVE(GUIView);
			GUIView(AssetManager& _assetManager, input::EventSignaler& _eventSignaler, int _inputPriority);
			~GUIView();

			gfx::View& getView();
			void setInputHandlingEnabled(bool _value);
			bool getInputhandlingEnabled() const;
			void setLayerMaskStyle(LayerMaskStyle _style);
			void setTextureModes(const gfx::TextureModes& _textureModes);

			void add(GUIElement& _element);
			void remove(GUIElement& _element);
			void clear();
			const std::vector<GUIElement*>& getRootElements() const;

			[[nodiscard]] ScopedConnection connectToRootElementAddedSignal(const std::function<void(GUIElement&)>& callback);

		private:

			void update(glm::vec2 _renderSize);
			bool mouseButtonCallback(const input::MouseButtonEvent& _event);

			gfx::Camera camera;
			gfx::Scene scene;

			ScopedConnection updateConnection;
			ScopedConnection mouseButtonConnection;
			Signal<void(GUIElement&)> rootElementAddedSignal;
			gfx::View view;

			std::unique_ptr<GUIMaterialManager> materialManager;
			bool inputHandlingEnabled = true;
			gui::LayerMaskStyle layerMaskStyle = gui::LayerMaskStyle::Incrementing;

			std::vector<GUIElement*> rootElements;
		};
	}
}
