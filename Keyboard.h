#include "utils.h"

enum KeyboardState
{
    KS_Free,
    KS_PrePress,
    KS_Pressed,
    KS_PreRelease,
    KS_Released
};

enum SpecialKey
{
    SK_A = 10,
    SK_E = 14,
    SK_Backspace = 14,
    SK_F = 15,
    SK_Enter = 15
};

typedef struct
{
    uint8_t pressedKey;
    uint8_t releasedKey;
    uint8_t prevKey;
    uint8_t state;
} Keyboard;

extern Keyboard keyboard;

#define Keyboard_init() keyboard.state = KS_Free;
void Keyboard_getKey();
