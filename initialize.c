#include "initialize.h"
#include "Joystick.h"
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
    Joystick_init();
    NumberInput_init();
    initTimer();
}
