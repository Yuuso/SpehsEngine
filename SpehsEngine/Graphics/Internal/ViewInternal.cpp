#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ViewInternal.h"

#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Internal/WindowInternal.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/View.h"


namespace se
{
	namespace graphics
	{
		ViewInternal::ViewInternal(View& _view)
			: view(&_view)
		{
			viewDestroyedConnection = view->destroyedSignal.connect(std::bind(&ViewInternal::viewDestroyed, this));
		}
		ViewInternal::~ViewInternal()
		{
		}

		void ViewInternal::viewDestroyed()
		{
			view = nullptr;
		}

		bool ViewInternal::operator==(const View& _other) const
		{
			return view == &_other;
		}

		void ViewInternal::render(RenderContext& _renderContext)
		{
			se_assert(view);

			calculateRenderSize(_renderContext.currentWindow);

			static_assert(ViewClearFlag::ClearColor == BGFX_CLEAR_COLOR &&
						  ViewClearFlag::ClearDepth == BGFX_CLEAR_DEPTH &&
						  ViewClearFlag::ClearStencil == BGFX_CLEAR_STENCIL,
						  "ViewClearFlags don't match bgfx values!");

			bgfx::setViewMode(_renderContext.currentViewId, getViewMode(view->getDrawCallSortOrder()));
			bgfx::setViewClear(_renderContext.currentViewId, view->getClearFlags(), view->getClearColor());

			// Camera
			{
				const glm::mat4 viewMatrix = view->camera.getViewMatrix();
				const glm::mat4 projectionMatrix = view->camera.getProjectionMatrix(renderSize.x, renderSize.y);

				bgfx::setViewTransform(_renderContext.currentViewId, &viewMatrix, &projectionMatrix);
				bgfx::setViewRect(_renderContext.currentViewId, (uint16_t)renderOffset.x, (uint16_t)renderOffset.y, (uint16_t)renderSize.x, (uint16_t)renderSize.y);
			}

			bgfx::touch(_renderContext.currentViewId);

			_renderContext.enableMSAA = view->getMSAAEnabled() && (checkBit(_renderContext.rendererFlags, MSAA2) ||
																   checkBit(_renderContext.rendererFlags, MSAA4) ||
																   checkBit(_renderContext.rendererFlags, MSAA8) ||
																   checkBit(_renderContext.rendererFlags, MSAA16));

			view->scene.render(_renderContext);

			_renderContext.currentViewId++;
		}

		void ViewInternal::preRender(const WindowInternal* _currentWindow, const bool _renderState, const bool _forceAllUpdates)
		{
			calculateRenderSize(_currentWindow);
			view->onPreRenderSignal(renderSize);
			view->scene.preRender(_renderState, _forceAllUpdates || wasAdded);
		}
		void ViewInternal::postRender(const bool _renderState)
		{
			view->scene.postRender(_renderState);
			wasAdded = false;
		}

		const bool ViewInternal::wasDestroyed() const
		{
			return view == nullptr;
		}
		void ViewInternal::calculateRenderSize(const WindowInternal* _currentWindow)
		{
			if (view->size.type == ViewSizeType::Relative)
			{
				renderSize.x = _currentWindow->getWidth() * view->size.width;
				renderSize.y = _currentWindow->getHeight() * view->size.height;
			}
			else
			{
				se_assert(view->size.type == ViewSizeType::Pixels);
				renderSize.x = view->size.width;
				renderSize.y = view->size.height;
			}

			if (view->offset.type == ViewSizeType::Relative)
			{
				renderOffset.x = _currentWindow->getWidth() * view->offset.width;
				renderOffset.y = _currentWindow->getHeight() * view->offset.height;
			}
			else
			{
				se_assert(view->offset.type == ViewSizeType::Pixels);
				renderOffset.x = view->offset.width;
				renderOffset.y = view->offset.height;
			}
		}
	}
}
