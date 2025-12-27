#include "Joystick.h"

Joystick joystick;

void Joystick_getKey()
{
    uint8_t currentKeyState = P4 & 0x0F;
    uint8_t releaseState = (~joystick.lastKeyState) & currentKeyState;
    uint8_t key = SK_Right;
    uint8_t mask = 0x01;
    joystick.releasedKey = 0xFF;
    for (; key != SK_Up + 1; key++)
    {
        if (releaseState & mask)
            joystick.releasedKey = key;
        mask <<= 1;
    }
    joystick.lastKeyState = currentKeyState;
}
