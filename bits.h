#pragma once
#include <stdlib.h>

typedef unsigned char byte;

// lets call it a bitish.h PLS 🙏

#define GETBIT(a, b) (((a) & (1 << (b))) == 0 ? 0 : 1 )
#define SETBIT(a, b, c) (((c) == 0) ? ((a) &= ~(1 << (b))) : ((a) |= (1 << (b)) ) )
#define FLIPBIT(a, b) ((a) ^= (1 << (b)))
#define BITSIZEOF(a) (sizeof((a)) * 8)