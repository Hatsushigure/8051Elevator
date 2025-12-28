#include "utils.h"
#include "Display.h"

const char code passwordPrompt[] = "Password:";
const char code personPrompt[] = "Max Person:";
const char code weightPrompt[] = "Max Weight:";
const char code finishPrompt[] = "Config Finish";
const char code passwordWrongPrompt[] = "Wrong Password";
const char code trialCountPrompt[] = "Tiral Left: ";
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

