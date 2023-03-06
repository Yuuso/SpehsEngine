#include "stdafx.h"
#include "SpehsEngine/Core/Murmur3.h"
//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

namespace se
{
	//-----------------------------------------------------------------------------
	// Platform-specific functions and macros
	
	// Block read - if your platform needs to do endian-swapping or can only handle aligned reads, do the conversion here
#define SE_GET_BLOCK(p, i) (p[i])
#define	SE_ROTL32(x,y) rotl32(x,y)
#define SE_ROTL64(x,y) rotl64(x,y)
#define SE_BIG_CONSTANT(x) (x##LLU)

	//-----------------------------------------------------------------------------

	Murmur3::Result128 Murmur3::murmurHash3_x64_128(const void * key, const size_t len, const uint32_t seed)
	{
		const uint8_t * data = (const uint8_t*)key;
		const int nblocks = int(len) / 16;
		int i = 0;

		uint64_t h1 = seed;
		uint64_t h2 = seed;

		uint64_t c1 = SE_BIG_CONSTANT(0x87c37b91114253d5);
		uint64_t c2 = SE_BIG_CONSTANT(0x4cf5ad432745937f);

		//----------
		// body

		const uint64_t * blocks = (const uint64_t *)(data);

		for (i = 0; i < nblocks; i++)
		{
			uint64_t k1 = SE_GET_BLOCK(blocks, i * 2 + 0);
			uint64_t k2 = SE_GET_BLOCK(blocks, i * 2 + 1);

			k1 *= c1; k1 = SE_ROTL64(k1, 31); k1 *= c2; h1 ^= k1;

			h1 = SE_ROTL64(h1, 27); h1 += h2; h1 = h1 * 5 + 0x52dce729;

			k2 *= c2; k2 = SE_ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

			h2 = SE_ROTL64(h2, 31); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
		}

		//----------
		// tail

		const uint8_t * tail = (const uint8_t*)(data + nblocks * 16);

		uint64_t k1 = 0;
		uint64_t k2 = 0;

		switch (int(len) & 15)
		{
		case 15: k2 ^= (uint64_t)(tail[14]) << 48;
		case 14: k2 ^= (uint64_t)(tail[13]) << 40;
		case 13: k2 ^= (uint64_t)(tail[12]) << 32;
		case 12: k2 ^= (uint64_t)(tail[11]) << 24;
		case 11: k2 ^= (uint64_t)(tail[10]) << 16;
		case 10: k2 ^= (uint64_t)(tail[9]) << 8;
		case  9: k2 ^= (uint64_t)(tail[8]) << 0;
			k2 *= c2; k2 = SE_ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

		case  8: k1 ^= (uint64_t)(tail[7]) << 56;
		case  7: k1 ^= (uint64_t)(tail[6]) << 48;
		case  6: k1 ^= (uint64_t)(tail[5]) << 40;
		case  5: k1 ^= (uint64_t)(tail[4]) << 32;
		case  4: k1 ^= (uint64_t)(tail[3]) << 24;
		case  3: k1 ^= (uint64_t)(tail[2]) << 16;
		case  2: k1 ^= (uint64_t)(tail[1]) << 8;
		case  1: k1 ^= (uint64_t)(tail[0]) << 0;
			k1 *= c1; k1 = SE_ROTL64(k1, 31); k1 *= c2; h1 ^= k1;
		};

		//----------
		// finalization

		h1 ^= int(len); h2 ^= int(len);

		h1 += h2;
		h2 += h1;

		h1 = fmix64(h1);
		h2 = fmix64(h2);

		h1 += h2;
		h2 += h1;

		Result128 result;
		((uint64_t*)result.bytes[0])[0] = h1;
		((uint64_t*)result.bytes[8])[2] = h2;
		return result;
	}
	uint32_t murmurHash3_x86_32(const void* key, size_t length, uint32_t seed)
	{
		return Murmur3::murmurHash3_x86_32(key, length, seed);
	}
}
