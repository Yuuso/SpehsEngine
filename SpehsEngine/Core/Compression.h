#pragma once

#include <optional>


namespace se
{
	namespace compression
	{
		// Returns the maximum compressed size for given byte count
		size_t getMaxCompressedSize(const size_t bytes);

		std::optional<std::vector<uint8_t>> compress(const void* const data, const size_t size);

		/// Uncompress source data (compressed with SpehsEngine) into destination. Returns byte size of the uncompressed data, -1 if uncompression fails.
		std::optional<std::vector<uint8_t>> uncompress(const void* const data, const size_t size);
	}
}