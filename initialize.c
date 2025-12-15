#include "initialize.h"
#include "Keyboard.h"
#include "Led.h"
#include "NumberInput.h"
#include "display.h"
#include "utils.h"

void init()
{
    initWatchdog();
    initPin();
    Led_init();
    Display_init();
    Keyboard_init();
    NumberInput_init();
    initTimer();
}
