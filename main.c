#include "MaxPersonInput.h"
#include "PasswordInput.h"
#include "display.h"
#include "initialize.h"
#include "keyboard.h"
#include "utils.h"
#include <reg51.h>

#ifdef __VSCODE_C51__
#define INTERRUPT(x)
#else
#define INTERRUPT(x) interrupt x
#endif

enum WorkState
{
    WS_Password,
    WS_Person,
    WS_Weight
};
uint8_t workState = WS_Password;
uint8_t castTable[] = {
    DC_0, DC_1, DC_2, DC_3, DC_4, DC_5, DC_6, DC_7, DC_8, DC_9, DC_A, DC_B, DC_C, DC_D, DC_E, DC_F
};
uint8_t truePassword[] = {1, 1, 4, 5, 1, 4};
bit passwordIsRight = 1;
uint8_t personPrompt[] = {
    DC_N, DC_O& DS_Dot, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled
};
uint8_t errorPrompt[] = {DC_E, DC_R, DC_R, DC_O, DC_R, DS_Disabled, DS_Disabled, DS_Disabled};
uint8_t weightPrompt[] = {
    DC_R, DC_L& DS_Dot, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled
};

void getPassword();
void getMaxPerson();
void varifyPassword();

int main()
{
    init();
    workState = WS_Password;
    while (1)
        ;
}

void onTimer0Timeout() INTERRUPT(1)
{
    EA = 0;
    TH0 = (0xFFFF - 10000) / 0x0100; // 10 ms in 12MHz
    TL0 = (0xFFFF - 10000) % 0x0100;
    switch (workState)
    {
    case WS_Password:
        if (passwordInput.currentIndex < 6)
            getPassword();
        else
        {
            displayBuffer[4] = DS_Middle;
            displayBuffer[5] = DS_Middle;
            displayBuffer[6] = DS_Disabled;
            varifyPassword();
        }
        break;
    case WS_Person:
        getMaxPerson();
        break;
    case WS_Weight:
        break;
    }
    refreshDisplay();
    EA = 1;
}

void getPassword()
{
    promptInput(passwordInput.currentIndex);
    Keyboard_getKey(&keyboard);
    if (keyboard.state == Released)
    {
        PasswordInput_setCurrentDigit(&passwordInput, keyboard.releasedKey);
        displayBuffer[passwordInput.currentIndex] = castTable[keyboard.releasedKey];
        delayDisappear(passwordInput.currentIndex, 50);
        if (keyboard.releasedKey != truePassword[passwordInput.currentIndex])
            passwordIsRight = 0;
        passwordInput.currentIndex++;
    }
}

void getMaxPerson()
{
    uint8_t i = 0;
    promptInput(2 + maxPersonInput.currentIndex);
    Keyboard_getKey(&keyboard);
    if (keyboard.state != Released)
        return;
    if (maxPersonInput.currentIndex == 3)
    {
        if (keyboard.releasedKey == SK_Enter)
        {
            workState = WS_Weight;
            for (; i < 8; i++)
                displayBuffer[i] = weightPrompt[i];
        }
    } else
    {
        displayBuffer[2 + maxPersonInput.currentIndex] = castTable[keyboard.releasedKey];
        MaxPersonInput_append(&maxPersonInput, keyboard.releasedKey);
    }
}

void varifyPassword()
{
    uint8_t i = 0;
    uint8_t* targetPrompt;
    if (!passwordIsRight)
        targetPrompt = errorPrompt;
    else
    {
        workState = WS_Person;
        targetPrompt = personPrompt;
    }
    resetDelayDisappear();
    for (; i < 8; i++)
        displayBuffer[i] = targetPrompt[i];
}
