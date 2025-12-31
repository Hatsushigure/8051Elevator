#include "initialize.h"
#include "ElevatorControl.h"
#include "Joystick.h"
#include "Keyboard.h"
#include "LcdDisplay.h"
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
    LcdDisplay_init();
    Keyboard_init();
    Joystick_init();
    NumberInput_init();
    ElevatorControl_init();
    initTimer();
}

void initPin()
{
    P0MDOUT = 0xFF; // P0 pull-up
    P1MDOUT = 0xFF; // P1 pull-up
    P2MDOUT = 0xFF; // P2 pull-up
    P3MDOUT = 0xFF; // P3 pull-up
    P74OUT = 0x0F;  // P4 and P5 pull-up
    XBR2 = 0x40;
}

void initTimer()
{
    CKCON = 0x08;
    TCON = 0x10; /* Enable T0 */
    TMOD = 0x01; /* Timer 0 mode 1 */
    refillTimer0();
    IE = 0x82; /* Enable Timer 0 interrupt */
}

