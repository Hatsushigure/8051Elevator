#ifndef __NUMBER_INPUT_H__
#define __NUMBER_INPUT_H__

#include "utils.h"

typedef struct
{
    uint8_t currentIndex;
    uint8_t inputBuffer[6];
    uint16_t result;
} NumberInput;

extern NumberInput numberInput;

#define NumberInput_clear() numberInput.currentIndex = 0;
#define NumberInput_init() NumberInput_clear();
void NumberInput_append(uint8_t key);
void NumberInput_backspace();
void NumberInput_getNumber();

#endif // __NUMBER_INPUT_H__
