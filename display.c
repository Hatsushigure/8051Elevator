#include "display.h"
#include "utils.h"
#include <reg51.h>

uint8_t displayBuffer[8] = {
    DS_Disabled,
    DS_Disabled,
    DS_Disabled,
    DS_Disabled,
    DS_Disabled,
    DS_Disabled,
    DS_Disabled,
    DS_Disabled
};
uint8_t promptCount = 0;
uint8_t currentPromptValue = DS_Bottom;
int8_t disappearCounter[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

void shortDelay(uint8_t time)
{
    while (time--)
        ;
}

void displayCharacter(uint8_t item, uint8_t index)
{
    P2 = 0;
    P0 = DS_Disabled;
    shortDelay(10);
    P2 = index;
    P0 = item;
    shortDelay(100);
}

void displayString(uint8_t* str)
{
    int i = 0;
    int currentDigit = 0x01;
    for (i = 0; i < 8; i++)
    {
        displayCharacter(str[i], currentDigit);
        currentDigit <<= 1;
    }
}

void refreshDisplay()
{
    uint8_t i = 0;
    for (; i < 8; i++)
    {
        if (disappearCounter[i] == 0)
            displayBuffer[i] = DS_Middle;
        if (disappearCounter[i] >= 0)
            disappearCounter[i]--;
    }
    displayString(displayBuffer);
}

void promptInput(uint8_t index)
{
    displayBuffer[index] = currentPromptValue;
    promptCount++;
    if (promptCount == 20)
    {
        promptCount = 0;
        if (currentPromptValue == DS_Bottom)
            currentPromptValue = DS_Disabled;
        else
            currentPromptValue = DS_Bottom;
    }
}

void delayDisappear(uint8_t index, int8_t time) { disappearCounter[index] = time; }

void resetDelayDisappear()
{
    uint8_t i = 0;
    for (; i < 8; i++)
        disappearCounter[i] = -1;
}
