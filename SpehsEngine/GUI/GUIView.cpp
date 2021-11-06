#include "stdafx.h"
#include "SpehsEngine/GUI/GUIView.h"


namespace se
{
	namespace gui
	{
		static constexpr float cameraZPos = static_cast<float>(std::numeric_limits<ZIndex>::max()) + 1.0f;

		GUIView::GUIView()
			: view(scene, camera)
		{
			view.setClearFlags(se::graphics::ViewClearFlag::ClearDepth);
			camera.setUp({ 0.0f, 1.0f, 0.0f });
			camera.setFar(cameraZPos * 2.0f);
			camera.setDirection({ 0.0f, 0.0f, -1.0f });
			camera.setProjection(se::graphics::Projection::Orthographic);

			updateConnection = view.connectToOnRenderSignal([this](glm::vec2 _renderSize){ update(_renderSize); });
		}
		graphics::View& GUIView::getView()
		{
			return view;
		}

		void GUIView::update(glm::vec2 _renderSize)
		{
			camera.setPosition(glm::vec3{ _renderSize.x * 0.5f, -_renderSize.y * 0.5, cameraZPos });

			UpdateContext context(*this);
			for (auto&& element : rootElements)
			{
				se_assert(!element->parent);
				element->update(context);
			}
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
	}
}
