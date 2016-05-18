
#pragma once


namespace spehs
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