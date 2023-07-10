/*
	MurmurHash3 was written by Austin Appleby, and is placed in the public domain. The author hereby disclaims copyright to this source code.
	C port by: https://github.com/PeterScott/murmur3
	Constexpr port by: https://github.com/AntonJohansson/StaticMurmur
*/
#pragma once

#include <stdint.h>

namespace se
{
	struct Murmur3
	{
	public:

		static constexpr uint32_t impl(const char* key, const size_t length, const uint32_t seed)
		{
			const int32_t len = int32_t(length);
			const int32_t nblocks = int32_t(len / 4);
			uint32_t h1 = seed;
			const uint32_t c1 = 0xcc9e2d51;
			const uint32_t c2 = 0x1b873593;
			for (int32_t i = 0; i < nblocks; i++)
			{
				uint32_t k1 = getBlock(key, i);
				k1 *= c1;
				k1 = rotl32(k1, 15);
				k1 *= c2;
				h1 ^= k1;
				h1 = rotl32(h1, 13);
				h1 = h1 * 5 + 0xe6546b64;
			}
			uint32_t k1 = 0;
			const int32_t tail_start = int32_t(len - (len % 4));
			switch (len & 3)
			{
			case 3: k1 ^= key[tail_start + 2] << 16;
			case 2: k1 ^= key[tail_start + 1] << 8;
			case 1: k1 ^= key[tail_start + 0];
				k1 *= c1; k1 = rotl32(k1, 15); k1 *= c2; h1 ^= k1;
			};
			h1 ^= len;
			h1 = fmix32(h1);
			return h1;
		}

		template<uint64_t Length>
		static constexpr uint32_t impl(const char(&s)[Length], const uint32_t seed)
		{
			return impl(s, size_t(Length - 1), seed);
		}

		static constexpr uint32_t impl(const std::string_view string, const uint32_t seed)
		{
			return impl(string.data(), string.size(), seed);
		}

	private:

		static constexpr uint32_t getBlock(const char* p, const int i)
		{
			uint32_t block =
				static_cast<uint8_t>(p[0 + i * 4]) << 0 |
				static_cast<uint8_t>(p[1 + i * 4]) << 8 |
				static_cast<uint8_t>(p[2 + i * 4]) << 16 |
				static_cast<uint8_t>(p[3 + i * 4]) << 24;
			return block;
		}

		static constexpr uint32_t rotl32(const uint32_t x, const int8_t r)
		{
			return (x << r) | (x >> (32 - r));
		}

		static constexpr uint32_t fmix32(uint32_t h)
		{
			h ^= h >> 16;
			h *= 0x85ebca6b;
			h ^= h >> 13;
			h *= 0xc2b2ae35;
			h ^= h >> 16;
			return h;
		}
	};
}
