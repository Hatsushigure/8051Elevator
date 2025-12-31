#include "display.h"
#include "utils.h"

Display display;

void Display_init()
{
    display.currentPromptValue = DS_Bottom;
    Display_clear();
    display.promptCount = 0;
}

void Display_displayCharacter(uint8_t item, uint8_t index)
{
    uint8_t delayTime = 100;
    P2 = ~index;
    P0 = item;
    shortDelay(delayTime);
    P2 = 0xFF;
}

void Display_refreshDisplay()
{
    uint8_t currentDigit = 0x01;
    uint8_t i = 0;
    for (; i != 6; i++)
    {
        Display_displayCharacter(display.displayBuffer[i], currentDigit);
        currentDigit <<= 1;
    }
    display.promptCount++;
    if (display.promptCount == 20)
    {
        display.promptCount = 0;
        display.currentPromptValue = ~(display.currentPromptValue ^ DS_Bottom);
    }
}

void Display_promptInput(uint8_t index)
{
    display.displayBuffer[index] = display.currentPromptValue;
}

void Display_clear()
{
    uint8_t i = 6;
    while (i)
    {
        i--;
        display.displayBuffer[i] = DS_Disabled;
    }
}
