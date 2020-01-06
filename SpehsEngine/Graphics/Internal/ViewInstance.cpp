#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ViewInstance.h"

#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"

#pragma warning(disable : 4127)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


namespace se
{
	namespace graphics
	{
		ViewInstance::ViewInstance(const View& _view)
			: view(_view)
		{
		}
		ViewInstance::~ViewInstance()
		{
		}

		bool ViewInstance::operator==(const View& _other)
		{
			return &view == &_other;
		}

		void ViewInstance::render(RenderContext& _renderContext)
		{
			bgfx::setViewClear(_renderContext.currentViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x101010ff * _renderContext.currentViewId, 1.0f, 0);

			float viewWidthPixels;
			float viewHeightPixels;
			if (view.size.type == ViewSize::Relative)
			{
				viewWidthPixels = _renderContext.currentWindow->getWidth() * view.size.width;
				viewHeightPixels = _renderContext.currentWindow->getHeight() * view.size.height;
			}
			else
			{
				se_assert(view.size.type == ViewSize::Pixels);
				viewWidthPixels = view.size.width;
				viewHeightPixels = view.size.height;
			}

			float viewOffsetXPixels;
			float viewOffsetYPixels;
			if (view.offset.type == ViewSize::Relative)
			{
				viewOffsetXPixels = _renderContext.currentWindow->getWidth() * view.offset.width;
				viewOffsetYPixels = _renderContext.currentWindow->getHeight() * view.offset.height;
			}
			else
			{
				se_assert(view.offset.type == ViewSize::Pixels);
				viewOffsetXPixels = view.offset.width;
				viewOffsetYPixels = view.offset.height;
			}

			// Camera
			{
				glm::mat4 viewMatrix;
				glm::mat4 projectionMatrix;

				switch (view.camera.getProjection())
				{
					case Projection::Perspective:
					{
						projectionMatrix = glm::perspective(glm::radians(view.camera.getFov()),
															viewWidthPixels / viewHeightPixels,
															view.camera.getNear(), view.camera.getFar());
						break;
					}
					case Projection::Orthographic:
					{
						projectionMatrix = glm::ortho(-viewWidthPixels * 0.01f * view.camera.getZoom(), viewWidthPixels * 0.01f * view.camera.getZoom(),
													  -viewHeightPixels * 0.01f * view.camera.getZoom(), viewHeightPixels * 0.01f * view.camera.getZoom(),
													  view.camera.getNear(), view.camera.getFar());
						break;
					}
					default:
						se_assert(false);
						break;
				}

				viewMatrix = glm::lookAt(view.camera.getPosition(), view.camera.getTarget(), view.camera.getUp());

				bgfx::setViewTransform(_renderContext.currentViewId, &viewMatrix, &projectionMatrix);
				bgfx::setViewRect(_renderContext.currentViewId, (uint16_t)viewOffsetXPixels, (uint16_t)viewOffsetYPixels, (uint16_t)viewWidthPixels, (uint16_t)viewHeightPixels);
			}

			bgfx::touch(_renderContext.currentViewId);

			view.scene.render(_renderContext);

			_renderContext.currentViewId++;
		}
	}
}
