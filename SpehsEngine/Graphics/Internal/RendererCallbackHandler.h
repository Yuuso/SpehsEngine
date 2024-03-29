#pragma once


namespace se
{
	namespace graphics
	{
		#define RUNTIME_SHADER_CACHE

		class RendererCallbackHandler : public bgfx::CallbackI
		{
		public:

			// Log
			void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override;
			void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override;

			// Profiler
			void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override;
			void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override;
			void profilerEnd() override;

			// Shader cache
			uint32_t cacheReadSize(uint64_t _id) override;
			bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override;
			void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override;

			// Screen shot
			void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) override;

			// Video capture
			void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) override;
			void captureEnd() override;
			void captureFrame(const void* _data, uint32_t _size) override;

		private:

#if defined(RUNTIME_SHADER_CACHE)
			std::unordered_map<uint64_t, std::vector<uint8_t>> cache;
#endif
		};
	}
}
