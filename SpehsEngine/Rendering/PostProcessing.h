
#pragma once


namespace se
{
	namespace graphics
	{
		namespace postproc
		{
			void setPostProcessingShader(const int _shaderIndex);
			void enablePostProcessing();
			void disablePostProcessing();
			bool postProcessingEnabled();
			void postProcessingReshape();
		}
	}
}