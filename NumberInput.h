#ifndef __NUMBER_INPUT_H__
#define __NUMBER_INPUT_H__

#include "utils.h"

typedef struct
{
    uint8_t currentIndex;
    uint8_t inputBuffer[8];
    uint8_t result;
} NumberInput;

extern NumberInput numberInput;

#define NumberInput_clear(self) (self)->currentIndex = 0;
#define NumberInput_init(self) NumberInput_clear(self);
#define NumberInput_append(self, key)                                                              \
    (self)->inputBuffer[(self)->currentIndex] = key;                                               \
    (self)->currentIndex++;
#define NumberInput_backspace(self)                                                                \
    if ((self)->currentIndex)                                                                      \
        (self)->currentIndex--;
uint8_t NumberInput_getNumber(NumberInput* self);

#endif // __NUMBER_INPUT_H__
