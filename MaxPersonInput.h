#ifndef __PERSON_INPUT_H__
#define __PERSON_INPUT_H__

#include "utils.h"

typedef struct
{
    uint8_t currentIndex;
    uint8_t inputBuffer[3];

} MaxPersonInput;

extern MaxPersonInput maxPersonInput;

void MaxPersonInput_init(MaxPersonInput* self);
void MaxPersonInput_append(MaxPersonInput* self, uint8_t key);
uint8_t MaxPersonInput_getPersonCount(MaxPersonInput* self);

#endif // __PERSON_INPUT_H__
