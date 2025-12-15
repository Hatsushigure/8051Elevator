#include "initialize.h"
#include "Keyboard.h"
#include "NumberInput.h"
#include "display.h"
#include "utils.h"
#include <c8051F020.h>

void init()
{
    initWatchdog();
    initPin();
    Display_init();
    Keyboard_init();
    NumberInput_init();
    initTimer();
}
