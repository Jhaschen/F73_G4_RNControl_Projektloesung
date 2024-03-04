#pragma once

#define setBit(register, bit) ((register) |= (1 << (bit)))
#define clrBit(register, bit) ((register) &= (~(1 << (bit))))
#define tglBit(register, bit) ((register) ^= (1 << (bit)))
#define isBitSet(register, bit) ((register) & (1 << (bit)))
#define isBitClr(register, bit) (!((register) & (1 << (bit))))