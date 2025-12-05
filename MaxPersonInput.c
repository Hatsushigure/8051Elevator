#include "MaxPersonInput.h"

MaxPersonInput maxPersonInput;

void MaxPersonInput_init(MaxPersonInput* self) { self->currentIndex = 0; }

void MaxPersonInput_append(MaxPersonInput* self, uint8_t key)
{
    self->inputBuffer[self->currentIndex] = key;
    self->currentIndex++;
}

uint8_t MaxPersonInput_getPersonCount(MaxPersonInput* self)
{
    return self->inputBuffer[0] * 100 + self->inputBuffer[1] * 10 + self->inputBuffer[2];
}
