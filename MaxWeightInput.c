#include "MaxWeightInput.h"

MaxWeightInput maxWeightInput;

void MaxWeightInput_init(MaxWeightInput* self)
{
    uint8_t i = 0;
    self->currentIndex = 0;
    for (; i < 3; i++)
        self->inputBuffer[i] = 0xFF;
}

void MaxWeightInput_append(MaxWeightInput* self, uint8_t key)
{
    self->inputBuffer[self->currentIndex] = key;
    self->currentIndex++;
}

void MaxWeightInput_backspace(MaxWeightInput* self)
{
    if (self->currentIndex == 0)
        return;
    self->currentIndex--;
    self->inputBuffer[self->currentIndex] = 0xFF;
}

uint8_t MaxWeightInput_getMaxWeight(MaxWeightInput* self)
{
    return self->inputBuffer[0] * 100 + self->inputBuffer[1] * 10 + self->inputBuffer[2];
}
