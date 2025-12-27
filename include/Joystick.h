#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "utils.h"

enum SpecialKey_1
{
    SK_Right = 16,
    SK_Down = 17,
    SK_Left = 18,
    SK_Up = 19
};

enum JoystickMask
{
    JM_Right = 0x01,
    JM_Down = 0x02,
    JM_Left = 0x04,
    JM_Top = 0x08
};

typedef struct
{
    uint8_t lastKeyState;
    uint8_t releasedKey;
} Joystick;

extern Joystick joystick;

#define Joystick_init()                                                        \
    {                                                                          \
        joystick.releasedKey = 0xFF;                                           \
        joystick.lastKeyState = 0xFF;                                          \
    }
void Joystick_getKey();

#endif // __JOYSTICK_H__
