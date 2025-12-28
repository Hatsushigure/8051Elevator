#include "utils.h"
#include "Display.h"

const uint8_t code personPrompt[] = {DC_N, DC_O& DS_Dot};
const uint8_t code weightPrompt[] = {DC_R, DC_L& DS_Dot};
const uint8_t code finishPrompt[] = {DC_F, DC_1, DC_N, DC_1, DC_5, DC_H};
const uint8_t code errorPrompt[] = {DC_E, DC_R, DC_R};
const uint8_t code correctPassword[] = {1, 1, 4, 5, 1, 4};
const uint8_t code openDoorPrompt[] = "OP";
const uint8_t code closeDoorPrompt[] = "CL";

void myItoa(int8_t value, char* str)
{
    if (value < 0)
    {
        str[0] = '-';
        value = -value;
    } else
        str[0] = ' ';
    str[1] = '0' + value;
}

