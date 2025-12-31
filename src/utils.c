#include "utils.h"
#include "Display.h"

const char code passwordPrompt[9] = "Password:";
const char code personPrompt[11] = "Max Person:";
const char code weightPrompt[11] = "Max Weight:";
const char code finishPrompt[13] = "Config Finish";
const char code passwordWrongPrompt[14] = "Wrong Password";
const char code trialCountPrompt[11] = "Tiral Left:";
const uint8_t code correctPassword[6] = {1, 1, 4, 5, 1, 4};
const char code openDoorPrompt[2] = "OP";
const char code closeDoorPrompt[2] = "CL";

void refillTimer0()
{
    TH0 = (uint16_t)(-20000) >> 8;     /*10 ms in 2MHz*/
    TL0 = (uint16_t)(-20000) & 0x00FF; /*10 ms in 2MHz*/
}

void refillTimer1()
{
    TH1 = (uint16_t)(-40000) >> 8;     /*20 ms in 2MHz*/
    TL1 = (uint16_t)(-40000) & 0x00FF; /*20 ms in 2MHz*/
}

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

