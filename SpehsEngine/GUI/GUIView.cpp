#include "stdafx.h"
#include "SpehsEngine/GUI/GUIView.h"

#include "SpehsEngine/GUI/Internal/StencilMaskManager.h"


namespace se
{
	namespace legacygui
	{
		static constexpr float cameraZPos = static_cast<float>(std::numeric_limits<gui::ZIndex>::max()) + 1.0f;

		GUIView::GUIView(gfx::ShaderManager& _shaderManager, gfx::TextureManager& _textureManager,
						 gfx::FontManager& _fontManager, input::EventSignaler& _eventSignaler, int _inputPriority)
			: view(scene, camera)
			, materialManager(_shaderManager, _textureManager, _fontManager)
		{
			view.setDrawCallSortOrder(gfx::DrawCallSortOrder::DepthAscending);
			view.setClearFlags(gfx::ViewClearFlag::ClearDepth | gfx::ViewClearFlag::ClearStencil);
			camera.setUp({ 0.0f, 1.0f, 0.0f });
			camera.setFar(cameraZPos * 2.0f);
			camera.setNear(1.0f);
			camera.setDirection({ 0.0f, 0.0f, -1.0f });
			camera.setProjection(se::gfx::Projection::Orthographic);

			updateConnection = view.connectToPreRenderSignal([this](glm::vec2 _renderSize){ update(_renderSize); });
			_eventSignaler.connectToMouseButtonSignal(mouseButtonConnection, [this](const input::MouseButtonEvent& _event){ return mouseButtonCallback(_event); }, _inputPriority);
		}
		gfx::View& GUIView::getView()
		{
			return view;
		}

		void GUIView::setInputHandlingEnabled(bool _value)
		{
			inputHandlingEnabled = _value;
		}
		bool GUIView::getInputhandlingEnabled() const
		{
			return inputHandlingEnabled;
		}
		void GUIView::setLayerMaskStyle(gui::LayerMaskStyle _style)
		{
			layerMaskStyle = _style;
		}

		void GUIView::update(glm::vec2 _renderSize)
		{
			camera.setPosition(glm::vec3{ _renderSize.x * 0.5f, -_renderSize.y * 0.5, cameraZPos });

			auto stencilMaskManager = gui::createStencilMaskManager(layerMaskStyle);
			UpdateContext context{ view.getScene(), materialManager, *stencilMaskManager, _renderSize };
			for (size_t i = 0; i < rootElements.size(); i++)
			{
				se_assert(!rootElements[i]->parent);
				rootElements[i]->preUpdate(context);
			}
			for (size_t i = 0; i < rootElements.size(); i++)
			{
				se_assert(!rootElements[i]->parent);
				rootElements[i]->update(context);
			}
		}

		bool GUIView::mouseButtonCallback(const input::MouseButtonEvent& _event)
		{
			if (!inputHandlingEnabled)
				return false;

			bool handled = false;
			const size_t maxRootElements = rootElements.size(); // Attempt to not handle elements added during the update
			for (size_t i = 0; i < std::min(maxRootElements, rootElements.size()); i++)
			{
				InputUpdateContext context{ _event };
				se_assert(!rootElements[i]->parent);
				handled = rootElements[i]->inputUpdate(context) || handled;
			}

			return handled;
		}

		void GUIView::add(GUIElement& _element)
		{
			auto it = std::find(rootElements.begin(), rootElements.end(), &_element);
			if (it != rootElements.end())
			{
				log::warning("Element already found in GUIView!");
				return;
			}
			se_assert(!_element.parent);
			rootElements.push_back(&_element);
			_element.addToView();
			rootElementAddedSignal(_element);
		}
		void GUIView::remove(GUIElement& _element)
		{
			auto it = std::find(rootElements.begin(), rootElements.end(), &_element);
			if (it == rootElements.end())
			{
				log::warning("Element not found in GUIView!");
				return;
			}
			(*it)->removeFromView();
			rootElements.erase(it);
		}

		void GUIView::clear()
		{
			for (auto&& element : rootElements)
				element->removeFromView();
			rootElements.clear();
		}

		const std::vector<GUIElement*>& GUIView::getRootElements() const
		{
			return rootElements;
		}

		boost::signals2::scoped_connection GUIView::connectToRootElementAddedSignal(const std::function<void(GUIElement&)>& callback)
		{
			return rootElementAddedSignal.connect(callback);
		}
	}
}
