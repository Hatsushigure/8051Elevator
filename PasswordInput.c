#include "PasswordInput.h"

PasswordInput passwordInput;

void PasswordInput_init(PasswordInput* self)
{
    self->trialLeft = 5;
    PasswordInput_clear(self);
}

void PasswordInput_append(PasswordInput* self, uint8_t key)
{
    self->passwordInput[self->currentIndex] = key;
    self->currentIndex++;
}

void PasswordInput_backspace(PasswordInput* self)
{
    if (self->currentIndex == 0)
        return;
    self->currentIndex--;
    self->passwordInput[self->currentIndex] = 0xFF;
}

void PasswordInput_clear(PasswordInput* self)
{
    uint8_t i = 0;
    self->currentIndex = 0;
    for (; i < 6; i++)
        self->passwordInput[i] = 0xFF;
}
