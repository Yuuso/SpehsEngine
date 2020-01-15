#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ViewInstance.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"

#pragma warning(disable : 4127)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


namespace se
{
	namespace graphics
	{
		ViewInstance::ViewInstance(View& _view)
			: view(&_view)
		{
			viewDestroyedConnection = view->destroyedSignal.connect(boost::bind(&ViewInstance::viewDestroyed, this));
		}
		ViewInstance::~ViewInstance()
		{
		}

		void ViewInstance::viewDestroyed()
		{
			view = nullptr;
		}

		bool ViewInstance::operator==(const View& _other) const
		{
			return view == &_other;
		}

		void ViewInstance::render(RenderContext& _renderContext)
		{
			se_assert(view);

			static_assert(ViewClearFlag::ClearColor == BGFX_CLEAR_COLOR
						  && ViewClearFlag::ClearDepth == BGFX_CLEAR_DEPTH
						  && ViewClearFlag::ClearStencil == BGFX_CLEAR_STENCIL, "ViewClearFlags don't match bgfx values!");
			bgfx::setViewClear(_renderContext.currentViewId, view->getClearFlags(), view->getClearColor(), 1.0f, 0);

			float viewWidthPixels;
			float viewHeightPixels;
			if (view->size.type == ViewSize::Relative)
			{
				viewWidthPixels = _renderContext.currentWindow->getWidth() * view->size.width;
				viewHeightPixels = _renderContext.currentWindow->getHeight() * view->size.height;
			}
			else
			{
				se_assert(view->size.type == ViewSize::Pixels);
				viewWidthPixels = view->size.width;
				viewHeightPixels = view->size.height;
			}

			float viewOffsetXPixels;
			float viewOffsetYPixels;
			if (view->offset.type == ViewSize::Relative)
			{
				viewOffsetXPixels = _renderContext.currentWindow->getWidth() * view->offset.width;
				viewOffsetYPixels = _renderContext.currentWindow->getHeight() * view->offset.height;
			}
			else
			{
				se_assert(view->offset.type == ViewSize::Pixels);
				viewOffsetXPixels = view->offset.width;
				viewOffsetYPixels = view->offset.height;
			}

			// Camera
			{
				glm::mat4 viewMatrix;
				glm::mat4 projectionMatrix;

				switch (view->camera.getProjection())
				{
					case Projection::Perspective:
					{
						projectionMatrix = glm::perspective(glm::radians(view->camera.getFov()),
															viewWidthPixels / viewHeightPixels,
															view->camera.getNear(), view->camera.getFar());
						break;
					}
					case Projection::Orthographic:
					{
						projectionMatrix = glm::orthoRH_ZO(-(viewWidthPixels * 0.5f) * view->camera.getZoom(),  (viewWidthPixels * 0.5f) * view->camera.getZoom(),
														   -(viewHeightPixels * 0.5f) * view->camera.getZoom(), (viewHeightPixels * 0.5f) * view->camera.getZoom(),
														   view->camera.getNear(), view->camera.getFar());
						break;
					}
					default:
						se_assert(false);
						break;
				}

				viewMatrix = glm::lookAt(view->camera.getPosition(), view->camera.getTarget(), view->camera.getUp());

				bgfx::setViewTransform(_renderContext.currentViewId, &viewMatrix, &projectionMatrix);
				bgfx::setViewRect(_renderContext.currentViewId, (uint16_t)viewOffsetXPixels, (uint16_t)viewOffsetYPixels, (uint16_t)viewWidthPixels, (uint16_t)viewHeightPixels);
			}

			bgfx::touch(_renderContext.currentViewId);

			_renderContext.enableMSAA = view->getMSAAEnabled() && (checkBit(_renderContext.rendererFlags, MSAA2) ||
																   checkBit(_renderContext.rendererFlags, MSAA4) ||
																   checkBit(_renderContext.rendererFlags, MSAA8) ||
																   checkBit(_renderContext.rendererFlags, MSAA16));

			view->scene.render(_renderContext);

			_renderContext.currentViewId++;
		}

		const bool ViewInstance::wasDestroyed() const
		{
			return view == nullptr;
		}
	}
}
