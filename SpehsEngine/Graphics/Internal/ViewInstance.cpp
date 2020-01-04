#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ViewInstance.h"

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

				switch (view.camera.projectionGet())
				{
					case Camera::Projection::perspective:
					{
						projectionMatrix = glm::perspective(glm::radians(view.camera.fovGet()),
															view.widthGet() / view.heightGet(),
															view.camera.nearGet(), view.camera.farGet());
						break;
					}
					case Camera::Projection::orthographic:
					{
						projectionMatrix = glm::ortho(-view.widthGet() * 0.01f * view.camera.zoomGet(), view.widthGet() * 0.01f * view.camera.zoomGet(),
													  -view.heightGet() * 0.01f * view.camera.zoomGet(), view.heightGet() * 0.01f * view.camera.zoomGet(),
													  view.camera.nearGet(), view.camera.farGet());
						break;
					}
					default:
						se_assert(false);
						break;
				}

				viewMatrix = glm::lookAt(view.camera.positionGet(), view.camera.directionGet(), view.camera.worldUpGet());

				bgfx::setViewTransform(_renderContext.currentViewId, &viewMatrix, &projectionMatrix);
				bgfx::setViewRect(_renderContext.currentViewId, 10, 10, (uint16_t)view.widthGet() - 20, (uint16_t)view.heightGet() - 20);
			}

			bgfx::touch(_renderContext.currentViewId);

			view.scene.render(_renderContext);

			_renderContext.currentViewId++;
		}
	}
}
