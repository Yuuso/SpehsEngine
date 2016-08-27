#pragma once
namespace spehs
{
	namespace compression
	{
		/// Returns the maximum compressed size for given byte count
		size_t getMaxCompressedSize(int bytes);

		/// Compresses data from source into destination. Returns byte size of the compressed data, -1 if uncompression fails.
		size_t compress(const void* source, int sourceBytes, void* destination, int destinationBytes);

		/// Uncompress source data (compressed with SpehsEngine) into destination. Returns byte size of the uncompressed data, -1 if uncompression fails.
		size_t uncompress(const void* source, int sourceBytes, void* destination, int destinationBytes);
	}
}