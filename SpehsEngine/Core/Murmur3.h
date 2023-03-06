#pragma once
/*
	MurmurHash3 was written by Austin Appleby, and is placed in the public domain. The author hereby disclaims copyright to this source code.
	C port by Peter Scott: https://github.com/PeterScott/murmur3
*/
#include <stdint.h>

namespace se
{
	struct Murmur3
	{
		struct Result128
		{
			uint8_t bytes[16] = { 0 };
		};

		static constexpr inline uint32_t rotl32(uint32_t x, int8_t r);
		static constexpr inline uint64_t rotl64(uint64_t x, int8_t r);
		static constexpr inline uint32_t fmix32(uint32_t h);
		static constexpr inline uint64_t fmix64(uint64_t k);

		static constexpr uint32_t murmurHash3_x86_32(const void* key, size_t length, uint32_t seed);
		static Result128 murmurHash3_x86_128(const void* key, size_t length, uint32_t seed);
		static Result128 murmurHash3_x64_128(const void* key, size_t length, uint32_t seed);
	};

	uint32_t murmurHash3_x86_32(const void* key, size_t length, uint32_t seed);

	// Block read - if your platform needs to do endian-swapping or can only handle aligned reads, do the conversion here
#define SE_GET_BLOCK(p, i) (p[i])
#define	SE_ROTL32(x,y) rotl32(x,y)
#define SE_ROTL64(x,y) rotl64(x,y)
#define SE_BIG_CONSTANT(x) (x##LLU)

	constexpr inline uint32_t Murmur3::rotl32(uint32_t x, int8_t r)
	{
		return (x << r) | (x >> (32 - r));
	}
	constexpr inline uint64_t Murmur3::rotl64(uint64_t x, int8_t r)
	{
		return (x << r) | (x >> (64 - r));
	}

	//-----------------------------------------------------------------------------
	// Finalization mix - force all bits of a hash block to avalanche

	constexpr inline uint32_t Murmur3::fmix32(uint32_t h)
	{
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;

		return h;
	}

	//----------

	constexpr inline uint64_t Murmur3::fmix64(uint64_t k)
	{
		k ^= k >> 33;
		k *= SE_BIG_CONSTANT(0xff51afd7ed558ccd);
		k ^= k >> 33;
		k *= SE_BIG_CONSTANT(0xc4ceb9fe1a85ec53);
		k ^= k >> 33;

		return k;
	}

	//-----------------------------------------------------------------------------

	constexpr uint32_t Murmur3::murmurHash3_x86_32(const void* key, size_t len, uint32_t seed)
	{
		const uint8_t* data = (const uint8_t*)key;
		const int nblocks = int(len) / 4;
		int i = 0;

		uint32_t h1 = seed;

		uint32_t c1 = 0xcc9e2d51;
		uint32_t c2 = 0x1b873593;

		//----------
		// body

		const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);

		for (i = -nblocks; i; i++)
		{
			uint32_t k1 = SE_GET_BLOCK(blocks, i);

			k1 *= c1;
			k1 = SE_ROTL32(k1, 15);
			k1 *= c2;

			h1 ^= k1;
			h1 = SE_ROTL32(h1, 13);
			h1 = h1 * 5 + 0xe6546b64;
		}

		//----------
		// tail

		const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);

		uint32_t k1 = 0;

		switch (int(len) & 3)
		{
		case 3: k1 ^= tail[2] << 16;
		case 2: k1 ^= tail[1] << 8;
		case 1: k1 ^= tail[0];
			k1 *= c1; k1 = SE_ROTL32(k1, 15); k1 *= c2; h1 ^= k1;
		};

		//----------
		// finalization

		h1 ^= int(len);

		h1 = fmix32(h1);

		return h1;
	}
}

#undef SE_GET_BLOCK
#undef SE_ROTL32
#undef SE_ROTL64
#undef SE_BIG_CONSTANT
