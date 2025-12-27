#include "NumberInput.h"

NumberInput numberInput;

uint8_t NumberInput_getNumber()
{
    uint8_t i = 0;
    uint8_t result = 0;
    for (; i != numberInput.currentIndex; i++)
        result = result * 10 + numberInput.inputBuffer[i];
    return result;
}

