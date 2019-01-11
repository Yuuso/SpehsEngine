#pragma once
/*
	MurmurHash3 was written by Austin Appleby, and is placed in the public domain. The author hereby disclaims copyright to this source code.
	C port by Peter Scott: https://github.com/PeterScott/murmur3
*/
#include <stdint.h>

namespace se
{
	uint32_t murmurHash3_x86_32(const void *key, int len, uint32_t seed);
	void murmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);
	void murmurHash3_x64_128(const void *key, int len, uint32_t seed, void *out);
}
