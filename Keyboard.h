#include "utils.h"

enum KeyboardState
{
    Free,
    PrePress,
    Pressed,
    PreRelease,
    Released
};

enum SpecialKey
{
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

void Keyboard_init(Keyboard* self);
void Keyboard_getKey(Keyboard* self);
