#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ViewInstance.h"

#include "SpehsEngine/Core/SE_Assert.h"

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
			// Camera
			{
				glm::mat4 viewMatrix;
				glm::mat4 projectionMatrix;

				switch (view.camera.getProjection())
				{
					case Camera::Projection::Perspective:
					{
						projectionMatrix = glm::perspective(glm::radians(view.camera.getFov()),
															view.getWidth() / view.getHeight(),
															view.camera.getNear(), view.camera.getFar());
						break;
					}
					case Camera::Projection::Orthographic:
					{
						projectionMatrix = glm::ortho(-view.getWidth() * 0.01f * view.camera.getZoom(), view.getWidth() * 0.01f * view.camera.getZoom(),
													  -view.getHeight() * 0.01f * view.camera.getZoom(), view.getHeight() * 0.01f * view.camera.getZoom(),
													  view.camera.getNear(), view.camera.getFar());
						break;
					}
					default:
						se_assert(false);
						break;
				}

				viewMatrix = glm::lookAt(view.camera.getPosition(), view.camera.getTarget(), view.camera.getUp());

				bgfx::setViewTransform(_renderContext.currentViewId, &viewMatrix, &projectionMatrix);
				bgfx::setViewRect(_renderContext.currentViewId, 0, 0, (uint16_t)view.getWidth(), (uint16_t)view.getHeight());
			}

			bgfx::touch(_renderContext.currentViewId);

			view.scene.render(_renderContext);

			_renderContext.currentViewId++;
		}
	}
}
