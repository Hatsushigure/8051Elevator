#include "keyboard.h"
#include "utils.h"

Keyboard keyboard;

void Keyboard_getKey()
{
    uint8_t currentKey = 0xFF;
    uint8_t rowMask = 0x08;
    uint8_t colMask = 0x10;
    uint8_t row = 0;
    uint8_t col = 0;

    for (; row != 4; row++)
    {
        P1 = ~rowMask;
        if (P1 != ~rowMask)
            break;
        rowMask >>= 1;
    }
    for (; col != 4; col++)
    {
        P1 = ~colMask;
        if (P1 != ~colMask)
            break;
        colMask <<= 1;
    }
    if (row != 4 && col != 4)
        currentKey = row * 4 + col;

    switch (keyboard.state)
    {
    case KS_Free:
        if (currentKey != (uint8_t)0xFF)
        {
            keyboard.state = KS_PrePress;
            keyboard.pressedKey = currentKey;
        }
        break;
    case KS_PrePress:
        if (currentKey == (uint8_t)0xFF)
        {
            keyboard.state = KS_Free;
            break;
        }
        if (currentKey == keyboard.prevKey)
        {
            keyboard.pressedKey = currentKey;
            keyboard.state = KS_Pressed;
            break;
        }
        break;
    case KS_Pressed:
        if (currentKey == (uint8_t)0xFF)
        {
            keyboard.state = KS_PreRelease;
            keyboard.releasedKey = keyboard.pressedKey;
            break;
        }
        if (currentKey != keyboard.pressedKey)
        {
            keyboard.state = KS_PrePress;
            break;
        }
        break;
    case KS_PreRelease:
        if (currentKey == (uint8_t)0xFF)
        {
            keyboard.state = KS_Released;
            break;
        }
        if (currentKey == keyboard.prevKey)
        {
            keyboard.pressedKey = currentKey;
            keyboard.state = KS_Pressed;
            break;
        }
        break;
    case KS_Released:
        if (currentKey == (uint8_t)0xFF)
        {
            keyboard.state = KS_Free;
            break;
        }
        keyboard.state = KS_PreRelease;
        break;
    }
    keyboard.prevKey = currentKey;
}
