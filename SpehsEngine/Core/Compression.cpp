#include "stdafx.h"
#define ZLIB_WINAPI//Needed for linkage
#include "SpehsEngine/Core/Compression.h"

#include "zlib/zlib.h"
//#pragma comment(lib, "zlibwapi.lib") // for access to the DLL


namespace se
{
	namespace compression
	{
		size_t getMaxCompressedSize(const size_t bytes)
		{
			return (bytes + 6 + (((bytes + 16383) / 16384)/*n16kBlocks*/ * 5));
		}

		std::optional<std::vector<uint8_t>> compress(const void* const data, const size_t size)
		{
			std::vector<uint8_t> buffer;
			buffer.resize(getMaxCompressedSize(size));

			z_stream zInfo = { 0 };
			zInfo.total_in = zInfo.avail_in = uInt(size);
			zInfo.total_out = zInfo.avail_out = uInt(buffer.size());
			zInfo.next_in = (Bytef*)data;
			zInfo.next_out = (Bytef*)buffer.data();

			int compressedBytes = -1;
			int error = deflateInit(&zInfo, Z_DEFAULT_COMPRESSION);
			if (error == Z_OK)
			{
				error = deflate(&zInfo, Z_FINISH);
				if (error == Z_STREAM_END)
				{
					compressedBytes = zInfo.total_out;
				}
				else
				{
					log::error("Failed to compress source data! Error code: " + error);
				}
			}
			else
			{
				log::error("Failed to compress source data! Error code: " + error);
			}
			deflateEnd(&zInfo);

			if (compressedBytes != -1)
			{
				buffer.resize(compressedBytes);
				return std::optional<std::vector<uint8_t>>(std::move(buffer));
			}
			else
			{
				return std::optional<std::vector<uint8_t>>();
			}
		}

		std::optional<std::vector<uint8_t>> uncompress(const void* const data, const size_t size)
		{
			std::vector<uint8_t> buffer;
			buffer.resize(size * 2); // Let's start with an output buffer double the size of input

			while (true)
			{
				z_stream zInfo = { 0 };
				zInfo.total_in = zInfo.avail_in = uInt(size);
				zInfo.total_out = zInfo.avail_out = uInt(buffer.size());
				zInfo.next_in = (Bytef*)data;
				zInfo.next_out = (Bytef*)buffer.data();

				int error = inflateInit(&zInfo);
				if (error == Z_OK)
				{
					error = inflate(&zInfo, Z_FINISH);
					if (error == Z_STREAM_END)
					{
						return std::optional<std::vector<uint8_t>>(std::move(buffer));
					}
					else if (error == Z_MEM_ERROR  || error == Z_BUF_ERROR)
					{
						// Increase output buffer size and retry
						buffer.resize(buffer.size() * 2);
						inflateEnd(&zInfo);
						continue;
					}
					else if (error == Z_VERSION_ERROR)
					{
						log::error("Failed to uncompress source data! Error: Z_VERSION_ERROR");
						return std::optional<std::vector<uint8_t>>();
					}
					else if (error == Z_STREAM_ERROR)
					{
						log::error("Failed to uncompress source data! Error: Z_STREAM_ERROR");
						return std::optional<std::vector<uint8_t>>();
					}
					else
					{
						log::error("Failed to uncompress source data! Error: " + std::to_string(error));
						return std::optional<std::vector<uint8_t>>();
					}
				}
				else
				{
					log::error("Failed to uncompress source data! Error code: " + error);
					return std::optional<std::vector<uint8_t>>();
				}
			}
		}
	}
}