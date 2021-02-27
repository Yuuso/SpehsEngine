#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/RendererCallbackHandler.h"

#include "bimg/bimg.h"
#include "bx/readerwriter.h"
#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Core/File/FileSystem.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include <fstream>


namespace se
{
	namespace graphics
	{
		void RendererCallbackHandler::fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str)
		{
			std::string message;
			switch (_code)
			{
				case bgfx::Fatal::DebugCheck:
					message += "Debug check\n";
					break;
				case bgfx::Fatal::InvalidShader:
					message += "Invalid shader\n";
					break;
				case bgfx::Fatal::UnableToInitialize:
					message += "Unable to initialize\n";
					break;
				case bgfx::Fatal::UnableToCreateTexture:
					message += "Unable to create a texture\n";
					break;
				case bgfx::Fatal::DeviceLost:
					message += "Device lost\n";
					break;

				default:
					message += "Unknown error\n";
					break;
			}
			message += std::string(_filePath) + " (" + std::to_string(_line) + "): " + std::string(_str);
			log::error(message);
			exit(1 + _code);
		}
		void RendererCallbackHandler::traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList)
		{
			std::string message = std::string(_filePath) + " (" + std::to_string(_line) + "): ";
			constexpr size_t traceLen = 512u;
			char trace[traceLen] = { '\0' };
			vsnprintf(trace, traceLen, _format, _argList);
			message += trace;
			log::debug(message);
		}

		void RendererCallbackHandler::profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line)
		{
		}
		void RendererCallbackHandler::profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line)
		{
		}
		void RendererCallbackHandler::profilerEnd()
		{
		}

		static const std::string cacheDir = "cache";
		uint32_t RendererCallbackHandler::cacheReadSize(uint64_t _id)
		{
			const std::string filePath = cacheDir + "/" + std::to_string(_id);
			if (fileExists(filePath))
				return static_cast<uint32_t>(fileSize(filePath));
			return 0u;
		}
		bool RendererCallbackHandler::cacheRead(uint64_t _id, void* _data, uint32_t _size)
		{
			const std::string filePath = cacheDir + "/" + std::to_string(_id);
			return readFile(filePath, reinterpret_cast<uint8_t*>(_data), _size);
		}
		void RendererCallbackHandler::cacheWrite(uint64_t _id, const void* _data, uint32_t _size)
		{
			const std::string filePath = cacheDir + "/" + std::to_string(_id);
			if (verifyDirectory(cacheDir))
				writeFile(filePath, reinterpret_cast<const uint8_t*>(_data), _size);
		}

		void RendererCallbackHandler::screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip)
		{
			struct pngWriter : bx::WriterI
			{
				~pngWriter() = default;
				int32_t write(const void* _data, int32_t _size, bx::Error* _err)
				{
					stream.write(reinterpret_cast<const char*>(_data), _size);
					if (!stream.good())
					{
						log::warning("Failed to write screenshot data to file at: " + path + ". Failure occured while writing.");
						return 0u;
					}
					return _size;
				}
				bool open(const std::string& _path)
				{
					path = _path + ".png";
					int counter = 0;
					while (fileExists(path))
					{
						counter++;
						path = _path + std::to_string(counter) + ".png";
					}
					if (counter > 0)
					{
						log::warning("File already exists: " + _path + ".png. Writing to file " + path + " instead.");
					}
					stream.open(path, std::ios::binary);
					if (stream.fail())
					{
						log::warning("Failed to open file: " + path + ".");
						return false;
					}
					return true;
				}
				std::ofstream stream;
				std::string path;
			};

			pngWriter writer;
			if (!writer.open(_filePath))
			{
				log::error("Failed to write screenshot image file '" + writer.path + "'!");
				return;
			}
			bimg::imageWritePng(&writer, _width, _height, _pitch, _data, bimg::TextureFormat::BGRA8, _yflip);
		}

		void RendererCallbackHandler::captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip)
		{
			se_assert_m(false, "Video capture not implemented!");
		}
		void RendererCallbackHandler::captureEnd()
		{
		}
		void RendererCallbackHandler::captureFrame(const void* _data, uint32_t _size)
		{
		}
	}
}
