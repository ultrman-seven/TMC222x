#ifndef D425699F_55F4_4745_A467_5254554813CF
#define D425699F_55F4_4745_A467_5254554813CF

#include <stdint.h>

template <uint8_t _n, uint8_t _poly>
uint8_t crcN_upTo8(uint8_t data, uint8_t initVal)
{
    data ^= initVal;
    for (uint8_t j = 0; j < _n; j++)
    {
        if (data & 0x8)
            data = (data << 1) ^ _poly;
        else
            data <<= 1;
    }
    return (data & (0xff >> (8 - _n)));
}

#endif /* D425699F_55F4_4745_A467_5254554813CF */
