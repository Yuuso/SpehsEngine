#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ViewInternal.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Internal/WindowInternal.h"


namespace se
{
	namespace graphics
	{
		ViewInternal::ViewInternal(View& _view)
			: view(&_view)
		{
			viewDestroyedConnection = view->destroyedSignal.connect(boost::bind(&ViewInternal::viewDestroyed, this));
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

			calculateRenderSize(_renderContext);
			view->onRenderSignal(renderSize);

			static_assert(ViewClearFlag::ClearColor == BGFX_CLEAR_COLOR &&
						  ViewClearFlag::ClearDepth == BGFX_CLEAR_DEPTH &&
						  ViewClearFlag::ClearStencil == BGFX_CLEAR_STENCIL,
						  "ViewClearFlags don't match bgfx values!");

			bgfx::setViewMode(_renderContext.currentViewId, bgfx::ViewMode::Default);
			bgfx::setViewClear(_renderContext.currentViewId, view->getClearFlags(), view->getClearColor(), 1.0f, 0);

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

		void ViewInternal::preRender(const bool _renderState, const bool _forceAllUpdates)
		{
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
		void ViewInternal::calculateRenderSize(RenderContext& _renderContext)
		{
			if (view->size.type == ViewSizeType::Relative)
			{
				renderSize.x = _renderContext.currentWindow->getWidth() * view->size.width;
				renderSize.y = _renderContext.currentWindow->getHeight() * view->size.height;
			}
			else
			{
				se_assert(view->size.type == ViewSizeType::Pixels);
				renderSize.x = view->size.width;
				renderSize.y = view->size.height;
			}

			if (view->offset.type == ViewSizeType::Relative)
			{
				renderOffset.x = _renderContext.currentWindow->getWidth() * view->offset.width;
				renderOffset.y = _renderContext.currentWindow->getHeight() * view->offset.height;
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
