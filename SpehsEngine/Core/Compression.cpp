#define ZLIB_WINAPI//Needed for linkage
#include "SpehsEngine/Core/Compression.h"
#include "SpehsEngine/Core/Exceptions.h"

//#include "zlib/zlib.h"
//#pragma comment(lib, "zlibwapi.lib") // for access to the DLL

namespace se
{
	namespace compression
	{
		//size_t getMaxCompressedSize(int bytes)
		//{
		//	return (bytes + 6 + (((bytes + 16383) / 16384)/*n16kBlocks*/ * 5));
		//}
		//size_t compress(const void* source, int sourceBytes, void* destination, int destinationBytes)
		//{
		//	z_stream zInfo = { 0 };
		//	zInfo.total_in = zInfo.avail_in = sourceBytes;
		//	zInfo.total_out = zInfo.avail_out = destinationBytes;
		//	zInfo.next_in = (Bytef*)source;
		//	zInfo.next_out = (Bytef*)destination;

		//	int compressedBytes = -1;
		//	int error = deflateInit(&zInfo, Z_DEFAULT_COMPRESSION);
		//	if (error == Z_OK)
		//	{
		//		error = deflate(&zInfo, Z_FINISH);
		//		if (error == Z_STREAM_END)
		//			compressedBytes = zInfo.total_out;
		//		else
		//			exceptions::unexpectedError("Failed to compress source data! Error code: " + error);
		//	}
		//	else
		//		exceptions::unexpectedError("Failed to compress source data! Error code: " + error);
		//	deflateEnd(&zInfo);
		//	return(compressedBytes);
		//}
		//size_t uncompress(const void* source, int sourceBytes, void* destination, int destinationBytes)
		//{
		//	z_stream zInfo = { 0 };
		//	zInfo.total_in = zInfo.avail_in = sourceBytes;
		//	zInfo.total_out = zInfo.avail_out = destinationBytes;
		//	zInfo.next_in = (Bytef*)source;
		//	zInfo.next_out = (Bytef*)destination;

		//	int uncompressedBytes = -1;
		//	int error = inflateInit(&zInfo);
		//	if (error == Z_OK)
		//	{
		//		error = inflate(&zInfo, Z_FINISH);
		//		if (error == Z_STREAM_END)
		//			uncompressedBytes = zInfo.total_out;
		//		else
		//			exceptions::unexpectedError("Failed to uncompress source data! Error code: " + error);
		//	}
		//	else
		//		exceptions::unexpectedError("Failed to uncompress source data! Error code: " + error);
		//	inflateEnd(&zInfo);
		//	return(uncompressedBytes);
		//}
	}
}