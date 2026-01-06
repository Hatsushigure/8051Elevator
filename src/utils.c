#include "utils.h"
#include "Joystick.h"
#include "Keyboard.h"

const char code PasswordPrompt[9] = "Password:";
const char code PersonPrompt[11] = "Max Person:";
const char code WeightPrompt[11] = "Max Weight:";
const char code FinishPrompt[13] = "Config Finish";
const char code PasswordWrongPrompt[14] = "Wrong Password";
const char code TrialCountPrompt[11] = "Tiral Left:";
const uint8_t code CorrectPassword[6] = {1, 1, 4, 5, 1, 4};
const char code OpenDoorPrompt[2] = "OP";
const char code CloseDoorPrompt[2] = "CL";
const char code DisableFloorPrompt[13] = "Unused Floors";
const char code AskConfigDisableFloorPrompt[16] = "Conf Unused Flr?";
const uint8_t code SpecialKeySequence[12] = {
    SK_Up,
    SK_Up,
    SK_Down,
    SK_Down,
    SK_Left,
    SK_Right,
    SK_Left,
    SK_Right,
    SK_B,
    SK_A,
    SK_B,
    SK_A
};

void refillTimer0()
{
    TH0 = (uint16_t)(-20000) >> 8;     /*10 ms in 2MHz*/
    TL0 = (uint16_t)(-20000) & 0x00FF; /*10 ms in 2MHz*/
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

