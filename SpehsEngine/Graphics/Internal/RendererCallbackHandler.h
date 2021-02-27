#pragma once

#include "bgfx/bgfx.h"


namespace se
{
	namespace graphics
	{
		class RendererCallbackHandler : public bgfx::CallbackI
		{
		public:
			// Log
			void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str);
			void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList);

			// Profiler
			void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line);
			void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line);
			void profilerEnd();

			// Shader cache
			uint32_t cacheReadSize(uint64_t _id);
			bool cacheRead(uint64_t _id, void* _data, uint32_t _size);
			void cacheWrite(uint64_t _id, const void* _data, uint32_t _size);

			// Screen shot
			void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip);

			// Video capture
			void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip);
			void captureEnd();
			void captureFrame(const void* _data, uint32_t _size);
		};
	}
}
