#include "initialize.h"
#include "Keyboard.h"
#include "MaxPersonInput.h"
#include "PasswordInput.h"
#include "display.h"
#include "utils.h"
#include <reg51.h>

void init()
{
    initTimer();
    PasswordInput_init(&passwordInput);
    Keyboard_init(&keyboard);
    MaxPersonInput_init(&maxPersonInput);
}

void initTimer()
{
    TMOD |= 0x01;                    // Timer 0 16 bit
    TH0 = (0xFFFF - 10000) / 0x0100; // 10 ms in 12MHz
    TL0 = (0xFFFF - 10000) % 0x0100;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}
