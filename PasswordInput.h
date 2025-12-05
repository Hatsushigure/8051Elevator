#ifndef __PASSWORD_INPUT_H__
#define __PASSWORD_INPUT_H__

#include "utils.h"

typedef struct
{
    uint8_t trialLeft;
    uint8_t passwordInput[6];
    uint8_t currentIndex;
} PasswordInput;

extern PasswordInput passwordInput;

void PasswordInput_init(PasswordInput* self);
void PasswordInput_append(PasswordInput* self, uint8_t key);
void PasswordInput_backspace(PasswordInput* self);
void PasswordInput_clear(PasswordInput* self);

#endif // __PASSWORD_INPUT_H__
