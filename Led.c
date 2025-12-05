#include "Led.h"
#include <reg51.h>

void Led_turnOn(uint8_t index)
{
    uint8_t flag = 0xFE;
    flag <<= index;
    P3 &= flag;
}

void Led_turnOff(uint8_t index)
{
    uint8_t flag = 0x01;
    flag <<= index;
    P3 |= flag;
}

void Led_toggle(uint8_t index)
{
    uint8_t flag = 0x01;
    flag <<= index;
    P3 ^= flag;
}
