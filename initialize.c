#include "initialize.h"
#include "Keyboard.h"
#include "NumberInput.h"
#include "display.h"
#include "utils.h"
#include <reg51.h>

void init()
{
    initTimer();
    Display_init();
    Keyboard_init(&keyboard);
    NumberInput_init(&numberInput);
}

void initTimer()
{
    TMOD |= 0x01;            // Timer 0 16 bit
    TH0 = (-10000) / 0x0100; // 10 ms in 12MHz
    TL0 = (-10000) % 0x0100;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}
