#pragma once
/*
These bitwise operations are pretty fast but be very careful when using them.
'variable' parameter is the variable whose state we want to change.
'bit' is the bit which the operation is applied to.
	-For example, to change the least significant bit, use 1. To change the 4th least significant bit, use 8...
*/
#define enableBit(variable, bit) variable |= bit
#define disableBit(variable, bit) (variable &= ~bit)
#define toggleBit(variable, bit) (variable ^= bit)
#define checkBit(variable, bit) (variable & bit)