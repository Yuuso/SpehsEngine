
#pragma once


namespace se
{
	namespace rendering
	{
		void setPostProcessingShader(const int _shaderIndex);
		void enablePostProcessing();
		void disablePostProcessing();
		bool postProcessingEnabled();
		void postProcessingReshape();
	}
}