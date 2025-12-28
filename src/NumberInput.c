#include "NumberInput.h"

NumberInput numberInput;

void NumberInput_append(uint8_t key)
{
    numberInput.inputBuffer[numberInput.currentIndex] = key;
    numberInput.currentIndex++;
}

void NumberInput_backspace()
{
    if (numberInput.currentIndex)
        numberInput.currentIndex--;
}

uint8_t NumberInput_getNumber()
{
    uint8_t i = 0;
    uint8_t result = 0;
    for (; i != numberInput.currentIndex; i++)
        result = result * 10 + numberInput.inputBuffer[i];
    return result;
}

