#ifndef __NUMBER_INPUT_H__
#define __NUMBER_INPUT_H__

#include "utils.h"

typedef struct
{
    uint8_t currentIndex;
    uint8_t inputBuffer[6];
    uint8_t result;
} NumberInput;

extern NumberInput numberInput;

#define NumberInput_clear() numberInput.currentIndex = 0;
#define NumberInput_init() NumberInput_clear();
#define NumberInput_append(key)                                                \
    {                                                                          \
        numberInput.inputBuffer[numberInput.currentIndex] = key;               \
        numberInput.currentIndex++;                                            \
    }
#define NumberInput_backspace()                                                \
    if (numberInput.currentIndex)                                              \
        numberInput.currentIndex--;
uint8_t NumberInput_getNumber();

#endif // __NUMBER_INPUT_H__
