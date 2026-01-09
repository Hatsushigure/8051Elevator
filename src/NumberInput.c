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

void NumberInput_getNumber()
{
    uint8_t i = 0;
    numberInput.result = 0;
    for (; i != numberInput.currentIndex; i++)
        numberInput.result =
            numberInput.result * 10 + numberInput.inputBuffer[i];
}

