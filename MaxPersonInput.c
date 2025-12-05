#include "MaxPersonInput.h"

MaxPersonInput maxPersonInput;

void MaxPersonInput_init(MaxPersonInput* self)
{
    uint8_t i = 0;
    self->currentIndex = 0;
    for (; i < 3; i++)
        self->inputBuffer[i] = 0xFF;
}

void MaxPersonInput_append(MaxPersonInput* self, uint8_t key)
{
    self->inputBuffer[self->currentIndex] = key;
    self->currentIndex++;
}

void MaxPersonInput_backspace(MaxPersonInput* self)
{
    if (self->currentIndex == 0)
        return;
    self->currentIndex--;
    self->inputBuffer[self->currentIndex] = 0xFF;
}

uint8_t MaxPersonInput_getPersonCount(MaxPersonInput* self)
{
    return self->inputBuffer[0] * 100 + self->inputBuffer[1] * 10 + self->inputBuffer[2];
}
