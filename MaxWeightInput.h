#ifndef __MAX_WEIGHT_INPUT_H__
#define __MAX_WEIGHT_INPUT_H__

#include "utils.h"

typedef struct
{
    uint8_t currentIndex;
    uint8_t inputBuffer[3];

} MaxWeightInput;

extern MaxWeightInput maxWeightInput;

void MaxWeightInput_init(MaxWeightInput* self);
void MaxWeightInput_append(MaxWeightInput* self, uint8_t key);
void MaxWeightInput_backspace(MaxWeightInput* self);
uint8_t MaxWeightInput_getMaxWeight(MaxWeightInput* self);

#endif // __MAX_WEIGHT_INPUT_H__
